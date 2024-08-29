#include "util.hpp"

#include <chrono>
#include <format>

std::string getDateTimeString(const bool withUnderscores, const int offsetSeconds) {
    using namespace std::chrono;

    const system_clock::time_point timePoint
        = system_clock::now() + seconds(offsetSeconds);
    const time_t time = system_clock::to_time_t(timePoint);

    char timeString[26];

    if (ctime_s(timeString, sizeof timeString, &time) != 0)
        throw std::runtime_error("Could not convert time to string.");

    std::string dateTimeString = timeString;

    // Remove \n
    if (dateTimeString.length()) dateTimeString.pop_back();

    if (withUnderscores) {
        std::replace(dateTimeString.begin(), dateTimeString.end(), ' ', '_');
        std::replace(dateTimeString.begin(), dateTimeString.end(), ':', '_');
    }

    return dateTimeString;
}

std::ifstream loadTextFile(const std::filesystem::path& filePath) {
    std::ifstream file(filePath);

    if (!file.good())
        throw std::runtime_error("Could not find or access file: '" + filePath.string()
                                 + "'.");

    return file;
}

int findVariableIndex(const std::string& variableName,
                      const std::vector<std::string>& inputFileFormat) {
    const auto iterator
        = find(inputFileFormat.begin(), inputFileFormat.end(), variableName);

    if (iterator != inputFileFormat.end())
        return distance(inputFileFormat.begin(), iterator);
    else
        return -1;
}

time_t getCurrentTimeSeconds() {
    using namespace std::chrono;

    return system_clock::to_time_t(system_clock::now());
}

std::vector<std::string> splitStringByDelimiter(const std::string& string,
                                                const char delimiter) {
    std::stringstream stream(string);
    std::vector<std::string> components;
    std::string component;

    while (std::getline(stream, component, delimiter)) {
        components.push_back(component);
    }

    if (!components.size()) components.push_back("");

    return components;
}

bool parseBoolString(const std::string& boolString) {
    if (boolString == "true")
        return true;
    else if (boolString == "false")
        return false;
    else
        throw std::invalid_argument(
            std::format("'{}' is not a valid bool string", boolString));
}
