CASE_NAME = 'Tumbling Brick with Rotational Damping'
ROOT_PATH = './sim/validation/nesc_cases/case_03/'

NED_VELOCITY_PNG = 'plots/case_03_ned_velocity.png'
INERTIAL_VELOCITY_PNG = 'plots/case_03_inertial_velocity.png'
ANGULAR_RATE_PNG = 'plots/case_03_angular_rate.png'
EULER_ANGLE_PNG = 'plots/case_03_euler_angle.png'
AIR_DATA_PNG = 'plots/case_03_air_data.png'

SIM_FILES = [
    'sim_results_csv/sim_data_03.csv'
]

CHECK_FILES = [
    'check_cases/Atmos_03_sim_01.csv',
    'check_cases/Atmos_03_sim_02.csv',
    'check_cases/Atmos_03_sim_04.csv',
    'check_cases/Atmos_03_sim_05.csv',
    'check_cases/Atmos_03_sim_06.csv',
]
