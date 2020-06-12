#include "particles.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <chrono>

struct NewtonGravitationalLaw {
    Vector operator ()(const Particle& a, const Particle& b) const {
        auto m_1 = a.mass;
        auto m_2 = b.mass;
        auto r_a = a.position;
        auto r_b = b.position;
        double G = 6.674e-20;
        auto r = (r_b - r_a);
        auto abs_r = r.getNorm();
        auto r3 = abs_r*abs_r*abs_r; //Speed hack
        return G * m_1 * m_2 * 1 / r3 * r;
    }
};


struct NewtonPotentialEnergy {
    double operator ()(const Particle& a, const Particle& b) const {
        auto m_1 = a.mass;
        auto m_2 = b.mass;
        auto r_a = a.position;
        auto r_b = b.position;
        double G = 6.674e-20;
        auto r = (r_b - r_a).getNorm();
        return - G * m_1*m_2 / r;
    }
};

int main(int argc, char* argv[]) {
    double time = 0;
    int step = 0;
    double timeStep = 60*60;
    double finalTime = 1.0 * 365 * 24 * 60 * 60;
    int printCount = 5000;
    auto steps = (int) std::round(finalTime/timeStep);
    int printAfterSteps = steps / printCount;

    NewtonGravitationalLaw forceCalculator;
    NewtonPotentialEnergy potentialCalculator;

    Particle sun(1988500e24, {0,0}, {0,0});
    Particle earth(5.9726e24, {147.09e6, 0}, {0,30.29});
    Particle moon(0.07342e24, {147.09e6 + 0.3633e6, 0}, {0,30.29 + 1.076});

    std::vector<Particle> particles = {sun, earth, moon};

    std::ofstream energyFile("data/solar_energy.csv");
    energyFile.precision(std::numeric_limits< double >::max_digits10);

    auto start = std::chrono::high_resolution_clock::now();

    auto initialEnergy = getTotalPotentialEnergy(particles, potentialCalculator) + getTotalKineticEnergy(particles);

    while (time < finalTime){
        double potentialEnergy = 0;
        if (step % printAfterSteps == 0){
            potentialEnergy = getTotalPotentialEnergy(particles, potentialCalculator);
        }

        setAllForces(particles, {0, 0});
        updateForces(particles, forceCalculator);
        updateVelocities(particles, timeStep);
        updatePositions(particles, timeStep);

        if (step % printAfterSteps == 0){
            updateTrajectories(particles, timeStep);

            auto kineticEnergy = getTotalKineticEnergy(particles);
            auto totalEnergy = potentialEnergy + kineticEnergy;

            energyFile << time << "," << (totalEnergy - initialEnergy) / totalEnergy << std::endl;
        }

        time += timeStep;
        step++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto nanosecondDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    auto duration = nanosecondDuration * 1e-9;

    std::cout << "Execution took: " << duration << "s." << std::endl;

    saveTrajectories("data/solar_trajectories.csv", particles);
}
