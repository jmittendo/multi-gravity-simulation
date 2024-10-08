#include "Vector2D.hpp"

#include <string>
#include <math.h>

Vector2D::Vector2D(const double x, const double y)
    : x(x)
    , y(y) {
}

Vector2D::Vector2D(const double n)
    : Vector2D(n, n) {
}

Vector2D::Vector2D()
    : Vector2D(0.0) {
}

Vector2D Vector2D::add(const Vector2D& v) const {
    return Vector2D(x + v.x, y + v.y);
}

Vector2D Vector2D::scalarProduct(const double n) const {
    return Vector2D(x * n, y * n);
}

double Vector2D::abs() const {
    return sqrt(dotProduct(*this));
}

double Vector2D::dotProduct(const Vector2D& v) const {
    return x * v.x + y * v.y;
}

Vector2D Vector2D::operator+(const Vector2D& v) const {
    return add(v);
}

Vector2D Vector2D::operator-(const Vector2D& v) const {
    return add(v * (-1.0));
}

double Vector2D::operator*(const Vector2D& v) const {
    return dotProduct(v);
}

Vector2D Vector2D::operator*(const double n) const {
    return scalarProduct(n);
}

Vector2D Vector2D::operator/(const double n) const {
    return scalarProduct(1.0 / n);
}

double Vector2D::operator[](const int i) const {
    if (i == 0)
        return x;
    else if (i == 1)
        return y;
    else
        throw std::invalid_argument("Index " + std::to_string(i) + " out of range");
}

Vector2D operator*(const double n, const Vector2D& v) {
    return v.scalarProduct(n);
}

std::ostream& operator<<(std::ostream& stream, const Vector2D& v) {
    stream << v.x << ", " << v.y;

    return stream;
}
