#include "matrix.h"

#include <cmath>

using namespace std;

// Vector2f here

Vector2f::Vector2f(float x, float y) {
    data[0] = x; data[1] = y;
}

float Vector2f::x() const noexcept { return data[0]; }

float Vector2f::y() const noexcept { return data[1]; }

float Vector2f::u() const noexcept { return data[0]; }

float Vector2f::v() const noexcept { return data[1]; }

float Vector2f::operator[](size_t index) const noexcept { return data[index]; }

float Vector2f::norm() const noexcept { return sqrtf(data[0] * data[0] + data[1] * data[1]); }

Vector2f Vector2f::normalize() const noexcept {
    Vector2f res = *this;
    float n = norm();
    res.data[0] /= n; res.data[1] /= n;
    return res;
}

Vector2f Vector2f::cwiseProduct(const Vector2f &other) const noexcept {
    return { data[0] * other.data[0], data[1] * other.data[1] };
}

float & Vector2f::x() noexcept { return data[0]; }

float& Vector2f::y() noexcept { return data[1]; }

float & Vector2f::u() noexcept { return data[0]; }

float & Vector2f::v() noexcept { return data[1]; }

float & Vector2f::operator[](size_t index) noexcept { return data[index]; }

void Vector2f::normalized() noexcept {
    float n = norm();
    data[0] /= n; data[1] /= n;
}

// Vector3f here

Vector3f::Vector3f(float x, float y, float z) {
    data[0] = x; data[1] = y; data[2] = z;
}

float Vector3f::x() const noexcept { return data[0]; }

float Vector3f::y() const noexcept { return data[1]; }

float Vector3f::z() const noexcept { return data[2]; }

float Vector3f::u() const noexcept { return data[0]; }

float Vector3f::v() const noexcept { return data[1]; }

float Vector3f::w() const noexcept { return data[2]; }

float Vector3f::operator[](size_t index) const noexcept { return data[index]; }

float Vector3f::norm() const noexcept {
    return sqrtf(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
}

Vector3f Vector3f::normalize() const noexcept {
    float n = norm();
    Vector3f res = *this;
    for (float & i : res.data) i /= n;
    return res;
}

Vector3f Vector3f::cwiseProduct(const Vector3f &other) const noexcept {
    return { data[0] * other.data[0], data[1] * other.data[1], data[2] * other.data[2] };
}

Vector3f Vector3f::cross(const Vector3f &other) const noexcept {
    return { data[1] * other.data[2] - data[2] * other.data[1],
             data[2] * other.data[0] - data[0] * other.data[2],
             data[0] * other.data[1] - data[1] * other.data[0] };
}

float & Vector3f::x() noexcept { return data[0]; }

float & Vector3f::y() noexcept { return data[1]; }

float & Vector3f::z() noexcept { return data[2]; }

float & Vector3f::u() noexcept { return data[0]; }

float & Vector3f::v() noexcept { return data[1]; }

float & Vector3f::w() noexcept { return data[2]; }

float & Vector3f::operator[](size_t index) noexcept { return data[index]; }

void Vector3f::normalized() noexcept {
    float n = norm();
    for (float & i : data) i /= n;
}

// Vector4f here

Vector4f::Vector4f(float x, float y, float z, float w) {
    data[0] = x; data[1] = y; data[2] = z; data[3] = w;
}

float Vector4f::x() const noexcept { return data[0]; }

float Vector4f::y() const noexcept { return data[1]; }

float Vector4f::z() const noexcept { return data[2]; }

float Vector4f::w() const noexcept { return data[3]; }

float Vector4f::operator[](size_t index) const noexcept { return data[index]; }

float Vector4f::norm() const noexcept {
    return sqrtf(data[0] * data[0] + data[1] * data[1] + data[2] * data[2] + data[3] * data[3]);
}

Vector4f Vector4f::normalize() const noexcept {
    Vector4f res = *this;
    float n = norm();
    for (float & i : res.data) i /= n;
    return res;
}

Vector4f Vector4f::cwiseProduct(const Vector4f &other) const noexcept {
    return { data[0] * other.data[0], data[1] * other.data[1],
             data[2] * other.data[2], data[3] * other.data[2] };
}

float & Vector4f::x() noexcept { return data[0]; }

float & Vector4f::y() noexcept { return data[1]; }

float & Vector4f::z() noexcept { return data[2]; }

float & Vector4f::w() noexcept { return data[3]; }

float & Vector4f::operator[](size_t index) noexcept { return data[index]; }

void Vector4f::normaized() noexcept {
    float n = norm();
    for (float & i : data) i /= n;
}

// Matrix3f here

Matrix3f Matrix3f::identity() noexcept {
    Matrix3f res;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            res.data[i].data[j] = (i == j);
        }
    }
    return res;
}

const Vector3f & Matrix3f::operator[](size_t index) const noexcept { return data[index]; }

Vector3f Matrix3f::row(size_t index) const noexcept { return data[index]; }

