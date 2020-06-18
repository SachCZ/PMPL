#include <chrono>
#include "particles.h"

double getGamma(double velocity) {
    auto c = 299792458;
    auto u = velocity;
    auto result = std::sqrt(1 + (u / c) * (u / c));
    return result;
}

void
borisRelativisticUpdateVelocity(std::vector<Particle> &particles, double timeStep, const Vector &E, const Vector &B) {
    for (auto &particle : particles) {
        auto q = particle.charge;
        auto dt = timeStep;
        auto m = particle.mass;
        auto b = B.getNorm();
        auto gamma = getGamma(particle.relativisticVelocity.getNorm());
        auto v1 = particle.relativisticVelocity + q * dt / 2 / m * E;
        auto f1 = std::tan(q * dt / 2 / m / gamma * b) / b;
        auto v2 = v1 + f1 * (v1.cross(B));
        auto f2 = 2 * f1 / (1 + f1 * f1 * (b * b));
        auto v3 = v1 + f2 * (v2.cross(B));

        particle.previousVelocity = particle.velocity;
        particle.relativisticVelocity = v3 + q * dt / 2 / m * E;
        gamma = getGamma(particle.relativisticVelocity.getNorm());
        particle.velocity = 1 / gamma * particle.relativisticVelocity;
    }
}

int main() {
    double time = 0;
    int step = 0;
    double timeStep = 1e-13;
    double finalTime = 1e-10;
    int printCount = 1000;
    auto steps = (int) std::round(finalTime / timeStep);
    int printAfterSteps = steps / printCount;

    Vector E = {1e8, 0, 0};
    Vector B = {0, 0, 1};
    Particle particle(9.10938356e-31, {0, 0, 0}, {0, 1e8, 0}, 1.60217662e-19);
    std::vector<Particle> particles = {particle};

    auto start = std::chrono::high_resolution_clock::now();

    while (time < finalTime) {

        //main routine
        //These are the only important line, everything else is setup
        //Update the particle velocities based on the relativistic Boris algorithm
        //Note that relativisticVelocity = gamma * particle.velocity
        borisRelativisticUpdateVelocity(particles, timeStep, E, B);
        updatePositions(particles, timeStep);

        if (step % printAfterSteps == 0) {
            updateTrajectories(particles, time);
        }

        time += timeStep;
        step++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto nanosecondDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    auto duration = nanosecondDuration * 1e-9;

    std::cout << "Execution took: " << duration << "s." << std::endl;

    saveTrajectories("data/boris_relativistic_trajectories.csv", particles);
}
