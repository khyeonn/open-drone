#!/usr/bin/env python3
"""Realtime IMU visualizer for the open-drone Pico firmware.

Reads the telemetry streamed by the Pico over USB serial (see
platform/pico/main.cpp) and renders the estimated attitude in 3D plus live
time-series of the raw accel / gyro / mag / temperature readings.

Data protocol (one CSV line per sample):
    q_w,q_x,q_y,q_z,accel_x_g,accel_y_g,accel_z_g,gyro_x_dps,gyro_y_dps,
    gyro_z_dps,mag_x_ut,mag_y_ut,mag_z_ut,temp_c

Non-numeric lines (boot messages, calibration progress, header) are skipped.

Modes:
    --source serial   read live from the USB port (default, auto-detected)
    --source file     replay a CSV recorded with --record
    --source sim      synthetic data, useful to test without hardware

GUI mode requires numpy + matplotlib + pyserial (installed in the devcontainer
venv). --headless mode only needs pyserial and is useful over SSH / for testing.
"""

from __future__ import annotations

import argparse
import math
import os
import queue
import sys
import threading
import time
from collections import deque

try:
    import serial
    import serial.tools.list_ports as list_ports
except ImportError:  # pragma: no cover - only when pyserial is missing
    serial = None

# ---------------------------------------------------------------------------
# Telemetry format
# ---------------------------------------------------------------------------

FIELDS = [
    "qw", "qx", "qy", "qz",
    "ax", "ay", "az",
    "gx", "gy", "gz",
    "mx", "my", "mz",
    "temp",
]

N_FIELDS = len(FIELDS)  # 14
Q_SLICE = slice(0, 4)
A_SLICE = slice(4, 7)
G_SLICE = slice(7, 10)
M_SLICE = slice(10, 13)
T_IDX = 13

RECORD_HEADER = "t_s," + ",".join(FIELDS)

AXIS_COLORS = {"x": "#ff6b6b", "y": "#6bff6b", "z": "#6baaff"}
TEMP_COLOR = "#ffd166"


def parse_line(line: str | bytes) -> tuple[float | None, list[float] | None]:
    """Parse one CSV line.

    Returns (timestamp, sample). ``timestamp`` is None for live serial lines and
    ``sample`` is None when the line isn't telemetry (boot messages, header, ...).
    """
    if isinstance(line, bytes):
        line = line.decode("utf-8", "replace")
    line = line.strip()
    if not line:
        return None, None
    try:
        values = [float(p) for p in line.split(",")]
    except ValueError:
        return None, None

    timestamp = None
    if len(values) == N_FIELDS + 1:  # recorded line with leading t_s
        timestamp = values[0]
        values = values[1:]
    elif len(values) == 4:  # quaternion-only line (older firmware)
        values = values + [0.0] * (N_FIELDS - 4)
    if len(values) != N_FIELDS:
        return None, None
    return timestamp, values


def quat_to_matrix(q: list[float]) -> list[list[float]]:
    """Hamilton-convention active rotation matrix from quaternion [w, x, y, z]."""
    w, x, y, z = q
    return [
        [1 - 2 * (y * y + z * z), 2 * (x * y - w * z), 2 * (x * z + w * y)],
        [2 * (x * y + w * z), 1 - 2 * (x * x + z * z), 2 * (y * z - w * x)],
        [2 * (x * z - w * y), 2 * (y * z + w * x), 1 - 2 * (x * x + y * y)],
    ]


def quat_to_euler(q: list[float]) -> tuple[float, float, float]:
    """Roll, pitch, yaw in degrees. Matches core/math/quaternion.hpp."""
    w, x, y, z = q
    roll = math.atan2(2 * (w * x + y * z), 1 - 2 * (x * x + y * y))
    pitch = math.asin(max(-1.0, min(1.0, 2 * (w * y - z * x))))
    yaw = math.atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z))
    return tuple(math.degrees(a) for a in (roll, pitch, yaw))


def mat_mul_vec(m: list[list[float]], v: list[float]) -> list[float]:
    return [m[i][0] * v[0] + m[i][1] * v[1] + m[i][2] * v[2] for i in range(3)]


def quat_mul(a: list[float], b: list[float]) -> list[float]:
    aw, ax, ay, az = a
    bw, bx, by, bz = b
    return [
        aw * bw - ax * bx - ay * by - az * bz,
        aw * bx + ax * bw + ay * bz - az * by,
        aw * by - ax * bz + ay * bw + az * bx,
        aw * bz + ax * by - ay * bx + az * bw,
    ]