Vector3f Matrix3f::col(size_t index) const noexcept {
    Vector3f res;
    for (int i = 0; i < 3; ++i) res.data[i] = data[i][index];
    return res;
}

Vector3f & Matrix3f::operator[](size_t index) noexcept { return data[index]; }

// Matrix4f here

Matrix4f Matrix4f::identity() noexcept {
    Matrix4f res;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            res.data[i].data[j] = (i == j);
        }
    }
    return res;
}

const Vector4f & Matrix4f::operator[](size_t index) const noexcept { return data[index]; }

Vector4f Matrix4f::row(size_t index) const noexcept { return data[index]; }

Vector4f Matrix4f::col(size_t index) const noexcept {
    Vector4f res;
    for (int i = 0; i < 4; ++i) res.data[i] = data[i][index];
    return res;
}

Vector4f & Matrix4f::operator[](size_t index) noexcept { return data[index]; }

// for Vector2f
Vector2f operator+(const Vector2f& lhs, const Vector2f& rhs) noexcept
{
    return { lhs.data[0] + rhs.data[0], lhs.data[1] + rhs.data[1] };
}

Vector2f operator-(const Vector2f& lhs, const Vector2f& rhs) noexcept
{
    return { lhs.data[0] - rhs.data[0], lhs.data[1] - rhs.data[1] };
}

float operator*(const Vector2f& lhs, const Vector2f& rhs) noexcept
{
    return lhs.data[0] * rhs.data[0] + lhs.data[1] * rhs.data[1];
}

Vector2f operator*(const Vector2f& lhs, float rhs) noexcept
{
    return { lhs.data[0] * rhs, lhs.data[1] * rhs };
}

Vector2f operator*(float lhs, const Vector2f& rhs) noexcept
{
    return { lhs * rhs.data[0], lhs * rhs.data[1] };
}

Vector2f operator/(const Vector2f& lhs, float rhs) noexcept
{
    return { lhs.data[0] / rhs, lhs.data[1] / rhs };
}

ostream& operator<<(ostream& lhs, const Vector2f& rhs) noexcept
{
    lhs << rhs.data[0] << endl;
    lhs << rhs.data[1] << endl;
    return lhs;
}

Vector2f& operator<<(Vector2f& lhs, float rhs) noexcept
{
    static size_t index = 0;
    if (index == 2) index = 0;
    lhs.data[index++] = rhs;
    return lhs;
}

Vector2f& operator,(Vector2f& lhs, float rhs) noexcept
{
    return lhs << rhs;
};

// for Vector3f
Vector3f operator+(const Vector3f& lhs, const Vector3f& rhs) noexcept
{
    return { lhs.data[0] + rhs.data[0],
             lhs.data[1] + rhs.data[1],
             lhs.data[2] + rhs.data[2] };
}

Vector3f operator-(const Vector3f& lhs, const Vector3f& rhs) noexcept
{
    return { lhs.data[0] - rhs.data[0],
             lhs.data[1] - rhs.data[1],
             lhs.data[2] - rhs.data[2] };
}

float operator*(const Vector3f& lhs, const Vector3f& rhs) noexcept
{
    return lhs.data[0] * rhs.data[0] + lhs.data[1] * rhs.data[1] + lhs.data[2] * rhs.data[2];
}

Vector3f operator*(const Vector3f& lhs, float rhs) noexcept
{
    return { lhs.data[0] * rhs, lhs.data[1] * rhs, lhs.data[2] * rhs };
}

Vector3f operator*(float lhs, const Vector3f& rhs) noexcept
{
    return { lhs * rhs.data[0], lhs * rhs.data[1], lhs * rhs.data[2] };
}

Vector3f operator/(const Vector3f& lhs, float rhs) noexcept
{
    return { lhs.data[0] / rhs, lhs.data[1] / rhs, lhs.data[2] / rhs };
}

ostream & operator<<(ostream& lhs, const Vector3f& rhs) noexcept
{
    for (float i : rhs.data) lhs << i << endl;
    return lhs;
}

Vector3f& operator<<(Vector3f& lhs, float rhs) noexcept
{
    static int index = 0;
    if (index == 3) index = 0;
    lhs.data[index++] = rhs;
    return lhs;
}

Vector3f& operator,(Vector3f& lhs, float rhs) noexcept
{
    return lhs << rhs;
};

// for Vector4f
Vector4f operator+(const Vector4f& lhs, const Vector4f& rhs) noexcept
{
    return { lhs.data[0] + rhs.data[0],
             lhs.data[1] + rhs.data[1],
             lhs.data[2] + rhs.data[2],
             lhs.data[3] + rhs.data[3] };
}

Vector4f operator-(const Vector4f& lhs, const Vector4f& rhs) noexcept
{
    return { lhs.data[0] - rhs.data[0],
             lhs.data[1] - rhs.data[1],
             lhs.data[2] - rhs.data[2],
             lhs.data[3] - rhs.data[3] };
}

float operator*(const Vector4f& lhs, const Vector4f& rhs) noexcept
{
    return lhs.data[0] * rhs.data[0] + lhs.data[1] * rhs.data[1]
        + lhs.data[2] * rhs.data[2] + lhs.data[3] * rhs.data[3];
}

