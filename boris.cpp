#include <chrono>
#include "particles.h"

void borisUpdateVelocity(std::vector<Particle>& particles, double timeStep, const Vector& E, const Vector& B){
    for (auto& particle : particles){
        auto q = particle.charge;
        auto dt = timeStep;
        auto m = particle.mass;
        auto b = B.getNorm();
        auto v1 = particle.velocity + q*dt/2/m*E;
        auto f1 = std::tan(q*dt/2/m * b) / b;
        auto v2 = v1 + f1 * (v1.cross(B));
        auto f2 = 2*f1 / (1 + f1*f1 * (b*b));
        auto v3 = v1 + f2 * (v2.cross(B));

        particle.previousVelocity = particle.velocity;
        particle.velocity = v3 + q*dt/2/m*E;
    }
}

int main() {
    double time = 0;
    int step = 0;
    double timeStep = 1e-3;
    double finalTime = 10;
    int printCount = 1000;
    auto steps = (int) std::round(finalTime/timeStep);
    int printAfterSteps = steps / printCount;

    Vector E = {1, 0, 0};
    Vector B = {0, 0, 1};
    Particle particle(1, {0,0,0},{1,1,0}, 1);
    std::vector<Particle> particles = {particle};

    auto start = std::chrono::high_resolution_clock::now();

    while (time < finalTime){

        borisUpdateVelocity(particles, timeStep, E, B);
        updatePositions(particles, timeStep);

        if (step % printAfterSteps == 0){
            updateTrajectories(particles, time);
        }

        time += timeStep;
        step++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto nanosecondDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    auto duration = nanosecondDuration * 1e-9;

    std::cout << "Execution took: " << duration << "s." << std::endl;

    saveTrajectories("data/boris_trajectories.csv", particles);
}
