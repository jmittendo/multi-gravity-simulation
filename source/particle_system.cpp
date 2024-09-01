#include "particle_system.hpp"

#include "util.hpp"

#include <fstream>
#include <memory>

#define INPUT_FILE_DELIMITER ' '
#define INPUT_FILE_MASS_INDEX 0
#define INPUT_FILE_POSITION_X_INDEX 1
#define INPUT_FILE_POSITION_Y_INDEX 2
#define INPUT_FILE_VELOCITY_X_INDEX 3
#define INPUT_FILE_VELOCITY_Y_INDEX 4
#define OUTPUT_FILE_SUFFIX "_output.txt"

static std::vector<Vector2D>
calculateAccelerations(const std::vector<Particle>& particles, double& timeStep,
                       const bool enableAdaptiveTimeStep, const double maxVelocityStep);
static void writeState(std::ofstream& outputFile, const double currentTime,
                       const std::vector<Particle>& particles);
static std::string getMassPosVelString(const std::vector<Particle>& particles);
static double getSystemEnergy(const std::vector<Particle>& particles);
static void integrate(std::vector<Vector2D>& particleAccelerations,
                      std::vector<Particle>& particles, double& timeStep,
                      const std::string& integrationMethod,
                      const bool enableAdaptiveTimeStep, const double maxVelocityStep);

ParticleSystem::ParticleSystem(
    const std::filesystem::path& inputFilePath,
    const std::shared_ptr<const UnitSystem> simulationUnitSystem)
    : inputFileStem(inputFilePath.stem().string()) {
    std::string line;
    std::ifstream inputFile = loadTextFile(inputFilePath);

    // The first line must contain a valid unit system id
    std::getline(inputFile, line);
    const UnitSystem fileUnitSystem(line);

    while (std::getline(inputFile, line)) {
        const std::vector<std::string> lineSegments
            = splitStringByDelimiter(line, INPUT_FILE_DELIMITER);

        const double mass = simulationUnitSystem->convertMass(
            stod(lineSegments[INPUT_FILE_MASS_INDEX]), fileUnitSystem);
        const double positionX = simulationUnitSystem->convertLength(
            stod(lineSegments[INPUT_FILE_POSITION_X_INDEX]), fileUnitSystem);
        const double positionY = simulationUnitSystem->convertLength(
            stod(lineSegments[INPUT_FILE_POSITION_Y_INDEX]), fileUnitSystem);
        const double velocityX = simulationUnitSystem->convertVelocity(
            stod(lineSegments[INPUT_FILE_VELOCITY_X_INDEX]), fileUnitSystem);
        const double velocityY = simulationUnitSystem->convertVelocity(
            stod(lineSegments[INPUT_FILE_VELOCITY_Y_INDEX]), fileUnitSystem);

        particles.push_back(Particle(mass, Vector2D(positionX, positionY),
                                     Vector2D(velocityX, velocityY),
                                     simulationUnitSystem));
    }
}

void ParticleSystem::simulate(const double fixedTimeStep,
                              const std::filesystem::path& outputDirPath,
                              const bool enableAdaptiveTimeStep,
                              const double maxVelocityStep, const double maxTime,
                              const unsigned long maxIterations,
                              const double writeStatePeriod,
                              const std::string& integrationMethod) {
    double timeStep = fixedTimeStep;
    double currentTime = 0.0;

    int writeStateCounter = 0;
    unsigned long iterationCounter = 0;

    std::ofstream outputFile
        = createOutputFile(outputDirPath / (inputFileStem + OUTPUT_FILE_SUFFIX));

    std::vector<Vector2D> particleAccelerations = calculateAccelerations(
        particles, timeStep, enableAdaptiveTimeStep, maxVelocityStep);

    while (currentTime <= maxTime) {
        if (currentTime >= static_cast<double>(writeStateCounter) * writeStatePeriod) {
            writeState(outputFile, currentTime, particles);
            writeStateCounter++;
        }

        integrate(particleAccelerations, particles, timeStep, integrationMethod,
                  enableAdaptiveTimeStep, maxVelocityStep);

        currentTime += timeStep;
        iterationCounter++;

        if (maxIterations > 0 && iterationCounter == maxIterations) {
            writeState(outputFile, -1.0, particles);

            break;
        }
    }
}

static std::vector<Vector2D>
calculateAccelerations(const std::vector<Particle>& particles, double& timeStep,
                       const bool enableAdaptiveTimeStep,
                       const double maxVelocityStep) {
    const size_t particleCount = particles.size();
    std::vector<Vector2D> particleAccelerations(particleCount);
    double maxAcceleration = 0.0;

    for (size_t i = 0; i < particleCount - 1; i++) {
        for (size_t j = i + 1; j < particleCount; j++) {
            const Vector2D factor
                = particles[i].getGravityAccelerationFactor(particles[j]);

            particleAccelerations[i]
                = particleAccelerations[i] + factor * particles[j].mass;
            particleAccelerations[j]
                = particleAccelerations[j] - factor * particles[i].mass;

            if (enableAdaptiveTimeStep) {
                if (particleAccelerations[i].abs() > maxAcceleration)
                    maxAcceleration = particleAccelerations[i].abs();
                if (particleAccelerations[j].abs() > maxAcceleration)
                    maxAcceleration = particleAccelerations[j].abs();
            }
        }
    }

    if (enableAdaptiveTimeStep) timeStep = maxVelocityStep / maxAcceleration;

    return particleAccelerations;
}

