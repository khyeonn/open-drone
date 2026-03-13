def style_axis(ax):
    ax.set_facecolor('black')
    ax.grid(True)
    ax.tick_params(colors='white')
    ax.set_xlabel('Time [s]', color='white')
    ax.legend(
        bbox_to_anchor=(1.05, 1),
        loc='upper left',
        fontsize=6,
        facecolor='none',
        labelcolor='white',
        frameon=False
    )

def style_figure(fig):
    fig.set_facecolor('black')