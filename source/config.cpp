#include "config.hpp"
#include "util.hpp"
#include "constants.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

static std::map<std::string, std::string>
getConfigMap(const std::filesystem::path& configPath);

Config::Config(const UnitSystem& unitSystem, const std::filesystem::path& outputDirPath,
               const std::filesystem::path& inputFilesDirPath,
               const double fixedTimeStep, const double maxVelocityStep,
               const bool enableAdaptiveTimeStep, const double maxTime,
               const unsigned long maxIterations, const double writeStatePeriod,
               const std::string& integrationMethod)
    : unitSystem(unitSystem)
    , outputDirPath(outputDirPath)
    , inputFilesDirPath(inputFilesDirPath)
    , fixedTimeStep(fixedTimeStep)
    , maxVelocityStep(maxVelocityStep)
    , enableAdaptiveTimeStep(enableAdaptiveTimeStep)
    , maxTime(maxTime)
    , maxIterations(maxIterations)
    , writeStatePeriod(writeStatePeriod)
    , integrationMethod(integrationMethod) {
}

Config Config::load(const std::filesystem::path& configPath) {
    const std::map<std::string, std::string> configMap = getConfigMap(configPath);

    const UnitSystem unitSystem(configMap.at("unitSystem"));
    const std::filesystem::path outputDirPath = configMap.at("outputDir");
    const std::filesystem::path inputFilesDirPath = configMap.at("inputFilesDir");
    const double fixedTimeStep = stod(configMap.at("fixedTimeStep"));
    const double maxVelocityStep = stod(configMap.at("maxVelocityStep"));
    const bool enableAdaptiveTimeStep
        = parseBoolString(configMap.at("enableAdaptiveTimeStep"));
    const double maxTime = stod(configMap.at("maxTime"));
    const double maxIterations = stoul(configMap.at("maxIterations"));
    const double writeStatePeriod = stod(configMap.at("writeStatePeriod"));
    const std::string integrationMethod = configMap.at("integrationMethod");

    return Config(unitSystem, outputDirPath, inputFilesDirPath, fixedTimeStep,
                  maxVelocityStep, enableAdaptiveTimeStep, maxTime, maxIterations,
                  writeStatePeriod, integrationMethod);
}

static std::map<std::string, std::string>
getConfigMap(const std::filesystem::path& configPath) {
    std::map<std::string, std::string> configMap;

    std::ifstream configFile = loadTextFile(configPath);
    std::string line;

    while (getline(configFile, line)) {
        if (line.substr(0, 2) == "//" || line == "") continue;

        std::stringstream lineStream(line);
        std::string parameterName;
        std::string parameterValue;

        lineStream >> parameterName >> parameterValue;

        configMap[parameterName] = parameterValue;
    }

    return configMap;
}
