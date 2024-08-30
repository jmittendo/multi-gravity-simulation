#include "error_dict.hpp"

#include <stdexcept>
#include <format>
#include <vector>

template <typename T>
ErrorDict<T>::ErrorDict(const std::string& name)
    : ErrorDict(name, {}) {
}

template <typename T>
ErrorDict<T>::ErrorDict(const std::string& name,
                        std::initializer_list<std::pair<const std::string, T>> init)
    : name(name)
    , map(init) {
}

template <typename T> T& ErrorDict<T>::at(const std::string& key) {
    try {
        return map.at(key);
    } catch (const std::out_of_range& exception) {
        throw std::runtime_error(
            format("Key: '{}' not found in dict: '{}'", key, name));
    }
}

template <typename T> const T& ErrorDict<T>::at(const std::string& key) const {
    try {
        return map.at(key);
    } catch (const std::out_of_range& exception) {
        throw std::runtime_error(
            format("Key: '{}' not found in dict: '{}'", key, name));
    }
}

template <typename T> T& ErrorDict<T>::operator[](const std::string& key) {
    return map[key];
}

template class ErrorDict<std::string>;
template class ErrorDict<std::vector<double>>;
