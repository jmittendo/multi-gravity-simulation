#include "config.hpp"
#include "util.hpp"
#include "particle_system.hpp"

#include <iostream>
#include <filesystem>
#include <memory>

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

    const std::shared_ptr<const UnitSystem> sharedUnitSystem
        = std::make_shared<const UnitSystem>(config.unitSystem);
    int progress = 0;

#ifdef _OPENMP
    #pragma omp parallel for
#endif
    for (const auto& dir_entry : dir_entries) {
        if (dir_entry.is_directory()) continue;

        ParticleSystem particleSystem(dir_entry.path(), sharedUnitSystem);

        particleSystem.simulate(config.fixedTimeStep, config.outputDirPath,
                                config.enableAdaptiveTimeStep, config.maxVelocityStep,
                                config.maxTime, config.maxIterations,
                                config.writeStatePeriod, config.integrationMethod);

#ifdef _OPENMP
    #pragma omp critical
#endif
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
