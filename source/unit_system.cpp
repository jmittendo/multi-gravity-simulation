#include "unit_system.hpp"

#include "constants.hpp"

#include <map>
#include <vector>

using namespace Constants;

static const std::map<const std::string, const std::vector<double>>
    systemIdToLengthMassTimeUnitsMap
    = { { "si", { 1.0, 1.0, 1.0 } },
        { "cgs", { 0.01, 0.001, 1.0 } },
        { "solsys", { au, massSun, year } },
        { "galactic", { 1000.0 * parsec, 1E10 * massSun, 1E9 * year } },
        { "earthmoon", { 3.844E8 * meter, massEarth, day } },
        { "fff", { 201.168, 40.8233133, 1.2096E6 } },
        { "G1", { au, massSun, sqrt(au* au* au / massSun / gravityConstant) } } };

static double convertUnits(const double inputValue, const double inputUnits,
                           const double outputUnits);

UnitSystem::UnitSystem(const std::string& id)
    : id(id)
    , unitLength(systemIdToLengthMassTimeUnitsMap.at(id)[0])
    , unitMass(systemIdToLengthMassTimeUnitsMap.at(id)[1])
    , unitTime(systemIdToLengthMassTimeUnitsMap.at(id)[2])
    , unitVelocity(unitLength / unitTime)
    , gravityConstant(
          Constants::gravityConstant
          / (unitLength * unitLength * unitLength / unitMass / (unitTime * unitTime))) {
}

double UnitSystem::convertLength(const double inputLength,
                                 const UnitSystem& inputUnitSystem) const {
    return convertUnits(inputLength, inputUnitSystem.unitLength, unitLength);
}

double UnitSystem::convertMass(const double inputMass,
                               const UnitSystem& inputUnitSystem) const {
    return convertUnits(inputMass, inputUnitSystem.unitMass, unitMass);
}

double UnitSystem::convertTime(const double inputTime,
                               const UnitSystem& inputUnitSystem) const {
    return convertUnits(inputTime, inputUnitSystem.unitTime, unitTime);
}

double UnitSystem::convertVelocity(const double inputVelocity,
                                   const UnitSystem& inputUnitSystem) const {
    return convertUnits(inputVelocity, inputUnitSystem.unitVelocity, unitVelocity);
}

static double convertUnits(const double inputValue, const double inputUnits,
                           const double outputUnits) {
    return inputValue * inputUnits / outputUnits;
}
