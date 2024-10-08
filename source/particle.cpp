#include "particle.hpp"

Particle::Particle(const double mass, const Vector2D& position,
                   const Vector2D& velocity,
                   const std::shared_ptr<const UnitSystem> unitSystem)
    : mass(mass)
    , position(position)
    , velocity(velocity)
    , unitSystem(unitSystem) {
}

double Particle::getKineticEnergy() const {
    const double absVelocity = velocity.abs();

    return 0.5 * mass * absVelocity * absVelocity;
}

double Particle::getPotentialEnergy(const Particle& particle) const {
    return -unitSystem->gravityConstant * mass * particle.mass
        / (position - particle.position).abs();
}

Vector2D Particle::getGravityAccelerationFactor(const Particle& particle) const {
    const Vector2D distance = position - particle.position;
    const double absDistance = distance.abs();

    return -unitSystem->gravityConstant / (absDistance * absDistance * absDistance)
        * distance;
}

void Particle::updatePosition(const Vector2D& velocity, const double timeStep) {
    position = position + velocity * timeStep;
}

void Particle::updateVelocity(const Vector2D& acceleration, const double timeStep) {
    velocity = velocity + acceleration * timeStep;
}
