#include <iostream>
#include <vector>
#include <cmath>
#include <random>

const int MAP_SIZE = 80;  
const int PERMUTATION_SIZE = 256;
std::vector<int> permutation(PERMUTATION_SIZE * 2);

const std::vector<std::pair<int, int>> GRADIENTS = {
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}
};

void initPerm() {
    std::vector<int> p(PERMUTATION_SIZE);
    for (int i = 0; i < PERMUTATION_SIZE; i++) {
        p[i] = i;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(p.begin(), p.end(), gen);

    for (int i = 0; i < PERMUTATION_SIZE; i++) {
        permutation[i] = permutation[i + PERMUTATION_SIZE] = p[i];
    }
}

double dot(int hash, double x, double y) {
    const auto& g = GRADIENTS[hash % GRADIENTS.size()];
    return g.first * x + g.second * y;
}

double lerp(double a, double b, double t) {
    return a + t * (b - a);
}

double fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double noise(double x, double y) {
    int X = (int)floor(x) & (PERMUTATION_SIZE - 1);
    int Y = (int)floor(y) & (PERMUTATION_SIZE - 1);

    x -= floor(x);
    y -= floor(y);

    double u = fade(x);
    double v = fade(y);

    int a = permutation[X] + Y;
    int b = permutation[X + 1] + Y;
    int aa = permutation[a];
    int ab = permutation[a + 1];
    int ba = permutation[b];
    int bb = permutation[b + 1];

    double x1 = lerp(
        dot(permutation[aa], x, y),
        dot(permutation[ba], x - 1, y),
        u
    );
    double x2 = lerp(
        dot(permutation[ab], x, y - 1),
        dot(permutation[bb], x - 1, y - 1),
        u
    );

    return lerp(x1, x2, v);
}

void generateMap() {
    std::vector<std::vector<double>> map(MAP_SIZE, std::vector<double>(MAP_SIZE));

    double min_val = 1.0, max_val = -1.0;
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            double nx = x / 15.0;
            double ny = y / 15.0;

            double value = 0;
            double amplitude = 1.0;
            double frequency = 1.0;
            double persistence = 0.5;

            for (int i = 0; i < 4; i++) {
                value += noise(nx * frequency, ny * frequency) * amplitude;
                amplitude *= persistence;
                frequency *= 2;
            }

            map[y][x] = value;
            min_val = std::min(min_val, value);
            max_val = std::max(max_val, value);
        }
    }

    const std::string terrain = " .:-=+*#%@";
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            double normalized = (map[y][x] - min_val) / (max_val - min_val);

            int index = std::min(9, (int)(normalized * 10));
            std::cout << terrain[index];
        }
        std::cout << std::endl;
    }

    std::cout << "Map generated successfully" << std::endl;
}

int main() {
    initPerm();
    generateMap();
    return 0;
}