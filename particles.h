#ifndef PMPL_PARTICLES_H
#define PMPL_PARTICLES_H

#include <string>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <vector>
#include <tuple>
#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>
#include <functional>

class Random {
public:
    double get(){
        return dist(engine);
    }
private:
    std::random_device dev;
    std::default_random_engine engine{dev()};
    std::uniform_real_distribution<double> dist{0, 1};
};

struct PhasePoint {
    double x, y, z;
    double vx, vy, vz;
    double t;
};

struct Vector {
    double x;
    double y;
    double z;

    Vector cross(const Vector& b){
        auto a = *this;
        return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
    }

    double getNorm() const {
        return std::sqrt(x * x + y * y + z * z);
    }
};

Vector operator+(Vector a, Vector b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector operator-(const Vector &a, const Vector &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector operator*(double coeff, Vector a) {
    return {coeff * a.x, coeff * a.y, coeff * a.z};
}

Vector &operator+=(Vector &a, const Vector &b) {
    a = a + b;
    return a;
}

Vector &operator-=(Vector &a, const Vector &b) {
    a = a - b;
    return a;
}

class Particle {
public:
    Vector getAcceleration() const {
        return 1.0 / this->mass * this->force;
    }

    void updateTrajectory(double currentTime) {
        auto previousTime = this->trajectory.back().t;
        PhasePoint newPoint{};
        newPoint.x = this->position.x;
        newPoint.y = this->position.y;
        newPoint.z = this->position.z;
        newPoint.vx = this->velocity.x;
        newPoint.vy = this->velocity.y;
        newPoint.vz = this->velocity.z;
        newPoint.t = currentTime;
        this->trajectory.emplace_back(newPoint);
    }

    Particle(double mass, Vector position, Vector velocity, double charge = 0) :
            mass(mass),
            position(position),
            velocity(velocity),
            previousVelocity(velocity),
            relativisticVelocity(velocity),
            charge(charge) {
        PhasePoint newPoint{};
        newPoint.x = this->position.x;
        newPoint.y = this->position.y;
        newPoint.z = this->position.z;
        newPoint.vx = this->velocity.x;
        newPoint.vy = this->velocity.y;
        newPoint.vz = this->velocity.z;
        newPoint.t = 0;
        trajectory.emplace_back(newPoint);
    }

    Vector force{0, 0};
    Vector position;
    Vector velocity;
    Vector previousVelocity;
    Vector relativisticVelocity;
    double mass, charge;
    std::vector<PhasePoint> trajectory;
    double nextCollisionTime{0};
};

void saveTrajectories(const std::string &filename, const std::vector<Particle> &particles) {
    std::ofstream file(filename);

    size_t size = particles[0].trajectory.size();
    for (const auto &particle : particles) {
        if (particle.trajectory.size() != size) {
            throw std::logic_error("Invalid trajectory size. Cannot be saved to file!");
        }
    }
    for (int i = 0; i < size; i++) {
        std::stringstream line;
        line << particles[0].trajectory[i].t;
        for (const auto &particle : particles) {
            line << "," << particle.trajectory[i].x << ","
                 << particle.trajectory[i].y << ","
                 << particle.trajectory[i].z << ","
                 << particle.trajectory[i].vx << ","
                 << particle.trajectory[i].vy << ","
                 << particle.trajectory[i].vz;
        }
        line << std::endl;
        file << line.str();
    }
}


void setAllForces(std::vector<Particle> &particles, const Vector &force) {
    for (auto &particle : particles) {
        particle.force = force;
    }
}

template<typename ForceCalculator>
void updateForces(std::vector<Particle> &particles, const ForceCalculator &forceCalculator) {
    for (uint i = 0; i < particles.size(); ++i) {
        for (uint j = i + 1; j < particles.size(); ++j) {
            auto force = forceCalculator(particles[i], particles[j]);
            particles[i].force += force;
            particles[j].force -= force;
        }
    }
}

template<typename PotentialEnergyCalculator>
double getTotalPotentialEnergy(
        const std::vector<Particle> &particles,
        const PotentialEnergyCalculator &potentialEnergyCalculator
) {
    double result = 0;

    for (uint i = 0; i < particles.size(); ++i) {
        for (uint j = i + 1; j < particles.size(); ++j) {
            result += potentialEnergyCalculator(particles[i], particles[j]);
        }
    }
    return result;
}

double getTotalKineticEnergy(const std::vector<Particle> &particles) {
    double result = 0;
    for (const auto &particle: particles) {
        auto averageVelocity = 0.5 * (particle.velocity + particle.previousVelocity).getNorm();
        result += 0.5 * particle.mass * std::pow(averageVelocity, 2);
    }
    return result;
}

void updateVelocities(std::vector<Particle> &particles, double timeStep) {
    for (auto &particle : particles) {
        particle.previousVelocity = particle.velocity;
        particle.velocity += timeStep * particle.getAcceleration();
    }
}


void updatePositions(std::vector<Particle> &particles, double timeStep) {
    for (auto &particle : particles) {
        particle.position += timeStep * particle.velocity;
    }
}

void updateTrajectories(std::vector<Particle> &particles, double currentTime) {
    for (auto &particle : particles) {
        particle.updateTrajectory(currentTime);
    }
}

struct Interval {
    double begin;
    double end;
};

void applyPeriodicBorderCondition(std::vector<Particle> &particles, Interval sideX, Interval sideY) {
    for (auto &particle : particles) {
        if (particle.position.x > sideX.end) {
            particle.position.x = sideX.begin;
        }
        if (particle.position.x < sideX.begin) {
            particle.position.x = sideX.end;
        }
        if (particle.position.y > sideY.end) {
            particle.position.y = sideY.begin;
        }
        if (particle.position.y < sideY.begin) {
            particle.position.y = sideY.end;
        }
    }
}

std::vector<Particle> generateInRectangle(size_t count, Interval sideX, Interval sideY, double mass) {
    std::random_device dev;
    std::default_random_engine generator(dev());
    std::uniform_real_distribution<double> xDistribution(sideX.begin, sideX.end);
    std::uniform_real_distribution<double> yDistribution(sideY.begin, sideY.end);

    std::vector<Particle> result;
    for (int i = 0; i < count; i++) {
        Vector position{xDistribution(generator), yDistribution(generator), 0};
        Vector velocity{0, 0, 0};
        result.emplace_back(Particle(mass, position, velocity));
    }
    return result;
}

//In 2D choose a new random direction for the particle while preserving energy
void collide(Particle &particle, double backgroundParticleMass) {
    auto m1 = particle.mass;
    auto m2 = backgroundParticleMass;
    Random R01;
    auto angle = 2 * M_PI * R01.get();

    const double relativeEnergyChange = 2 * m1 / m2 * (1 - std::cos(angle));
    const double xyNorm = std::sqrt(particle.velocity.x*particle.velocity.x + particle.velocity.y*particle.velocity.y);
    const double newVNorm = (1 - std::sqrt(relativeEnergyChange)) * xyNorm;
    particle.velocity.x = std::cos(angle) * newVNorm;
    particle.velocity.y = std::sin(angle) * newVNorm;
}

void setNextCollisionTime(Particle& particle, double maxFrequency){
    Random R01;
    particle.nextCollisionTime += - 1 / maxFrequency * std::log(1 - R01.get());
}

//Each particle tracks its internal time of next collision. Collide the particles if the time comes and again set this time randomly.
//see collide(particle)
template<typename Frequency>
void collide(
        std::vector<Particle> &particles,
        double backgroundParticleMass,
        double currentTime,
        double maxFrequency,
        Frequency getFrequency
) {
    Random R01;
    for (auto &particle : particles) {
        if (currentTime > particle.nextCollisionTime) {
            auto probability = getFrequency(particle.velocity.getNorm()) / maxFrequency;
            if (R01.get() > probability) {
                collide(particle, backgroundParticleMass);
            }
            setNextCollisionTime(particle, maxFrequency);
        }
    }
}

void initCollisionTimes(std::vector<Particle>& particles, double maxFrequency){
    for (auto& particle : particles){
        setNextCollisionTime(particle, maxFrequency);
    }
}

//Simply set each coordinate of velocity to be from normal distribution
void setThermalVelocities(std::vector<Particle> &particles, double temperature) {
    std::random_device dev;
    std::default_random_engine generator(dev());

    std::for_each(particles.begin(), particles.end(), [&generator, &temperature](Particle &particle) {
        double mean = 0;
        const double k = 1.38064852e-23;
        auto T = temperature;
        auto m = particle.mass;
        double stddev = std::sqrt(k * T / m);
        std::normal_distribution<double> distribution(mean, stddev);
        particle.velocity = {distribution(generator), distribution(generator), distribution(generator)};
    });
}

class SideSampler {
public:
    void sample(std::vector<Particle> &particles, const Interval &sideX) {
        for (const auto &particle : particles) {
            if (particle.position.x > sideX.end || particle.position.x < sideX.begin) {
                this->speeds.emplace_back(particle.velocity.getNorm());
            }
        }
    }

    void save(const std::string &filename) const {
        std::ofstream file(filename);
        for (const auto &speed : this->speeds) {
            file << speed << std::endl;
        }
    }

private:
    std::vector<double> speeds;
};
#endif //PMPL_PARTICLES_H
