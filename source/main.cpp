#include "config.hpp"
#include "util.hpp"
#include "particle_system.hpp"

#include <omp.h>
#include <iostream>
#include <filesystem>

#define CONFIG_PATH "../resources/config.txt"

int main() {
    const Config config = Config::load(CONFIG_PATH);

    std::cout << "Simulations started at: " << getDateTimeString(false, 0) << std::endl;
    std::cout << "Output directory: " << config.outputDirPath << '\n' << std::endl;

    const std::filesystem::directory_iterator dir_iterator(config.inputFilesDirPath);

    std::vector<std::filesystem::directory_entry> dir_entries;

    for (const auto& dir_entry : dir_iterator) {
        dir_entries.push_back(dir_entry);
    }

    int progress = 0;

#pragma omp parallel for
    for (const auto& dir_entry : dir_entries) {
        if (dir_entry.is_directory()) continue;

        ParticleSystem particleSystem(dir_entry.path(), config.unitSystem);

        particleSystem.simulate(config.fixedTimeStep, config.outputDirPath,
                                config.enableAdaptiveTimeStep, config.maxVelocityStep,
                                config.maxTime, config.maxIterations,
                                config.writeStatePeriod, config.integrationMethod);

#pragma omp critical
        {
            progress++;

            std::cout << "Progress: " << progress << "/" << dir_entries.size() << " ("
                      << static_cast<double>(progress)
                    / static_cast<double>(dir_entries.size()) * 100.0
                      << " %)" << std::endl;
        }
    }

    return 0;
}