def quat_from_euler(roll_deg, pitch_deg, yaw_deg) -> list[float]:
    """Compose a quaternion from ZYX (yaw->pitch->roll) euler angles."""
    r = math.radians(roll_deg) / 2
    p = math.radians(pitch_deg) / 2
    y = math.radians(yaw_deg) / 2
    cr, sr = math.cos(r), math.sin(r)
    cp, sp = math.cos(p), math.sin(p)
    cy, sy = math.cos(y), math.sin(y)
    qr = [cr, sr, 0, 0]
    qp = [cp, 0, sp, 0]
    qy = [cy, 0, 0, sy]
    q = quat_mul(quat_mul(qy, qp), qr)
    n = math.sqrt(sum(v * v for v in q))
    return [v / n for v in q]


def normalize(v: list[float]) -> list[float]:
    n = math.sqrt(sum(x * x for x in v))
    return [x / n for x in v] if n > 1e-8 else [0.0, 0.0, 0.0]


# ---------------------------------------------------------------------------
# Data sources
# ---------------------------------------------------------------------------


def _producer_thread(queue_out: queue.Queue, gen, stop_event: threading.Event) -> None:
    try:
        for sample in gen:
            queue_out.put(sample)
    except StopIteration:
        pass
    except Exception as exc:  # pragma: no cover - source-specific failures
        print(f"[visualizer] source error: {exc}", file=sys.stderr)
    finally:
        queue_out.put(None)  # end-of-stream sentinel
        stop_event.set()


class _SerialRecorder:
    def __init__(self, path: str | None):
        self.path = path
        self._fp = None
        if path:
            self._fp = open(path, "w", buffering=1)
            self._fp.write(RECORD_HEADER + "\n")
            print(f"[visualizer] recording telemetry to {path}")

    def write(self, sample: list[float]) -> None:
        if self._fp is None:
            return
        self._fp.write(f"{time.monotonic():.6f}," + ",".join(f"{v:.6f}" for v in sample) + "\n")


def _serial_group_hint(port: str) -> str:
    """Suggest the right group to join for serial access on this system.

    The group owning a USB serial device varies by distro (dialout on Debian,
    uucp on Arch, ...). Prefer the group that actually owns the device; fall
    back to whichever common candidates exist on this system.
    """
    try:
        import grp
        gid = os.stat(port).st_gid
        return grp.getgrgid(gid).gr_name
    except (ImportError, FileNotFoundError, KeyError, OSError):
        pass
    try:
        import grp
        for candidate in ("dialout", "uucp", "plugdev", "tty"):
            if candidate in {g.gr_name for g in grp.getgrall()}:
                return candidate
    except ImportError:
        pass
    return ""


def serial_gen(port: str, baud: int, recorder: _SerialRecorder):
    """Yield samples read from the Pico over USB serial, reconnecting on drop."""
    attempt = 0
    while True:
        try:
            print(f"[visualizer] opening {port} @ {baud} baud")
            ser = serial.Serial(port, baud, timeout=0.5)
            ser.reset_input_buffer()
            attempt = 0
            for raw in ser:
                if not raw:
                    continue
                _t, sample = parse_line(raw)
                if sample is not None:
                    recorder.write(sample)
                    yield sample
        except serial.SerialException as exc:
            attempt += 1
            if "Permission denied" in str(exc) and attempt == 1:
                group = _serial_group_hint(port)
                if group:
                    print(f"[visualizer] permission denied on the serial port - add your user to the "
                          f"'{group}' group (sudo usermod -aG {group} $USER) and re-login.",
                          file=sys.stderr)
                else:
                    print("[visualizer] permission denied on the serial port - run the visualizer as "
                          "root, or add your user to the group that owns the device "
                          "(e.g. sudo usermod -aG dialout $USER) and re-login.", file=sys.stderr)
            print(f"[visualizer] serial error: {exc} (attempt {attempt})", file=sys.stderr)
        time.sleep(1.0)
        if attempt > 5:
            print("[visualizer] giving up on serial port after repeated failures", file=sys.stderr)
            return


