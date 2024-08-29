#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <filesystem>

std::string getDateTimeString(const bool withUnderscores, const int offsetSeconds);
std::ifstream loadTextFile(const std::filesystem::path& filePath);
int findVariableIndex(const std::string& variableName,
                      const std::vector<std::string>& inputFileFormat);
time_t getCurrentTimeSeconds();
std::vector<std::string> splitStringByDelimiter(const std::string& string,
                                                const char delimiter);
bool parseBoolString(const std::string& boolString);
