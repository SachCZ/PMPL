#include "particles.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <chrono>
#include <fenv.h>

int main(int argc, char *argv[]) {
    feenableexcept(FE_INVALID | FE_OVERFLOW);

    double time = 0;
    int step = 0;
    double timeStep = 1e-8;
    double finalTime = 1e-2;
    int printCount = 350;
    auto steps = (int) std::round(finalTime / timeStep);
    int printAfterSteps = steps / printCount;
    double backgroundParticleMass = 6.63352088e-26;
    double maxCollisionFrequency = 1e7;

    const Interval side{0, 1};
    std::vector<Particle> particles = generateInRectangle(
            1000,
            side,
            side,
            9.10938356e-31 //mass of electron
    );

    //main routine
    //Sample the velocities from Maxwell distribution
    //See setThermalVelocities in particles.h
    setThermalVelocities(particles, 11600);
    initCollisionTimes(particles, maxCollisionFrequency);

    std::ofstream energyFile("data/collisional_energy.csv");
    energyFile.precision(std::numeric_limits<double>::max_digits10);

    auto start = std::chrono::high_resolution_clock::now();

    auto initialEnergy = getTotalKineticEnergy(particles);
    SideSampler sideSampler;

    while (time < finalTime) {
        updatePositions(particles, timeStep);

        //main routine
        //Apply collisions
        //see collide in particles.h
        collide(particles, backgroundParticleMass, time, maxCollisionFrequency, [](double speed){return 0.5e7;});

        sideSampler.sample(particles, side);

        //main routine
        //Apply periodic boundary conditions
        //Check if particle escaped and set it to the other side
        applyPeriodicBorderCondition(particles, side, side);

        if (step % printAfterSteps == 0) {
            updateTrajectories(particles, time);
            auto totalEnergy = getTotalKineticEnergy(particles);
            energyFile << time << "," << (totalEnergy - initialEnergy) / totalEnergy << std::endl;
        }

        time += timeStep;
        step++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto nanosecondDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    auto duration = nanosecondDuration * 1e-9;

    std::cout << "Execution took: " << duration << "s." << std::endl;

    saveTrajectories("data/collisional_trajectories.csv", particles);
}
