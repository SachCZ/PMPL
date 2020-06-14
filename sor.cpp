#include <vector>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include "utils.h"

using uint = unsigned int;

struct Index2D {
    uint i, j;
};

template<typename T>
class Vector2D {
public:
    explicit Vector2D(uint size) : Vector2D(size, size) {}

    Vector2D(uint sizeX, uint sizeY) : sizeX(sizeX), sizeY(sizeY), values(sizeX * sizeY, 0) {}

    T &operator[](const Index2D &index) {
        return values[index.i + index.j * sizeX];
    }

    uint sizeX{}, sizeY{};

private:
    std::vector<T> values;

    template<typename _T>
    friend std::ostream &operator<<(std::ostream &os, Vector2D<_T> &vector);
};

template<typename T>
std::ostream &operator<<(std::ostream &os, Vector2D<T> &vector) {
    os << "[";
    std::string lineSeparator;
    for (uint j = 0; j < vector.sizeY; ++j) {
        std::stringstream line;
        std::string separator;
        for (uint i = 0; i < vector.sizeX; ++i) {
            line << separator << vector[{i, j}];
            separator = ",";
        }
        os << lineSeparator << std::endl << "[" << line.str() << "]";
        lineSeparator = ",";
    }
    os << "]";
    return os;
}

double analyticFunction(double x, double y) {
    return std::exp(-2 * x) * std::cos(2 * y);
}

bool isBorder(const Index2D &index, uint N) {
    return index.i == 0 || index.j == 0 || index.i == N - 1 || index.j == N - 1;
}

std::string linspaceString(uint size, double step) {
    std::stringstream result;
    std::string separator;

    result << "[";
    for (uint i = 0; i < size; ++i) {
        result << separator << i * step;
        separator = ",";
    }
    result << "]";
    return result.str();
}

template<typename T>
struct SorResult {
    std::string json;
    double norm{};
    uint steps{};
    Vector2D<T> function{0};
};

template<typename T>
SorResult<T> sor(double step, double omega, const Vector2D<T> &initial, const Vector2D<T> &rightHand) {
    auto phi = initial;
    auto r = rightHand;
    double maxResidual = 0;
    uint steps = 0;
    do {
        steps++;
        maxResidual = 0;
        for (uint i = 1; i < phi.sizeX - 1; i++) {
            for (uint j = 1; j < phi.sizeY - 1; j++) {
                auto currentResidual =
                        -4 * phi[{i, j}] + phi[{i - 1, j}] + phi[{i + 1, j}] + phi[{i, j - 1}] + phi[{i, j + 1}] -
                        r[{i, j}] * step * step;
                phi[{i, j}] = phi[{i, j}] + omega * 1.0 / 4 * currentResidual;
                if (std::abs(currentResidual) > maxResidual) maxResidual = std::abs(currentResidual);
            }
        }
    } while (maxResidual > 1e-5 * step * step);

    double norm = 0;
    for (uint i = 1; i < phi.sizeX - 1; i++) {
        for (uint j = 1; j < phi.sizeY - 1; j++) {
            norm += std::abs(phi[{i, j}] - analyticFunction(i * step, j * step)) * step * step;
        }
    }

    std::stringstream json;
    json << "{\"phi\":" << phi << "," << std::endl;
    json << "\"x\":" << linspaceString(phi.sizeX, step) << "," << std::endl;
    json << "\"y\":" << linspaceString(phi.sizeY, step) << "}" << std::endl;

    return {json.str(), norm, steps, phi};
}

struct ScalingTestResult {
    double duration;
    double potential;
    SorResult<double> sorResult;
};

ScalingTestResult scalingTest(uint gridSize, double omega = 1.84){
    SorResult<double> result;

    Vector2D<double> phi(gridSize);
    Vector2D<double> r(gridSize);
    double step = 1.0 / (gridSize - 1);

    for (uint i = 0; i < gridSize; i++) {
        for (uint j = 0; j < gridSize; j++) {
            if (isBorder({i, j}, gridSize)) {
                phi[{i, j}] = analyticFunction(i * step, j * step);
            }
        }
    }
    auto duration = timeIt([&]() {
        result = sor(step, omega, phi, r);
    });
    return {duration, result.function[{(gridSize - 1) / 2, (gridSize - 1) / 2}], result};
}

int main() {
    uint N = 201; //nodes
    Vector2D<double> phi(N);
    Vector2D<double> r(N);
    double step = 1.0 / (N - 1);

    for (uint i = 0; i < N; i++) {
        for (uint j = 0; j < N; j++) {
            if (isBorder({i, j}, N)) {
                phi[{i, j}] = analyticFunction(i * step, j * step);
            }
        }
    }

    uint samples = 0;
    std::ofstream optimization("data/sor_optimization.csv");
    for (int i = 1; i < samples; i++) {
        double omega = 1 + (1.0 / samples) * i;
        auto result = sor(step, omega, phi, r);
        optimization << omega << "," << result.steps << std::endl;
    }

    std::ofstream timeFile("data/sor_time.csv");
    for (int gridSize = 20; gridSize < 202; gridSize=gridSize+2){
        auto result = scalingTest(gridSize);
        timeFile << gridSize << "," << result.duration << std::endl;
    }

    auto test = scalingTest(202);
    std::cout << test.potential - analyticFunction(0.5,0.5) << std::endl;

    std::ofstream sorFile("data/sor.json");
    sorFile << test.sorResult.json;
}