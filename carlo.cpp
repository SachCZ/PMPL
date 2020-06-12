#include <iostream>
#include <random>
#include <cmath>
#include <algorithm>
#include <utility>
#include <chrono>

template <typename Function>
double timeIt(Function function) {
    auto start = std::chrono::high_resolution_clock::now();
    function();
    auto end = std::chrono::high_resolution_clock::now();
    auto nanosecondDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return nanosecondDuration * 1e-9;
}

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
    static const BoundaryChecker<T> isBoundary(grid);
    std::generate(first, last, [&](){
        auto gridPoint = WanderRandomly(startPoint, isBoundary);
        auto realPoint = grid.getRealPoint(gridPoint);
        return boundaryFunction(realPoint);
    });
}

int main() {

    auto duration = timeIt([](){
        const int gridSize = 41;
        const uint wandersCount = 100000;
        RectGrid<double> grid(gridSize, 1.0);
        const GridPoint startPoint{20, 20};

        std::vector<double> randomBoundaryValues(wandersCount);
        generateCarloValues(randomBoundaryValues.begin(), randomBoundaryValues.end(), startPoint, grid);

        auto potential = std::accumulate(randomBoundaryValues.begin(), randomBoundaryValues.end(), 0.0) / wandersCount;
        std::cout << potential << std::endl;
    });
    std::cout << "Execution took: " << duration << "s." << std::endl;

    return 0;
}