static void writeState(std::ofstream& outputFile, const double currentTime,
                       const std::vector<Particle>& particles) {
    outputFile << currentTime << ", " << getMassPosVelString(particles) << ", "
               << getSystemEnergy(particles) << std::endl;
}

static std::string getMassPosVelString(const std::vector<Particle>& particles) {
    const size_t particleCount = particles.size();
    std::stringstream massPosVelStream;

    for (const Particle& particle : particles) {
        massPosVelStream << particle.mass << ", ";
    }

    for (const Particle& particle : particles) {
        massPosVelStream << particle.position << ", ";
    }

    for (size_t i = 0; i < particleCount - 1; i++) {
        massPosVelStream << particles[i].velocity << ", ";
    }

    massPosVelStream << particles[particleCount - 1].velocity;

    return massPosVelStream.str();
}

static double getSystemEnergy(const std::vector<Particle>& particles) {
    const size_t particleCount = particles.size();
    double potentialEnergy = 0.0;

    for (size_t i = 0; i < particleCount - 1; i++) {
        for (size_t j = i + 1; j < particleCount; j++) {
            potentialEnergy += particles[i].getPotentialEnergy(particles[j]);
        }
    }

    double kineticEnergy = 0.0;

    for (const Particle& particle : particles) {
        kineticEnergy += particle.getKineticEnergy();
    }

    return potentialEnergy + kineticEnergy;
}

static void integrate(std::vector<Vector2D>& particleAccelerations,
                      std::vector<Particle>& particles, double& timeStep,
                      const std::string& integrationMethod,
                      const bool enableAdaptiveTimeStep, const double maxVelocityStep) {
    if (integrationMethod == "kdk") {
        for (size_t i = 0; i < particles.size(); i++) {
            particles[i].updateVelocity(particleAccelerations[i], 0.5 * timeStep);
            particles[i].updatePosition(particles[i].velocity, timeStep);
        }

        particleAccelerations = calculateAccelerations(
            particles, timeStep, enableAdaptiveTimeStep, maxVelocityStep);

        for (size_t i = 0; i < particles.size(); i++) {
            particles[i].updateVelocity(particleAccelerations[i], 0.5 * timeStep);
        }
    } else if (integrationMethod == "dkd") {
        for (Particle& particle : particles) {
            particle.updatePosition(particle.velocity, 0.5 * timeStep);
        }

        particleAccelerations = calculateAccelerations(
            particles, timeStep, enableAdaptiveTimeStep, maxVelocityStep);

        for (size_t i = 0; i < particles.size(); i++) {
            particles[i].updateVelocity(particleAccelerations[i], timeStep);
            particles[i].updatePosition(particles[i].velocity, 0.5 * timeStep);
        }
    } else if (integrationMethod == "euler") {
        particleAccelerations = calculateAccelerations(
            particles, timeStep, enableAdaptiveTimeStep, maxVelocityStep);

        for (size_t i = 0; i < particles.size(); i++) {
            particles[i].updatePosition(particles[i].velocity, timeStep);
            particles[i].updateVelocity(particleAccelerations[i], timeStep);
        }
    } else if (integrationMethod == "rk4") {
        std::vector<Particle> k1Particles = particles;
        const std::vector<Vector2D> k1Accelerations
            = calculateAccelerations(k1Particles, timeStep, false, maxVelocityStep);

        std::vector<Particle> k2Particles = particles;
        for (size_t i = 0; i < k2Particles.size(); i++) {
            k2Particles[i].updatePosition(k1Particles[i].velocity, 0.5 * timeStep);
            k2Particles[i].updateVelocity(k1Accelerations[i], 0.5 * timeStep);
        }
        const std::vector<Vector2D> k2Accelerations
            = calculateAccelerations(k2Particles, timeStep, false, maxVelocityStep);

        std::vector<Particle> k3Particles = particles;
        for (size_t i = 0; i < k3Particles.size(); i++) {
            k3Particles[i].updatePosition(k2Particles[i].velocity, 0.5 * timeStep);
            k3Particles[i].updateVelocity(k2Accelerations[i], 0.5 * timeStep);
        }
        const std::vector<Vector2D> k3Accelerations
            = calculateAccelerations(k3Particles, timeStep, false, maxVelocityStep);

        std::vector<Particle> k4Particles = particles;
        for (size_t i = 0; i < k4Particles.size(); i++) {
            k4Particles[i].updatePosition(k3Particles[i].velocity, timeStep);
            k4Particles[i].updateVelocity(k3Accelerations[i], timeStep);
        }
        const std::vector<Vector2D> k4Accelerations
            = calculateAccelerations(k4Particles, timeStep, false, maxVelocityStep);

        for (size_t i = 0; i < particles.size(); i++) {
            particles[i].updatePosition(
                (k1Particles[i].velocity + 2.0 * k2Particles[i].velocity
                 + 2.0 * k3Particles[i].velocity + k4Particles[i].velocity)
                    / 6.0,
                timeStep);

            particles[i].updateVelocity((k1Accelerations[i] + 2.0 * k2Accelerations[i]
                                         + 2.0 * k3Accelerations[i]
                                         + k4Accelerations[i])
                                            / 6.0,
                                        timeStep);
        }

        if (enableAdaptiveTimeStep)
            calculateAccelerations(particles, timeStep, true, maxVelocityStep);
    } else {
        throw std::runtime_error("Unknown integration method: " + integrationMethod);
    }
}
