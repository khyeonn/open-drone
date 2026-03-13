import importlib
import argparse
import pandas as pd

from plotting.style import style_axis
from plotting.compare import plot_group, plot_group_with_difference
from plotting.groups import ANGULAR_RATE, INERTIAL_VELOCITY, NED_VELOCITY, AIR_DATA, EULER_ANGLE


def load_data(config, tol=1e-9):
    sim_data = pd.read_csv(config.ROOT_PATH + config.SIM_FILES[0], header=0)
    nesc_data = [pd.read_csv(config.ROOT_PATH + f, header=0) for f in config.CHECK_FILES]

    sim_data = sim_data.iloc[:-1].copy() # Remove last data points b/c it's always 0 (looks nicer :P)
    sim_data = sim_data.mask(sim_data.abs() < tol, 0)
    nesc_data = [df.mask(df.abs() < tol, 0) for df in nesc_data]

    return sim_data, nesc_data


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--case", default="case_01", help="NESC check case")
    args = parser.parse_args()

    config = importlib.import_module(f"nesc_cases.{args.case}.config")
    sim_data, nesc_data = load_data(config)

    plot_group_with_difference(
        sim_data=sim_data,
        nesc_data=nesc_data,
        specs=NED_VELOCITY,
        filename=config.ROOT_PATH + config.NED_VELOCITY_PNG,
        style_axis=style_axis,
    )
    
    plot_group_with_difference(
        sim_data=sim_data,
        nesc_data=nesc_data,
        specs=ANGULAR_RATE,
        filename=config.ROOT_PATH  + config.ANGULAR_RATE_PNG,
        style_axis=style_axis,
    )
    
    plot_group_with_difference(
        sim_data=sim_data,
        nesc_data=nesc_data,
        specs=EULER_ANGLE,
        filename=config.ROOT_PATH + config.EULER_ANGLE_PNG,
        style_axis=style_axis,
    )

    plot_group_with_difference(
        sim_data=sim_data,
        nesc_data=nesc_data,
        specs=INERTIAL_VELOCITY,
        filename=config.ROOT_PATH + config.INERTIAL_VELOCITY_PNG,
        style_axis=style_axis,
    )
    
    plot_group_with_difference(
        sim_data=sim_data,
        nesc_data=nesc_data,
        specs=AIR_DATA,
        filename=config.ROOT_PATH + config.AIR_DATA_PNG,
        style_axis=style_axis,
    )


if __name__ == "__main__":
    main()