from plotting.compare import FT_PER_M, DEG_PER_RAD, SLUG_FT3_PER_KG_M3, KNOTS_PER_M_S

NED_VELOCITY = [
    {
        'nesc_col': 'feVelocity_ft_s_X',
        'sim_col': 'vx_ned_mps',
        'transform': lambda s: s * FT_PER_M,
        'ylabel': 'X Velocity NED [ft/s]',
    },
    {
        'nesc_col': 'feVelocity_ft_s_Y',
        'sim_col': 'vy_ned_mps',
        'transform': lambda s: s * FT_PER_M,
        'ylabel': 'Y Velocity NED [ft/s]',
    },
    {
        'nesc_col': 'feVelocity_ft_s_Z',
        'sim_col': 'vz_ned_mps',
        'transform': lambda s: s * FT_PER_M,
        'ylabel': 'Z Velocity NED [ft/s]',
    },
]

ANGULAR_RATE = [
    {
        'nesc_col': 'bodyAngularRateWrtEi_deg_s_Roll',
        'sim_col': 'roll_vel_rps',
        'transform': lambda s: s * DEG_PER_RAD,
        'ylabel': 'Roll Rate [deg/s]',
    },
    {
        'nesc_col': 'bodyAngularRateWrtEi_deg_s_Pitch',
        'sim_col': 'pitch_vel_rps',
        'transform': lambda s: s * DEG_PER_RAD,
        'ylabel': 'Pitch Rate [deg/s]',
    },
    {
        'nesc_col': 'bodyAngularRateWrtEi_deg_s_Yaw',
        'sim_col': 'yaw_vel_rps',
        'transform': lambda s: s * DEG_PER_RAD,
        'ylabel': 'Yaw Rate [deg/s]',
    },
]

INERTIAL_VELOCITY = [
    {
        'nesc_col': 'eiVelocity_ft_s_X',
        'sim_col': 'x_vel_mps',
        'transform': lambda s: -s * FT_PER_M,
        'ylabel': 'X Inertial Velocity [ft/s]',
    },
    {
        'nesc_col': 'eiVelocity_ft_s_Y',
        'sim_col': 'y_vel_mps',
        'transform': lambda s: s * FT_PER_M,
        'ylabel': 'Y Inertial Velocity [ft/s]',
    },
    {
        'nesc_col': 'eiVelocity_ft_s_Z',
        'sim_col': 'z_vel_mps',
        'transform': lambda s: s * FT_PER_M,
        'ylabel': 'Z Inertial Velocity [ft/s]',
    },
]

AIR_DATA = [
    {
      'nesc_col': 'trueAirspeed_nmi_h',
      'sim_col': 'true_airspeed_mps',
      'transform': lambda s: s * KNOTS_PER_M_S,
      'ylabel': 'True Airspeed [knots]'
    },
    {
        'nesc_col': 'speedOfSound_ft_s',
        'sim_col': 'speed_of_sound_mps',
        'transform': lambda s: s * FT_PER_M,
        'ylabel': 'Speed of Sound [ft/s]',
    },
    {
        'nesc_col': 'airDensity_slug_ft3',
        'sim_col': 'air_density',
        'transform': lambda s: s * SLUG_FT3_PER_KG_M3,
        'ylabel': 'Air Density [slug/ft^3]'
    },
    {
        'nesc_col': 'mach',
        'sim_col': 'mach',
        'transform': lambda s: s,
        'ylabel': 'Mach Number'
    }
]

EULER_ANGLE = [
    {
        'nesc_col': 'eulerAngle_deg_Roll',
        'sim_col': 'roll_angle',
        'transform': lambda s: s * DEG_PER_RAD,
        'ylabel': 'Roll Angle [deg]'
    },
    {
        'nesc_col': 'eulerAngle_deg_Pitch',
        'sim_col': 'pitch_angle',
        'transform': lambda s: s * DEG_PER_RAD,
        'ylabel': 'Pitch Angle [deg]'
    },
    {
        'nesc_col': 'eulerAngle_deg_Yaw',
        'sim_col': 'yaw_angle',
        'transform': lambda s: s * DEG_PER_RAD,
        'ylabel': 'Yaw Angle [deg]'
    }
]

EULER_ANGLE_2 = [
    {
        'nesc_col': 'eulerAngle_deg_Roll',
        'sim_col': 'roll_dcm_rad',
        'transform': lambda s: s * DEG_PER_RAD,
        'ylabel': 'Roll Angle [deg]'
    },
    {
        'nesc_col': 'eulerAngle_deg_Pitch',
        'sim_col': 'pitch_dcm_rad',
        'transform': lambda s: s * DEG_PER_RAD,
        'ylabel': 'Pitch Angle [deg]'
    },
    {
        'nesc_col': 'eulerAngle_deg_Yaw',
        'sim_col': 'yaw_dcm_rad',
        'transform': lambda s: s * DEG_PER_RAD,
        'ylabel': 'Yaw Angle [deg]'
    }
]