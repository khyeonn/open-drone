import math
import matplotlib.pyplot as plt
import numpy as np
from plotting.style import style_figure

FT_PER_M = 3.280839895
DEG_PER_RAD = 180.0 / math.pi
SLUG_FT3_PER_KG_M3 = 0.00194032
KNOTS_PER_M_S = 1.94384


def identity(x):
    return x


def plot_nesc_series(ax, nesc_data, nesc_col):
    for i, data in enumerate(nesc_data):
        if nesc_col not in data.columns:
            print(f'NESC dataset 0{i + 1}.csv missing column: {nesc_col}')
            continue

        ax.plot(
            data['time'],
            data[nesc_col],
            label=f'NESC Data 0{i + 1}'
        )


def plot_sim_series(ax, sim_data, sim_col, transform=identity,
                    label='C++ Sim', color='magenta', linestyle='dashed'):
    if sim_col not in sim_data.columns:
        print(f'Sim data missing column: {sim_col}')
        return

    ax.plot(
        sim_data['time'],
        transform(sim_data[sim_col]),
        color=color,
        linestyle=linestyle,
        label=label
    )
    

def plot_comparison_axis(ax, sim_data, nesc_data, spec, style_axis):
    plot_nesc_series(ax, nesc_data, spec['nesc_col'])
    plot_sim_series(ax, sim_data, spec['sim_col'], spec.get('transform', identity))
    style_axis(ax)
    ax.set_ylabel(spec['ylabel'], color='white')
    

def plot_group(sim_data, nesc_data, specs, nrows, ncols, filename, style_axis, figsize=(12, 8)):
    fig, axes = plt.subplots(nrows, ncols, figsize=figsize)
    style_figure(fig)

    axes_flat = axes.flat if hasattr(axes, 'flat') else [axes]

    for ax, spec in zip(axes_flat, specs['series']):
        plot_comparison_axis(ax, sim_data, nesc_data, spec, style_axis)

    # hide unused axes
    for ax in list(axes_flat)[len(specs):]:
        ax.set_visible(False)

    plt.tight_layout()
    plt.savefig(filename)
    plt.close()
    print(f'Figures saved to {filename}')
    
    
def plot_difference_series(ax, sim_data, nesc_data, spec):
    if spec['sim_col'] not in sim_data.columns:
        print(f"Sim data missing column: {spec['sim_col']}")
        return

    sim_time = sim_data['time'].to_numpy()
    sim_series = spec.get('transform', identity)(sim_data[spec['sim_col']]).to_numpy()

    for i, data in enumerate(nesc_data):
        if spec['nesc_col'] not in data.columns:
            print(f"NESC dataset 0{i + 1}.csv missing column: {spec['nesc_col']}")
            continue

        nesc_time = data['time'].to_numpy()
        nesc_series = data[spec['nesc_col']].to_numpy()

        # Interpolate NESC onto sim time base
        nesc_interp = np.interp(sim_time, nesc_time, nesc_series)

        diff = nesc_interp - sim_series

        ax.plot(
            sim_time,
            diff,
            label=f'NESC 0{i + 1} - Sim'
        )

    ax.axhline(0.0, color='white', linewidth=0.75, linestyle=':')
    
    
def plot_difference_axis(ax, sim_data, nesc_data, spec, style_axis):
    plot_difference_series(ax, sim_data, nesc_data, spec)
    style_axis(ax)
    ax.set_ylabel(f"Δ{spec['ylabel']}", color='white')
    
    
def plot_group_with_difference(sim_data, nesc_data, specs, filename, style_axis, case_num, figsize=(12, 8)):
    fig, axes = plt.subplots(len(specs['series']), 2, figsize=figsize)
    fig.suptitle(fr"NESC Atmospheric Check Case {case_num} - {specs['title']}", fontsize=14, y=0.93, fontweight='normal', color='white')
    style_figure(fig)

    if len(specs['series']) == 1:
        axes = [axes]

    for row, spec in enumerate(specs['series']):
        ax_main = axes[row][0]
        ax_diff = axes[row][1]

        plot_comparison_axis(ax_main, sim_data, nesc_data, spec, style_axis)
        plot_difference_axis(ax_diff, sim_data, nesc_data, spec, style_axis)

    plt.tight_layout(rect=[0.02, 0.02, 0.97, 0.90])
    plt.savefig(filename, bbox_inches="tight", pad_inches=0.35)
    plt.close()
    print(f'Figures saved to {filename}')