def file_gen(path: str, speed: float):
    """Yield samples replayed from a recorded CSV, paced to the original timing."""
    prev_t = None
    with open(path, "r", encoding="utf-8") as fp:
        for raw in fp:
            t, sample = parse_line(raw)
            if sample is None:
                continue
            if t is not None and prev_t is not None and t - prev_t > 0:
                time.sleep((t - prev_t) / speed)
            prev_t = t
            yield sample


def sim_gen(rate_hz: float = 20.0):
    """Yield synthetic IMU samples of a gently tumbling quadcopter."""
    period = 1.0 / rate_hz
    t = 0.0
    prev_euler = (0.0, 0.0, 0.0)
    while True:
        roll = 25.0 * math.sin(2 * math.pi * t / 4.0)
        pitch = 30.0 * math.sin(2 * math.pi * t / 5.0)
        yaw = (t / 20.0) * 360.0
        q = quat_from_euler(roll, pitch, yaw)

        gx = (roll - prev_euler[0]) / period
        gy = (pitch - prev_euler[1]) / period
        gz = (yaw - prev_euler[2]) / period
        prev_euler = (roll, pitch, yaw)

        r = quat_to_matrix(q)
        accel = [v + 0.01 * math.sin(2 * math.pi * 60 * t) for v in mat_mul_vec(r, [0, 0, 1])]
        mag = mat_mul_vec(r, [20, 5, 40])
        temp = 32.0 + 0.5 * math.sin(2 * math.pi * t / 60)

        yield [q[0], q[1], q[2], q[3], *accel, gx, gy, gz, *mag, temp]
        time.sleep(period)
        t += period


# ---------------------------------------------------------------------------
# Visualization (matplotlib)
# ---------------------------------------------------------------------------

BODY_ARMS = [
    ([0.0, 0.6], [0.0, 0.0], [0.0, 0.0]),
    ([0.0, -0.6], [0.0, 0.0], [0.0, 0.0]),
    ([0.0, 0.0], [0.0, 0.6], [0.0, 0.0]),
    ([0.0, 0.0], [0.0, -0.6], [0.0, 0.0]),
]
BODY_AXES = [
    ([0.0, 1.0], [0.0, 0.0], [0.0, 0.0], "x"),
    ([0.0, 0.0], [0.0, 1.0], [0.0, 0.0], "y"),
    ([0.0, 0.0], [0.0, 0.0], [0.0, 1.0], "z"),
]


def _rotated(r, xs, ys, zs):
    pts = [mat_mul_vec(r, [x, y, z]) for x, y, z in zip(xs, ys, zs)]
    return [p[0] for p in pts], [p[1] for p in pts], [p[2] for p in pts]


def build_figure():
    import matplotlib.pyplot as plt

    fig = plt.figure(figsize=(13, 8), facecolor="#111111")
    fig.suptitle("open-drone IMU", color="white", fontsize=14)

    gs = fig.add_gridspec(3, 2, height_ratios=[2, 1, 1], width_ratios=[2, 1], hspace=0.45, wspace=0.28)
    ax3d = fig.add_subplot(gs[0, 0], projection="3d")
    axmag = fig.add_subplot(gs[0, 1])
    axacc = fig.add_subplot(gs[1, :])
    axgyro = fig.add_subplot(gs[2, :])
    axtemp = axmag.twinx()

    for ax in (axmag, axacc, axgyro):
        ax.set_facecolor("#111111")
        ax.grid(True, color="#333333")
        ax.tick_params(colors="white", labelsize=8)
        ax.xaxis.label.set_color("white")
        ax.yaxis.label.set_color("white")
    for ax in (ax3d, axtemp):
        ax.set_facecolor("#111111")

    # ---- 3D attitude panel ------------------------------------------------
    for axis in range(3):
        seg = [[0, 0], [0, 0], [0, 0]]
        seg[axis] = [0.0, 1.0]
        ax3d.plot(seg[0], seg[1], seg[2], color="#666666", lw=0.8, alpha=0.5)

    body_lines = []
    for x, y, z in BODY_ARMS:
        (ln,) = ax3d.plot(x, y, z, color="white", lw=2.5)
        body_lines.append(ln)
    for x, y, z, axis in BODY_AXES:
        (ln,) = ax3d.plot(x, y, z, color=AXIS_COLORS[axis], lw=2.5)
        body_lines.append(ln)

    ax3d.set_xlim(-1, 1)
    ax3d.set_ylim(-1, 1)
    ax3d.set_zlim(-1, 1)
    ax3d.set_box_aspect((1, 1, 1))
    ax3d.view_init(elev=30, azim=-60)
    ax3d.set_xticks([])
    ax3d.set_yticks([])
    ax3d.set_zticks([])
    ax3d.set_title("Attitude", color="white", fontsize=11)
    ax3d.set_facecolor("#111111")

    # ---- scrolling time series -------------------------------------------
    acc_lines = [axacc.plot([], [], color=AXIS_COLORS[a], lw=1.4, label=f"a{a} (g)")[0] for a in "xyz"]
    gyro_lines = [axgyro.plot([], [], color=AXIS_COLORS[a], lw=1.4, label=f"g{a} (deg/s)")[0] for a in "xyz"]
    mag_lines = [axmag.plot([], [], color=AXIS_COLORS[a], lw=1.4, label=f"m{a} (uT)")[0] for a in "xyz"]
    (temp_line,) = axtemp.plot([], [], color=TEMP_COLOR, lw=1.6, label="temp (deg C)")

    for ax, title in ((axacc, "Accelerometer"), (axgyro, "Gyroscope"), (axmag, "Magnetometer")):
        ax.set_title(title, color="white", fontsize=11)
    axtemp.set_ylabel("deg C", color=TEMP_COLOR, fontsize=8)
    axtemp.tick_params(axis="y", labelcolor=TEMP_COLOR, labelsize=8)

    for ax in (axacc, axgyro, axmag):
        ax.legend(loc="upper right", fontsize=7, facecolor="#111111", labelcolor="white", framealpha=0.6)

    return fig, ax3d, axacc, axgyro, axmag, axtemp, acc_lines, gyro_lines, mag_lines, temp_line, body_lines


