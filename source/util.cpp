#include "util.hpp"

#include <chrono>
#include <format>
#include <sstream>
#include <iostream>

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

    if (!file)
        throw std::ifstream::failure(
            format("Could not find or read file: '{}'", filePath.string()));

    return file;
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

std::vector<std::filesystem::directory_entry>
getFileEntries(const std::filesystem::path& dirPath) {
    const std::filesystem::directory_iterator dirIterator(dirPath);

    std::vector<std::filesystem::directory_entry> fileEntries;

    for (const auto& dirEntry : dirIterator) {
        if (dirEntry.is_regular_file()) fileEntries.push_back(dirEntry);
    }

    return fileEntries;
}

void printProgress(const size_t progress, const size_t total) {
    std::cout << "Progress: " << progress << "/" << total << " ("
              << static_cast<double>(progress) / static_cast<double>(total) * 100.0
              << " %)" << std::endl;
}
