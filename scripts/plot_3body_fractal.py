from pathlib import Path

import matplotlib.pyplot as plt
import natsort
import numpy as np
from tqdm import tqdm

# Parameters
PLOT_FILE_PATH = "output/plots/3body-fractal.png"
IMPACT_STEPS = 301
PHASE_STEPS = 301
IMPACT_RANGE = (-4.5, 7.5)
PHASE_RANGE = (0, np.pi)


def main() -> None:
    sim_files_dir = input(
        "Enter simulation file directory ('outputDir' in config.txt): "
    )

    sim_file_paths = natsort.natsorted(Path(sim_files_dir).iterdir())
    num_simulations = IMPACT_STEPS * PHASE_STEPS

    if len(sim_file_paths) != num_simulations:
        raise ValueError(
            f"Number of simulation files ({len(sim_file_paths)}) does not match "
            f"expected number of simulations ({num_simulations})"
        )

    plot_file_path = Path(PLOT_FILE_PATH)
    plot_file_path.parent.mkdir(parents=True, exist_ok=True)

    image_array = generate_image_array(sim_file_paths)

    plot_image(image_array, IMPACT_RANGE, PHASE_RANGE, plot_file_path)


def generate_image_array(sim_file_paths: list[Path]) -> np.ndarray:
    image_array = np.zeros((IMPACT_STEPS, PHASE_STEPS, 3))

    plot_file_path = Path(PLOT_FILE_PATH)
    plot_file_path.parent.mkdir(parents=True, exist_ok=True)

    for file_index, file_path in enumerate(
        tqdm(sim_file_paths, desc="Processing simulation files")
    ):
        x = file_index % PHASE_STEPS
        y = int(file_index / PHASE_STEPS)

        sim_data = np.genfromtxt(file_path, delimiter=",")

        # This indicates an incomplete simulation due to maxIterations being reached
        if sim_data[-1, 0] == -1:
            image_array[x, y] = 1
            continue

        ejected_star_index, deflection_angle = get_ejected_star_deflection_angle(
            sim_data
        )

        image_array[x, y, ejected_star_index] = 0.5 * (np.cos(deflection_angle) + 1)

    return image_array


def get_ejected_star_deflection_angle(sim_data: np.ndarray) -> tuple[int, float]:
    before_final_positions = sim_data[-2, 4:10].reshape(-1, 2).T
    final_positions = sim_data[-1, 4:10].reshape(-1, 2).T

    before_final_pos_1 = before_final_positions[:, 0]
    before_final_pos_2 = before_final_positions[:, 1]
    before_final_pos_3 = before_final_positions[:, 2]

    final_pos_1 = final_positions[:, 0]
    final_pos_2 = final_positions[:, 1]
    final_pos_3 = final_positions[:, 2]

    distance_12 = np.sqrt(((final_pos_1 - final_pos_2) ** 2).sum())
    distance_13 = np.sqrt(((final_pos_1 - final_pos_3) ** 2).sum())
    distance_23 = np.sqrt(((final_pos_2 - final_pos_3) ** 2).sum())

    min_distance_index = int(np.argmin([distance_12, distance_13, distance_23]))
    ejected_star_index = 2 - min_distance_index

    if min_distance_index == 0:
        before_deflected_pos = before_final_pos_3
        deflected_pos = final_pos_3
    elif min_distance_index == 1:
        before_deflected_pos = before_final_pos_2
        deflected_pos = final_pos_2
    else:
        before_deflected_pos = before_final_pos_1
        deflected_pos = final_pos_1

    # Amplify the final movement direction of the deflected star by 10000 to make
    # the deflection angle approximation more accurate
    deflected_pos = deflected_pos + (deflected_pos - before_deflected_pos) * 10000
    deflected_pos_abs = np.sqrt((deflected_pos**2).sum())
    deflection_angle = np.arccos(deflected_pos[0] / deflected_pos_abs)

    return ejected_star_index, deflection_angle


def plot_image(
    image_array: np.ndarray,
    impact_range: tuple[float, float],
    phase_range: tuple[float, float],
    file_path: Path,
) -> None:
    aspect = (phase_range[1] - phase_range[0]) / (impact_range[1] - impact_range[0])

    plt.style.use("dark_background")
    fig, ax = plt.subplots(1, 1, figsize=(6, 6), dpi=150)

    ax.imshow(
        np.rot90(image_array), extent=(*phase_range, *impact_range), aspect=aspect
    )

    ax.set_xlabel("Phase angle")
    ax.set_ylabel("Impact parameter")

    fig.savefig(file_path, dpi=600, bbox_inches="tight")
    plt.show()


if __name__ == "__main__":
    main()
