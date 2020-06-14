#include "particles.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <chrono>

int main(int argc, char *argv[]) {
    double time = 0;
    int step = 0;
    double timeStep = 1e-9;
    double finalTime = 1e-5;
    int printCount = 350;
    auto steps = (int) std::round(finalTime / timeStep);
    int printAfterSteps = steps / printCount;

    const Interval side{0, 1};
    std::vector<Particle> particles = generateInRectangle(
            10000,
            side,
            side,
            9.10938356e-31 //mass of electron
    );

    //main routine
    //Sample the velocities from Maxwell distribution
    //See setThermalVelocities in particles.h
    setThermalVelocities(particles, 11600);

    std::ofstream energyFile("data/collisionless_energy.csv");
    energyFile.precision(std::numeric_limits<double>::max_digits10);

    auto start = std::chrono::high_resolution_clock::now();

    auto initialEnergy = getTotalKineticEnergy(particles);
    SideSampler sideSampler;

    while (time < finalTime) {

        //main routine
        //Propagate the particles
        updatePositions(particles, timeStep);

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

    saveTrajectories("data/collisionless_trajectories.csv", particles);
    sideSampler.save("data/collisionless_side_speeds.csv");
}
