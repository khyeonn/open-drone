CASE_NAME = 'Constant Mass, Dragless Sphere'
ROOT_PATH = './sim/validation/nesc_cases/case_01/'

NED_VELOCITY_PNG = 'plots/case_01_ned_velocity.png'
INERTIAL_VELOCITY_PNG = 'plots/case_01_inertial_velocity.png'
ANGULAR_RATE_PNG = 'plots/case_01_angular_rate.png'
EULER_ANGLE_PNG = 'plots/case_01_euler_angle.png'
AIR_DATA_PNG = 'plots/case_01_air_data.png'

SIM_FILES = [
    'sim_results_csv/sim_data_01.csv'
]

CHECK_FILES = [
    'check_cases/Atmos_01_sim_01.csv',
    'check_cases/Atmos_01_sim_02.csv',
    'check_cases/Atmos_01_sim_03.csv',
    'check_cases/Atmos_01_sim_04.csv',
    'check_cases/Atmos_01_sim_05.csv',
    'check_cases/Atmos_01_sim_06.csv',
]
