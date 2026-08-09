FROM python:3.11-slim

# Tk is needed for matplotlib's interactive window (TkAgg) over forwarded X11
RUN apt-get update \
    && apt-get install -y --no-install-recommends python3-tk \
    && rm -rf /var/lib/apt/lists/*

RUN pip install --no-cache-dir pyserial numpy matplotlib

WORKDIR /workspace

ENV PYTHONUNBUFFERED=1

ENTRYPOINT ["python3", "visualize/imu_visualizer.py"]
