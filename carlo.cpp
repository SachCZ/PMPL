#include <iostream>
#include <random>
#include <cmath>
#include <algorithm>
#include <utility>
#include "utils.h"
#include <fstream>

struct GridPoint {
    int i = 0, j = 0;
};

template <typename T>
struct Point {
    T x = 0, y = 0;
};

enum class Direction {
    left, right, up, down
};

template<typename T>
struct RealInterval {
    T from = 0, to = 0;
    T size() const {
        return to - from;
    }
};

using DiscreteInterval = RealInterval<int>;

template<typename T>
struct RectGrid {
    RectGrid(int discreteSize, T realSize):
            RectGrid({0, discreteSize}, {0, discreteSize}, {0, realSize}, {0, realSize}) {}

    RectGrid(
            const DiscreteInterval &horizontalDiscrete,
            const DiscreteInterval &verticalDiscrete,
            const RealInterval<T> &horizontalReal,
            const RealInterval<T> &verticalReal) :
            horizontalDiscrete(horizontalDiscrete),
            verticalDiscrete(verticalDiscrete),
            horizontalReal(horizontalReal),
            verticalReal(verticalReal) {}


    Point<T> getRealPoint(const GridPoint& discretePoint) const {
        Point<T> result;
        result.x = horizontalReal.from + (T) discretePoint.i / (horizontalDiscrete.size() -1) * horizontalReal.size();
        result.y = verticalReal.from + (T) discretePoint.j / (verticalDiscrete.size() -1) * verticalReal.size();
        return result;
    }

    DiscreteInterval horizontalDiscrete, verticalDiscrete;
    RealInterval<T> horizontalReal, verticalReal;
};

struct RandomDirection{
    Direction operator()() const {
        return (Direction) distribution(generator);
    }

private:
    std::random_device randomDevice;
    mutable std::mt19937 generator{randomDevice()};
    mutable std::uniform_int_distribution<> distribution{0, 3};
};

//Randomly add i, j +/- 1 to the current point until a boundary point is found
template<typename Function>
GridPoint WanderRandomly(const GridPoint &from, Function isBoundary) {
    static const RandomDirection getRandomDirection;
    auto currentPoint = from;
    while (!isBoundary(currentPoint)) {
        switch (getRandomDirection()) {
            case Direction::left:
                currentPoint.i -= 1;
                break;
            case Direction::right:
                currentPoint.i += 1;
                break;
            case Direction::up:
                currentPoint.j += 1;
                break;
            case Direction::down:
                currentPoint.j -= 1;
                break;
            default:
                throw std::logic_error("Fatal error, no such direction.");
        }
    }
    return currentPoint;
}

template<typename T>
struct BoundaryChecker {

    explicit BoundaryChecker(RectGrid<T>  grid): grid(std::move(grid)) {}

    bool operator()(const GridPoint &point) const {
        return
                point.i <= grid.horizontalDiscrete.from ||
                point.i > grid.horizontalDiscrete.to ||
                point.j <= grid.verticalDiscrete.from ||
                point.j > grid.verticalDiscrete.to;
    }

private:
    RectGrid<T> grid;
};


double boundaryFunction(const Point<double>& point){
    auto x = point.x;
    auto y = point.y;
    return std::exp(-2*x)*std::cos(2*y);
}

template <typename ForwardIt, typename T>
void generateCarloValues(ForwardIt first, ForwardIt last, const GridPoint& startPoint, const RectGrid<T>& grid){
    const BoundaryChecker<T> isBoundary(grid);
    std::generate(first, last, [&](){
        auto gridPoint = WanderRandomly(startPoint, isBoundary);
        auto realPoint = grid.getRealPoint(gridPoint);
        return boundaryFunction(realPoint);
    });
}

struct ScalingTestResult {
    double duration;
    double potential;
};

ScalingTestResult scalingTest(int gridSize, int wandersCount = 1000){
    RectGrid<double> grid(gridSize, 1.0);
    const GridPoint startPoint{(gridSize-1)/2, (gridSize-1)/2};
    double potential;
    auto duration = timeIt([&](){


        //main routine
        std::vector<double> randomBoundaryValues(wandersCount);

        //Call function wanderRandomly number of times and save found boundary values to randomBoundaryValues
        //See wanderRandomly
        generateCarloValues(randomBoundaryValues.begin(), randomBoundaryValues.end(), startPoint, grid);

        //Calculate mean - this is equivalent to finding the probabilities a sum by boundary_value*probability
        potential = std::accumulate(randomBoundaryValues.begin(), randomBoundaryValues.end(), 0.0) / wandersCount;
    });
    return {duration, potential};
}

int main() {
    std::ofstream file("data/carlo.csv");
    for (int gridSize = 20; gridSize < 202; gridSize=gridSize+2){
        auto result = scalingTest(gridSize);
        file << gridSize << "," << result.duration << std::endl;
    }

    std::ofstream filePrecision("data/carlo_precision.csv");
    for (int i = 0; i < 100; i++){
        filePrecision << scalingTest(202).potential << std::endl;
    }

    return 0;
}