def _configure_gui_backend() -> None:
    """Pick an interactive matplotlib backend when a display is available.

    Container images often ship matplotlib without a GUI toolkit, so matplotlib
    defaults to the non-interactive 'agg' backend and plt.show() would be a
    no-op. Try the common interactive backends until one imports cleanly.
    """
    if not os.environ.get("DISPLAY"):
        return
    import matplotlib

    for backend in ("TkAgg", "QtAgg", "GTK3Agg"):
        try:
            matplotlib.use(backend)
            return
        except Exception:
            continue


def _build_gui(samples: queue.Queue, window_s: float, fps: int):
    import matplotlib.pyplot as plt
    from matplotlib.animation import FuncAnimation

    (fig, ax3d, axacc, axgyro, axmag, axtemp,
     acc_lines, gyro_lines, mag_lines, temp_line, body_lines) = build_figure()

    start = time.monotonic()
    times: deque[float] = deque()
    buf: deque[list[float]] = deque()
    maxlen = max(2, int(window_s * fps))

    def on_sample(values: list[float]) -> None:
        times.append(time.monotonic() - start)
        buf.append(values)
        if len(times) > maxlen:
            times.popleft()
            buf.popleft()

    def update(_frame) -> None:
        try:
            while True:
                values = samples.get_nowait()
                if values is None:  # end of stream
                    return
                on_sample(values)
        except queue.Empty:
            pass
        if not buf:
            return

        q = buf[-1][Q_SLICE]
        r = quat_to_matrix(q)
        for ln, (x, y, z) in zip(body_lines[:4], BODY_ARMS):
            ln.set_data_3d(*_rotated(r, x, y, z))
        for ln, (x, y, z, _axis) in zip(body_lines[4:], BODY_AXES):
            ln.set_data_3d(*_rotated(r, x, y, z))

        roll, pitch, yaw = quat_to_euler(q)
        ax3d.set_title(
            f"Attitude    roll {roll:+7.1f} deg    pitch {pitch:+7.1f} deg    yaw {yaw:+7.1f} deg",
            color="white", fontsize=11,
        )

        t_win = [t - times[-1] for t in times]
        for lines, sl in ((acc_lines, A_SLICE), (gyro_lines, G_SLICE), (mag_lines, M_SLICE)):
            for i, ln in enumerate(lines):
                ln.set_data(t_win, [s[sl.start + i] for s in buf])

        temp_line.set_data(t_win, [s[T_IDX] for s in buf])
        for ax in (axacc, axgyro, axmag, axtemp):
            ax.relim()
            ax.autoscale_view(scalex=False)
        for ax in (axacc, axgyro, axmag):
            ax.set_xlim(t_win[-1] - window_s, t_win[-1])

    try:
        anim = FuncAnimation(fig, update, interval=1000 / fps, blit=False, cache_frame_data=False)
    except TypeError:  # older matplotlib without cache_frame_data
        anim = FuncAnimation(fig, update, interval=1000 / fps, blit=False)
    return fig, update, anim


