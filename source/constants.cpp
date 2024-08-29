#include "constants.hpp"

#include <iostream>

namespace Constants {
    static const std::map<const std::string, const double> nameToConstantMap
        = { { "second", second },
            { "minute", minute },
            { "hour", hour },
            { "day", day },
            { "year", year },
            { "massSun", massSun },
            { "massMoon", massMoon },
            { "massEarth", massEarth },
            { "meter", meter },
            { "au", au },
            { "radiusEarth", radiusEarth },
            { "distanceMoon", distanceMoon },
            { "parsec", parsec },
            { "gravityConstant", gravityConstant },
            { "lightSpeed", lightSpeed },
            { "pi", pi },
            { "planckConstant", planckConstant },
            { "kilogram", kilogram },
            { "kilometer", kilometer } };

    double getConstantByName(const std::string& name) {
        return nameToConstantMap.at(name);
    }
}
