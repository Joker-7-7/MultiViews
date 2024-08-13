#include "Vector3D.h"
#include <cmath>


Vector3D::Vector3D(double x, double y, double z) noexcept :
    x(x),
    y(y),
    z(z)
{
}

Vector3D::Vector3D(const double* value) noexcept :
    x(value[0]),
    y(value[1]),
    z(value[2])
{
}

Vector3D& Vector3D::operator=(const double* value) noexcept
{
    x = value[0];
    y = value[1];
    z = value[2];
    return *this;
}

double* Vector3D::ptr() noexcept
{
    return &x;
}

const double* Vector3D::ptr() const noexcept
{
    return &x;
}

Vector3D Vector3D::operator+(const Vector3D& other) const noexcept
{
    return Vector3D{ x + other.x, y + other.y, z + other.z };
}

Vector3D Vector3D::operator-(const Vector3D& other) const noexcept
{
    return Vector3D{ x - other.x, y - other.y, z - other.z };
}

Vector3D Vector3D::operator/(double factor) const noexcept
{
    return (*this) * (1.0 / factor);
}

Vector3D Vector3D::operator*(double factor) const noexcept
{
    return { x * factor, y * factor, z * factor };
}

void Vector3D::round() noexcept
{
    x = std::round(x);
    y = std::round(y);
    z = std::round(z);
}

Vector3D Vector3D::normalized() const noexcept
{
    const double norm = length();
    return (norm == 0.0 || norm == 1.0) ? *this : *this / norm;
}

double Vector3D::length() const noexcept
{
    return std::sqrt(x * x + y * y + z * z);
}