def run_gui(
    samples: queue.Queue,
    window_s: float = 30.0,
    fps: int = 20,
):
    import matplotlib.pyplot as plt

    fig, update, anim = _build_gui(samples, window_s, fps)
    try:
        plt.show()
    finally:
        if anim is not None and anim.event_source is not None:
            anim.event_source.stop()


# ---------------------------------------------------------------------------
# Headless mode (no matplotlib needed) - useful over SSH / for testing
# ---------------------------------------------------------------------------


def run_headless(samples: queue.Queue, label: str) -> None:
    n = 0
    t0 = time.monotonic()
    print(f"[visualizer] headless mode, consuming {label} samples", flush=True)
    try:
        while True:
            try:
                values = samples.get(timeout=0.5)
            except queue.Empty:
                continue
            if values is None:  # end of stream
                break
            n += 1
            if n % 50 == 0:
                r, p, y = quat_to_euler(values[Q_SLICE])
                rate = n / (time.monotonic() - t0)
                print(f"  {n:6d} samples | {rate:5.1f} Hz | roll {r:+6.1f} pitch {p:+6.1f} yaw {y:+6.1f}",
                      flush=True)
    except KeyboardInterrupt:
        pass
    dt = time.monotonic() - t0
    print(f"[visualizer] consumed {n} samples in {dt:.1f} s ({n / max(dt, 1e-6):.1f} Hz)", flush=True)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------


def _find_pico_port() -> str | None:
    candidates = []
    for port in list_ports.comports():
        if port.vid == 0x2E8A:  # Raspberry Pi
            return port.device
        if port.device.startswith("/dev/ttyACM") or port.device.startswith("/dev/ttyUSB"):
            candidates.append(port.device)
    return candidates[0] if candidates else None


def main(argv: list[str] | None = None) -> int:
    if serial is None:
        print("pyserial is required. Install it (in the devcontainer venv: pip install pyserial).",
              file=sys.stderr)
        return 2

    parser = argparse.ArgumentParser(description="Realtime IMU visualizer for the open-drone Pico.")
    parser.add_argument("--source", choices=["serial", "file", "sim"], default="serial",
                        help="data source (default: serial)")
    parser.add_argument("--port", default=None, help="serial port (default: auto-detect Pico)")
    parser.add_argument("--baud", type=int, default=115200, help="serial baud rate (default: 115200)")
    parser.add_argument("--file", default=None, help="CSV file to replay (with --source file)")
    parser.add_argument("--record", default=None, help="log incoming telemetry to a CSV file")
    parser.add_argument("--speed", type=float, default=1.0, help="file replay speed multiplier")
    parser.add_argument("--window", type=float, default=30.0, help="scrolling window length in seconds")
    parser.add_argument("--fps", type=int, default=20, help="plot refresh rate")
    parser.add_argument("--headless", action="store_true",
                        help="don't open a window; just consume and report (no matplotlib needed)")
    args = parser.parse_args(argv)

    if args.source == "serial":
        if args.port is None:
            args.port = _find_pico_port()
            if args.port is None:
                print("Could not find a serial device. Pass --port /dev/ttyACM0 (or add your user to the "
                      "'dialout' group).", file=sys.stderr)
                return 1
        recorder = _SerialRecorder(args.record)
        gen = serial_gen(args.port, args.baud, recorder)
    elif args.source == "file":
        if not args.file:
            print("--file is required with --source file", file=sys.stderr)
            return 1
        gen = file_gen(args.file, args.speed)
    else:
        if args.record:
            print("warning: --record is ignored in sim mode", file=sys.stderr)
        gen = sim_gen()

    samples: queue.Queue = queue.Queue(maxsize=1000)
    stop = threading.Event()
    thread = threading.Thread(target=_producer_thread, args=(samples, gen, stop), daemon=True)
    thread.start()

    try:
        if args.headless:
            run_headless(samples, args.source)
        else:
            _configure_gui_backend()
            run_gui(samples, window_s=args.window, fps=args.fps)
    except KeyboardInterrupt:
        pass
    finally:
        stop.set()
    return 0


if __name__ == "__main__":
    sys.exit(main())
