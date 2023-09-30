#include "Vector4.h"

Vector4::Vector4(void): x(0.0f), y(0.0f), z(0.0f), w(0.0f) {};
Vector4::Vector4(float value): x(value), y(value), z(value), w(value) {};
Vector4::Vector4(const Vector4& copy): x(copy.x), y(copy.y), z(copy.z), w(copy.w) {};
Vector4::Vector4(float x, float y, float z, float w): x(x), y(y), z(z), w(w) {};

Vector4& Vector4::operator=(const Vector4& copy) {
	this->x = copy.x;
	this->y = copy.y;
	this->z = copy.z;
	this->w = copy.w;
	return *this;
}

Vector4 Vector4::Scale(float scale) {
	this->x *= scale;
	this->y *= scale;
	this->z *= scale;
	this->w *= scale;
	return *this;
}

Vector4 Vector4::Set(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
	return *this;
}

Vector4 Vector4::GetNormalized(void) {
	Vector4 destination(x, y, z, w);
	float invMagnitude = 1.0f / (float) Vector4::Magnitude(destination);
	destination.x *= invMagnitude;
	destination.y *= invMagnitude;
	destination.z *= invMagnitude;
	destination.w *= invMagnitude;
	return destination;
}

float* Vector4::GetValue(Vector4& value) {
	return &value.x;
}

const float* Vector4::GetValue(const Vector4& value) {
	return &value.x;
}

bool Vector4::Equals(Vector4 vector) {
	return x == vector.x && y == vector.y && z == vector.z && w == vector.w;
}

float Vector4::Dot(const Vector4& a, const Vector4& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float Vector4::Angle(const Vector4& a, const Vector4& b) {
	float angle = Vector4::Dot(a, b);
	angle /= Vector4::Magnitude(a) * Vector4::Magnitude(b);
	return acosf(angle);
}

float Vector4::Magnitude(const Vector4& a) {
	return sqrtf(Vector4::Dot(a, a));
}

float Vector4::Distance(const Vector4& a, const Vector4& b) {
	return Magnitude(Sub(a, b));
}

Vector4 Vector4::Min(const Vector4& a, const Vector4& b) {
	Vector4 c;
	c.x = a.x > b.x ? b.x : a.x;
	c.y = a.y > b.y ? b.y : a.y;
	c.z = a.z > b.z ? b.z : a.z;
    c.w = a.w > b.w ? b.w : a.w;
	return c;
}

Vector4 Vector4::Max(const Vector4& a, const Vector4& b) {
	Vector4 c;
	c.x = a.x < b.x ? b.x : a.x;
	c.y = a.y < b.y ? b.y : a.y;
	c.z = a.z < b.z ? b.z : a.z;
    c.w = a.w < b.w ? b.w : a.w;
	return c;
}

Vector4 Vector4::Add(const Vector4& a, const Vector4& b) {
	return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

Vector4 Vector4::Sub(const Vector4& a, const Vector4& b) {
	return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

Vector4 Vector4::Mul(const Vector4& a, const Vector4& b) {
	return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

Vector4 Vector4::Mul(float a, const Vector4& b) {
	return Vector4(a * b.x, a * b.y, a * b.z, a * b.w);
}

float getClamp(float value, float minVal, float maxVal) {
	if (value < minVal) {
		return minVal;
    } else if (value > maxVal) {
		return maxVal;
    } else {
		return value;
    }
}

Vector4 Vector4::Clamp(const Vector4& vector, float minVal, float maxVal) {
	Vector4 vec = Vector4();
	vec.x = getClamp(vec.x, minVal, maxVal);
	vec.y = getClamp(vec.y, minVal, maxVal);
	vec.z = getClamp(vec.z, minVal, maxVal);
	return vec;
}


bool operator==(Vector4& lhs, const Vector4& rhs)
{
	return lhs.Equals(rhs);
}

bool operator!=(Vector4& lhs, const Vector4& rhs)
{
	return !lhs.Equals(rhs);
}

Vector4 operator+(const Vector4& lhs, const Vector4& rhs)
{
	return Vector4::Add(lhs, rhs);
}

Vector4 operator+=(Vector4& lhs, const Vector4& rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
    lhs.w += rhs.w;
	return lhs;
}

Vector4 operator*(const Vector4& lhs, const float& rhs) {
	return Vector4::Mul(rhs, lhs);
}
