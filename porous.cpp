#include <fstream>
#include <vector>
#include <queue>
#include <random>

struct Node {
    int i, j;

    std::vector<Node> getNeighbours() const {
        std::vector<Node> result;
        result.emplace_back(Node{i + 1, j});
        result.emplace_back(Node{i, j + 1});
        result.emplace_back(Node{i - 1, j});
        result.emplace_back(Node{i, j - 1});
        return result;
    };
};

class Que {
public:
    void add(const std::vector<Node> &nodes) {
        for (const auto &node : nodes) {
            storedNodes.emplace(node);
        }
    }

    bool isEmpty() {
        return storedNodes.empty();
    }

    Node pop() {
        Node result = storedNodes.front();
        storedNodes.pop();
        return result;
    }

private:
    std::queue<Node> storedNodes;
};

class Grid {
public:
    Grid(uint width, uint height) : width(width), height(height), blocked(width * height), visited(width * height) {}

    std::vector<Node> firstRow() const {
        std::vector<Node> result;
        result.reserve(this->width);
        for (int i = 0; i < this->width; i++) {
            result.emplace_back(Node{i, 0});
        }
        return result;
    }

    bool hasInLastRow(const Node &node) const {
        return node.j == this->height - 1;
    }

    std::vector<Node> getExplorableNeighbours(const Node &node) const {
        std::vector<Node> result;
        for (const auto &neighbour : node.getNeighbours()) {
            if (!isOutside(neighbour) &&
                !isBlocked(neighbour) &&
                !isVisited(neighbour)) {
                result.emplace_back(neighbour);
            }
        }
        return result;
    }

    void markAsVisited(const std::vector<Node> &nodes) {
        for (const auto &node : nodes)
            visited[node.i + node.j * this->width] = true;
    }

    void fillRandomly(double probability) {
        this->clear();
        for (auto &&isBlocked : this->blocked) {
            isBlocked = distribution(generator) < probability;
        }
    }

private:
    uint width, height;
    std::vector<bool> blocked;
    std::vector<bool> visited;
    std::random_device randomDevice;
    std::mt19937 generator{randomDevice()};
    std::uniform_real_distribution<> distribution{0, 1};

    bool isBlocked(const Node &node) const {
        return blocked[node.i + node.j * this->width];
    }

    bool isVisited(const Node &node) const {
        return visited[node.i + node.j * this->width];
    }

    bool isOutside(const Node &node) const {
        return node.i < 0 || node.i >= width || node.j < 0 || node.j >= height;
    }

    void clear(){
        std::fill(blocked.begin(), blocked.end(), false);
        std::fill(visited.begin(), visited.end(), false);
    }
};

//main routine
bool pathExists(Grid &grid) {
    Que que;
    que.add(grid.firstRow());
    while (!que.isEmpty()) {
        auto currentNode = que.pop(); //Take one node from the que
        if (grid.hasInLastRow(currentNode)) //if the currentNode is in the last row we reached the destination and the path exists
            return true;
        //Add valid neighbours to the que (!isOutside(neighbour) the grid && !isBlocked(neighbour) (being set randomly) &&!isVisited(neighbour) alreadyVisited by the algorithm)
        auto neighbours = grid.getExplorableNeighbours(currentNode);
        grid.markAsVisited(neighbours);
        que.add(neighbours);
    }
    return false;
}

double estimatePassProbability(Grid& grid, double blockedProbability, int sampleCount = 100){
    int existCount = 0;
    for (int i = 0; i < sampleCount; i++){
        grid.fillRandomly(blockedProbability);

        //main routine
        //see pathExists
        if(pathExists(grid)) existCount++;
    }
    return (double) existCount / sampleCount;
}

int main() {
    Grid grid(50, 50);

    uint count = 100;
    double step = 1.0 / count;
    std::ofstream file("data/porous.csv");
    for (int i = 0; i <= count; i++){
        auto blockedProbability = i * step;
        file << blockedProbability << "," << estimatePassProbability(grid, blockedProbability) << std::endl;
    }
}
