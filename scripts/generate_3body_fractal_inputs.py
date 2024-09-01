from pathlib import Path

import numpy as np
from tqdm import tqdm

# Constants
GRAVITY_CONSTANT = 39.4781201869325428788215504027903079986572265625  # in solsys units

# Parameters
OUTPUT_FILE_PATH_BASE = "input/3body-fractal/3body-fractal"
UNIT_SYSTEM = "solsys"
MASS = 1
BINARY_RADIUS = 0.5
DISTANCE = 50
IMPACT_STEPS = 101
PHASE_STEPS = 101
IMPACT_RANGE = (-4.5, 7.5)
PHASE_RANGE = (0, np.pi)


def main() -> None:
    impact_values = np.linspace(*IMPACT_RANGE, IMPACT_STEPS)
    phase_values = np.linspace(*PHASE_RANGE, PHASE_STEPS)

    output_file_path_base = Path(OUTPUT_FILE_PATH_BASE)
    output_file_dir_path = output_file_path_base.parent
    output_file_name_base = output_file_path_base.stem

    num_files = IMPACT_STEPS * PHASE_STEPS
    file_index = 0
    progress_bar = tqdm(total=num_files, desc="Generating input files")

    for impact in impact_values:
        for phase in phase_values:
            file_name = f"{output_file_name_base}_{file_index}.txt"

            generate_input_file(
                impact,
                phase,
                MASS,
                BINARY_RADIUS,
                DISTANCE,
                UNIT_SYSTEM,
                output_file_dir_path / file_name,
            )

            file_index += 1
            progress_bar.update()


def generate_input_file(
    impact: float,
    phase: float,
    mass: float,
    binary_radius: float,
    distance: float,
    unit_system: str,
    file_path: Path,
) -> None:
    sin_phase = np.sin(phase)
    cos_phase = np.cos(phase)
    binary_speed = 0.5 * np.sqrt(GRAVITY_CONSTANT * mass / binary_radius)

    # Binary star 1 position and velocity
    binary1_pos = np.array([cos_phase, sin_phase]) * binary_radius
    binary1_vel = np.array([-sin_phase, cos_phase]) * binary_speed

    # Binary star 2 position and velocity
    binary2_pos = -binary1_pos
    binary2_vel = -binary1_vel

    # Incoming star position and velocity
    incoming_pos, incoming_vel = get_incoming_pos_vel(
        mass, binary_radius, impact, distance
    )

    output_array = np.zeros((3, 5))

    output_array[:, 0] = mass

    output_array[0, 1:3] = binary1_pos
    output_array[1, 1:3] = binary2_pos
    output_array[2, 1:3] = incoming_pos

    output_array[0, 3:5] = binary1_vel
    output_array[1, 3:5] = binary2_vel
    output_array[2, 3:5] = incoming_vel

    file_path.parent.mkdir(parents=True, exist_ok=True)
    np.savetxt(file_path, output_array, header=unit_system, comments="")


def get_incoming_pos_vel(
    mass: float, binary_radius: float, impact: float, distance: float
) -> tuple[np.ndarray, np.ndarray]:
    # Standard gravitational parameter for the "2-body" system
    std_gravity_param = 3 * GRAVITY_CONSTANT * mass

    # Semi major axis of the binary star system
    semi_major_axis = 2 * binary_radius

    # Critical velocity for incoming star
    critical_velocity = np.sqrt(std_gravity_param / (2 * semi_major_axis))

    # Velocity of incoming star at infinite distance
    inf_velocity = 0.5 * critical_velocity

    # Absolute value of the impact parameter for simpler calculation
    # (the sign will be considered later)
    abs_impact = np.abs(impact)

    # Hyperbolic semi major axis of the "2-body" system
    semi_major_axis_2body = -std_gravity_param / inf_velocity**2

    # Utility variables
    c = np.sqrt(semi_major_axis_2body**2 + abs_impact**2)
    theta = np.arctan(-abs_impact / semi_major_axis_2body)
    sin_theta = np.sin(theta)
    cos_theta = np.cos(theta)

    # Parameters of quadratic solution for (absolute) y-coordinate of incoming star
    A = (sin_theta / semi_major_axis_2body) ** 2 - (cos_theta / abs_impact) ** 2
    B = (
        -2
        * sin_theta
        * (
            (distance * cos_theta + c) / semi_major_axis_2body**2
            + distance * cos_theta / abs_impact**2
        )
    )
    C = (
        ((distance * cos_theta + c) / semi_major_axis_2body) ** 2
        - (distance * sin_theta / abs_impact) ** 2
        - 1
    )

    # Negative term of quadratic solution for (absolute) y-coordinate of incoming star
    pos_y_abs = (-B - np.sqrt(B**2 - 4 * A * C)) / (2 * A)

    # Actual distance between the two "stars" of the "2-body" system
    distance_2body = np.sqrt(pos_y_abs**2 + distance**2)

    # Initial speed of incoming star
    speed = np.sqrt(
        std_gravity_param * (2 / distance_2body - 1 / semi_major_axis_2body)
    )

    # Utility variables
    X = -pos_y_abs * sin_theta + distance * cos_theta + c
    gamma = np.arctan(
        abs_impact
        / semi_major_axis_2body**2
        * X
        / np.sqrt((X / semi_major_axis_2body) ** 2 - 1)
    )
    vel_angle = gamma - theta + np.pi

    # True y-coordinate of incoming star
    pos_y = np.sign(impact) * pos_y_abs

    pos_incoming = np.array([distance, pos_y])
    vel_incoming = (
        np.array([np.cos(vel_angle), np.sin(vel_angle) * np.sign(impact)]) * speed
    )

    return pos_incoming, vel_incoming


if __name__ == "__main__":
    main()
