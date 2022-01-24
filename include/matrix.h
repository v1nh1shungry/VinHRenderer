#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

// stupid, silly and ugly
// check out `matrix.h` from folder `abandoned` if you're interested
// i think that is more beaultiful than this
// however, because of my fault, that matrix lib is too slow :(, but i think it's beautiful
// at least more beaultiful than this guy

struct Vector2f
{
    Vector2f() = default;
    Vector2f(float x, float y);
    ~Vector2f() = default;

    // const operations
    float x() const noexcept;
    float y() const noexcept;
    float u() const noexcept;
    float v() const noexcept;
    float operator[](size_t index) const noexcept;
    float norm() const noexcept;
    Vector2f normalize() const noexcept;
    Vector2f cwiseProduct(const Vector2f& other) const noexcept;

    // non-const operations
    float& x() noexcept;
    float& y() noexcept;
    float& u() noexcept;
    float& v() noexcept;
    float& operator[](size_t index) noexcept;
    void normalized() noexcept;

    float data[2]{};
};

struct Vector3f
{
    Vector3f() = default;
    Vector3f(float x, float y, float z);
    ~Vector3f() = default;

    // const operations
    float x() const noexcept;
    float y() const noexcept;
    float z() const noexcept;
    float u() const noexcept;
    float v() const noexcept;
    float w() const noexcept;
    float operator[](size_t index) const noexcept;
    float norm() const noexcept;
    Vector3f normalize() const noexcept;
    Vector3f cwiseProduct(const Vector3f& other) const noexcept;
    Vector3f cross(const Vector3f& other) const noexcept;

    // non-const operations
    float& x() noexcept;
    float& y() noexcept;
    float& z() noexcept;
    float& u() noexcept;
    float& v() noexcept;
    float& w() noexcept;
    float& operator[](size_t index) noexcept;
    void normalized() noexcept;

    float data[3]{};
};

struct Vector4f
{
    Vector4f() = default;
    Vector4f(float x, float y, float z, float w);
    ~Vector4f() = default;

    // const operations
    float x() const noexcept;
    float y() const noexcept;
    float z() const noexcept;
    float w() const noexcept;
    float operator[](size_t index) const noexcept;
    float norm() const noexcept;
    Vector4f normalize() const noexcept;
    Vector4f cwiseProduct(const Vector4f& other) const noexcept;

    // non-const operations
    float& x() noexcept;
    float & y() noexcept;
    float & z() noexcept;
    float & w() noexcept;
    float & operator[](size_t index) noexcept;
    void normaized() noexcept;

    float data[4]{};
};

struct Matrix3f
{
    Matrix3f() = default;
    ~Matrix3f() = default;

    static Matrix3f identity() noexcept;

    // const operations
    const Vector3f& operator[](size_t index) const noexcept;
    Vector3f row(size_t index) const noexcept;
    Vector3f col(size_t index) const noexcept;

    // non-const operations
    Vector3f& operator[](size_t index) noexcept;

    Vector3f data[3]{};
};

struct Matrix4f
{
    Matrix4f() = default;
    ~Matrix4f() = default;

    static Matrix4f identity() noexcept;

    // const operations
    const Vector4f& operator[](size_t index) const noexcept;
    Vector4f row(size_t index) const noexcept;
    Vector4f col(size_t index) const noexcept;

    // non-const operations
    Vector4f& operator[](size_t index) noexcept;

    Vector4f data[4]{};
};

// for Vector2f
Vector2f operator+(const Vector2f& lhs, const Vector2f& rhs) noexcept;
Vector2f operator-(const Vector2f& lhs, const Vector2f& rhs) noexcept;
float operator*(const Vector2f& lhs, const Vector2f& rhs) noexcept;
Vector2f operator*(const Vector2f& lhs, float rhs) noexcept;
Vector2f operator*(float lhs, const Vector2f& rhs) noexcept;
Vector2f operator/(const Vector2f& lhs, float rhs) noexcept;
std::ostream& operator<<(std::ostream& lhs, const Vector2f& rhs) noexcept;
Vector2f& operator<<(Vector2f& lhs, float rhs) noexcept;
Vector2f& operator,(Vector2f& lhs, float rhs) noexcept;

// for Vector3f
Vector3f operator+(const Vector3f& lhs, const Vector3f& rhs) noexcept;
Vector3f operator-(const Vector3f& lhs, const Vector3f& rhs) noexcept;
float operator*(const Vector3f& lhs, const Vector3f& rhs) noexcept;
Vector3f operator*(const Vector3f& lhs, float rhs) noexcept;
Vector3f operator*(float lhs, const Vector3f& rhs) noexcept;
Vector3f operator/(const Vector3f& lhs, float rhs) noexcept;
std::ostream& operator<<(std::ostream& lhs, const Vector3f& rhs) noexcept;
Vector3f& operator<<(Vector3f& lhs, float rhs) noexcept;
Vector3f& operator,(Vector3f& lhs, float rhs) noexcept;

// for Vector4f
Vector4f operator+(const Vector4f& lhs, const Vector4f& rhs) noexcept;
Vector4f operator-(const Vector4f& lhs, const Vector4f& rhs) noexcept;
float operator*(const Vector4f& lhs, const Vector4f& rhs) noexcept;
Vector4f operator*(const Vector4f& lhs, float rhs) noexcept;
Vector4f operator*(float lhs, const Vector4f& rhs) noexcept;
Vector4f operator/(const Vector4f& lhs, float rhs) noexcept;
std::ostream & operator<<(std::ostream& lhs, const Vector4f& rhs) noexcept;
Vector4f& operator<<(Vector4f& lhs, float rhs) noexcept;
Vector4f& operator,(Vector4f& lhs, float  rhs) noexcept;

// for Matrix3f
Matrix3f operator+(const Matrix3f& lhs, const Matrix3f& rhs) noexcept;
Matrix3f operator-(const Matrix3f& lhs, const Matrix3f& rhs) noexcept;
Matrix3f operator*(const Matrix3f& lhs, const Matrix3f& rhs) noexcept;
Matrix3f operator*(const Matrix3f& lhs, float rhs) noexcept;
Vector3f operator*(const Matrix3f& lhs, const Vector3f& rhs) noexcept;
std::ostream & operator<<(std::ostream& lhs, const Matrix3f& rhs) noexcept;
Matrix3f& operator<<(Matrix3f& lhs, float rhs) noexcept;
Matrix3f& operator,(Matrix3f& lhs, float rhs) noexcept;

// for Matrix4f
Matrix4f operator+(const Matrix4f& lhs, const Matrix4f& rhs) noexcept;
Matrix4f operator-(const Matrix4f& lhs, const Matrix4f& rhs) noexcept;
Matrix4f operator*(const Matrix4f& lhs, const Matrix4f& rhs) noexcept;
Matrix4f operator*(const Matrix4f& lhs, float rhs) noexcept;
Vector4f operator*(const Matrix4f& lhs, const Vector4f& rhs) noexcept;
std::ostream & operator<<(std::ostream& lhs, const Matrix4f& rhs) noexcept;
Matrix4f& operator<<(Matrix4f& lhs, float rhs) noexcept;
Matrix4f& operator,(Matrix4f& lhs, float rhs) noexcept;

#endif