#include <vector>
#include <cstring>
#include <stdexcept>

struct Vec {
    std::vector<double> values;
    Vec(const std::vector<double>& values) : values(values) {}
    Vec() {}
    Vec operator+(const Vec& other) {
        Vec result;
        result.values.resize(values.size());
        if (values.size() != other.values.size()) {
            throw std::invalid_argument("Vectors are of different dimensions");
        }
        for (size_t i = 0; i < values.size(); ++i) {
            result.values[i] = values[i] + other.values[i];
        }
        return result;
    }
};