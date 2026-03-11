import importlib
import argparse
import matplotlib.pyplot as plt
from utils.maps import label_map
import pandas as pd

def style_axis(ax):
    ax.set_facecolor('black')
    ax.grid(True)
    ax.tick_params(colors='white')
    ax.set_xlabel('Time [s]', color='white')

parser = argparse.ArgumentParser()
parser.add_argument("--case", default="case_01", help="NESC check case")
args = parser.parse_args()

config = importlib.import_module(f"nesc_cases.{args.case}.config")

sim_data_01 = pd.read_csv(config.ROOT_PATH + config.SIM_FILES[0], header=0)
check_data = [pd.read_csv(config.ROOT_PATH + f, header=0) for f in config.CHECK_FILES]
sim_data = {k: v[:-1] for k, v in sim_data_01.items()}
  
############# Plotting NESC check case vs. C++ simulation ################
fig, axes = plt.subplots(2, 4, figsize=(10, 6))
fig.set_facecolor('black')

# Altitude
for i, data in enumerate(check_data):
    axes[0, 0].plot(data['time'], data['altitudeMsl_ft'], linewidth=1, alpha=0.5, label=f'NESC Data 0{i + 1}') 
    
axes[0, 0].plot(sim_data['time'], sim_data['altitude_m']*3.28, linewidth=1, color='magenta', label="C++ Sim", linestyle='dashed')
style_axis(axes[0, 0])
axes[0, 0].set_ylabel('Altitude from Mean Sea Level [ft]', color='white')
axes[0, 0].legend(loc='lower left', fontsize=6, facecolor='none', labelcolor='white', frameon=False)

# Mach number
for i, data in enumerate(check_data):
    if i == 2: continue # Data is missing for file
    
    axes[0, 1].plot(data['time'], data['mach'], linewidth=1, label=f'NESC Data 0{i + 1}') 
    
axes[0, 1].plot(sim_data['time'], sim_data['mach'], linewidth=1, color='magenta', label="C++ Sim", linestyle='dashed')  
style_axis(axes[0, 1]) 
axes[0, 1].set_ylabel('Mach', color='white')
axes[0, 1].legend(loc='best', fontsize=6, facecolor='none', labelcolor='white', frameon=False)



plt.tight_layout()
plt.savefig(config.ROOT_PATH + 'plots/' + config.CHECK_CASES_PNG)
plt.close()
print(f'Figures saved to {config.ROOT_PATH + 'plots/' + config.CHECK_CASES_PNG}')

########## END SECTION ##########



########## Plot C++ simulation data #########

plot_data = {
    (0, 0, 'x_vel_mps'),
    (0, 1, 'y_vel_mps'),
    (0, 2, 'z_vel_mps'),
    (1, 0, 'roll_angle'),
    (1, 1, 'pitch_angle'),
    (1, 2, 'yaw_angle'),
    (2, 0, 'x_pos_ned',),
    (2, 1, 'y_pos_ned'),
    (2, 2, 'z_pos_ned')
}

fig, axes = plt.subplots(3, 3, figsize=(10, 6))
fig.set_facecolor('black')

for row, col, data in plot_data:
    if data == 'z_pos_ned':
        axes[row, col].plot(sim_data_01['time'], -sim_data_01[data], linewidth=1)
    else:
        axes[row, col].plot(sim_data_01['time'], sim_data_01[data], linewidth=1)
    style_axis(axes[row, col])
    axes[row, col].set_ylabel(label_map[data], color='white')

plt.tight_layout()
plt.savefig(config.ROOT_PATH + 'plots/' + config.SIM_RESULTS_PNG)
plt.close()
print(f'Figures saved to {config.ROOT_PATH + 'plots/' + config.SIM_RESULTS_PNG}\n')

######### END SECTION ###########