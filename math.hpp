#pragma once

struct Math
{
 static constexpr float gravity = 1.0f;
};

struct Coords
{
    float x;
    float y;

    Coords() = default;

    Coords(float x, float y)
        : x(x), y(y) 
        {
            
        }
};

std::vector<float> operator*(const std::vector<float>& vec, float scalar) {
    std::vector<float> result = vec; // Copy the original vector
    for (float &element : result) {
        element *= scalar; // Multiply each element by the scalar
    }
    return result;
}

std::vector<float> operator*(const std::vector<float>& vec1, const std::vector<float>& vec2) {
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must have the same size for addition.");
    }

    std::vector<float> result;
    result.reserve(vec1.size());

    for (size_t i = 0; i < vec1.size(); ++i) {
        result.push_back(vec1[i] * vec2[i]); // Add corresponding elements
    }

    return result;
}

std::vector<float> operator+(const std::vector<float>& vec1, const std::vector<float>& vec2) {
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must have the same size for addition.");
    }

    std::vector<float> result;
    result.reserve(vec1.size());

    for (size_t i = 0; i < vec1.size(); ++i) {
        result.push_back(vec1[i] + vec2[i]); // Add corresponding elements
    }

    return result;
}

std::vector<float> operator-(const std::vector<float>& vec1, const std::vector<float>& vec2) {
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must have the same size for addition.");
    }

    std::vector<float> result;
    result.reserve(vec1.size());

    for (size_t i = 0; i < vec1.size(); ++i) {
        result.push_back(vec1[i] - vec2[i]); // Add corresponding elements
    }

    return result;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Coords>& vec) {
    os << "(";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i].x << ", " << vec[i].y;
        if (i != vec.size() - 1) {
            os << "; ";
        }
    }
    os << ")";
    return os;
}