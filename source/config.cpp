#include "config.hpp"

#include "util.hpp"
#include "error_dict.hpp"

#include <format>

static ErrorDict<std::string> getConfigDict(const std::filesystem::path& configPath);
static double parseDoubleParam(const std::string& paramName,
                               const ErrorDict<std::string>& configDict);
static unsigned long parseUnsignedLongParam(const std::string& paramName,
                                            const ErrorDict<std::string>& configDict);
static bool parseBoolParam(const std::string& paramName,
                           const ErrorDict<std::string>& configDict);

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
    const ErrorDict<std::string> configDict = getConfigDict(configPath);

    const UnitSystem unitSystem(configDict.at("unitSystem"));
    const std::filesystem::path outputDirPath = configDict.at("outputDir");
    const std::filesystem::path inputFilesDirPath = configDict.at("inputFilesDir");
    const double fixedTimeStep = parseDoubleParam("fixedTimeStep", configDict);
    const double maxVelocityStep = parseDoubleParam("maxVelocityStep", configDict);
    const bool enableAdaptiveTimeStep
        = parseBoolParam("enableAdaptiveTimeStep", configDict);
    const double maxTime = parseDoubleParam("maxTime", configDict);
    const unsigned long maxIterations
        = parseUnsignedLongParam("maxIterations", configDict);
    const double writeStatePeriod = parseDoubleParam("writeStatePeriod", configDict);
    const std::string integrationMethod = configDict.at("integrationMethod");

    return Config(unitSystem, outputDirPath, inputFilesDirPath, fixedTimeStep,
                  maxVelocityStep, enableAdaptiveTimeStep, maxTime, maxIterations,
                  writeStatePeriod, integrationMethod);
}

static ErrorDict<std::string> getConfigDict(const std::filesystem::path& configPath) {
    ErrorDict<std::string> configDict("configDict");

    std::ifstream configFile = loadTextFile(configPath);
    std::string line;

    while (getline(configFile, line)) {
        if (line.substr(0, 2) == "//" || line == "") continue;

        std::stringstream lineStream(line);
        std::string parameterName;
        std::string parameterValue;

        lineStream >> parameterName >> parameterValue;

        configDict[parameterName] = parameterValue;
    }

    return configDict;
}

static double parseDoubleParam(const std::string& paramName,
                               const ErrorDict<std::string>& configDict) {
    const std::string paramValue = configDict.at(paramName);

    try {
        return stod(paramValue);
    } catch (const std::invalid_argument& exception) {
        throw std::invalid_argument(
            format("Config parameter: '{} = {}' could not be converted to double",
                   paramName, paramValue));
    }
}

static unsigned long parseUnsignedLongParam(const std::string& paramName,
                                            const ErrorDict<std::string>& configDict) {
    const std::string paramValue = configDict.at(paramName);

    try {
        return stoul(paramValue);
    } catch (const std::invalid_argument& exception) {
        throw std::invalid_argument(format(
            "Config parameter: '{} = {}' could not be converted to unsigned long",
            paramName, paramValue));
    }
}

static bool parseBoolParam(const std::string& paramName,
                           const ErrorDict<std::string>& configDict) {
    const std::string paramValue = configDict.at(paramName);

    if (paramValue == "true")
        return true;
    else if (paramValue == "false")
        return false;
    else
        throw std::invalid_argument(
            format("Config parameter: '{} = {}' could not be converted to bool",
                   paramName, paramValue));
}