Vector4f operator*(const Vector4f& lhs, float rhs) noexcept
{
    return { lhs.data[0] * rhs,
             lhs.data[1] * rhs,
             lhs.data[2] * rhs,
             lhs.data[3] * rhs };
}

Vector4f operator*(float lhs, const Vector4f& rhs) noexcept
{
    return { lhs * rhs.data[0], lhs * rhs.data[1],
             lhs * rhs.data[2], lhs * rhs.data[2] };
}

Vector4f operator/(const Vector4f& lhs, float rhs) noexcept
{
    return { lhs.data[0] / rhs,
             lhs.data[1] / rhs,
             lhs.data[2] / rhs,
             lhs.data[3] / rhs };
}

ostream & operator<<(ostream& lhs, const Vector4f& rhs) noexcept
{
    for (float i : rhs.data) lhs << i << endl;
    return lhs;
}

Vector4f& operator<<(Vector4f& lhs, float rhs) noexcept
{
    static int index = 0;
    if (index == 4) index = 0;
    lhs.data[index++] = rhs;
    return lhs;
}

Vector4f& operator,(Vector4f& lhs, float  rhs) noexcept
{
    return lhs << rhs;
};

// for Matrix3f
Matrix3f operator+(const Matrix3f& lhs, const Matrix3f& rhs) noexcept
{
    Matrix3f res = lhs;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            res[i][j] += rhs[i][j];
        }
    }
    return res;
}

Matrix3f operator-(const Matrix3f& lhs, const Matrix3f& rhs) noexcept
{
    Matrix3f res = lhs;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            res[i][j] -= rhs[i][j];
        }
    }
    return res;
}

Matrix3f operator*(const Matrix3f& lhs, const Matrix3f& rhs) noexcept
{
    Matrix3f res;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                res[i][j] += lhs[i][k] * rhs[k][j];
            }
        }
    }
    return res;
}

Matrix3f operator*(const Matrix3f& lhs, float rhs) noexcept
{
    Matrix3f res = lhs;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            res[i][j] *= rhs;
        }
    }
    return res;
}

Vector3f operator*(const Matrix3f& lhs, const Vector3f& rhs) noexcept
{
    float product[3];
    for (int i = 0; i < 3; ++i)
    {
        product[i] = lhs[i][0] * rhs[0] + lhs[i][1] * rhs[1] + lhs[i][2] * rhs[2];
    }
    return { product[0], product[1], product[2] };
}

ostream & operator<<(ostream& lhs, const Matrix3f& rhs) noexcept
{
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            lhs << rhs[i][j] << endl;
        }
    }
    return lhs;
}

Matrix3f& operator<<(Matrix3f& lhs, float rhs) noexcept
{
    static int row = 0;
    static int col = 0;
    if (col == 3)
    {
        col = 0;
        if (++row == 3) row = 0;
    }
    lhs.data[row][col++] = rhs;
    return lhs;
}
Matrix3f& operator,(Matrix3f& lhs, float rhs) noexcept
{
    return lhs << rhs;
};

// for Matrix4f
Matrix4f operator+(const Matrix4f& lhs, const Matrix4f& rhs) noexcept
{
    Matrix4f res = lhs;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            res[i][j] += rhs[i][j];
        }
    }
    return res;
}

Matrix4f operator-(const Matrix4f& lhs, const Matrix4f& rhs) noexcept
{
    Matrix4f res = lhs;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            res[i][j] -= rhs[i][j];
        }
    }
    return res;
}

Matrix4f operator*(const Matrix4f& lhs, const Matrix4f& rhs) noexcept
{
    Matrix4f res;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                res[i][j] += lhs[i][k] * rhs[k][j];
            }
        }
    }
    return res;
}

Matrix4f operator*(const Matrix4f& lhs, float rhs) noexcept
{
    Matrix4f res = lhs;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            res[i][j] *= rhs;
        }
    }
    return res;
}

Vector4f operator*(const Matrix4f& lhs, const Vector4f& rhs) noexcept
{
    float product[4];
    for (int i = 0; i < 4; ++i)
    {
        product[i] = lhs[i][0] * rhs[0] + lhs[i][1] * rhs[1]
                + lhs[i][2] * rhs[2] + lhs[i][3] * rhs[3];
    }
    return { product[0], product[1], product[2], product[3] };
}

ostream & operator<<(ostream& lhs, const Matrix4f& rhs) noexcept
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            lhs << rhs[i][j] << endl;
        }
    }
    return lhs;
}

Matrix4f& operator<<(Matrix4f& lhs, float rhs) noexcept
{
    static int row = 0;
    static int col = 0;
    if (col == 4)
    {
        col = 0;
        if (++row == 4) row = 0;
    }
    lhs.data[row][col++] = rhs;
    return lhs;
}

Matrix4f& operator,(Matrix4f& lhs, float rhs) noexcept
{
    return lhs << rhs;
};