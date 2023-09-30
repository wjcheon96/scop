#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>

struct Vector4 {
	float x, y, z, w;

	Vector4(void);
	Vector4(float value);
	Vector4(const Vector4& copy);
	Vector4(float x, float y, float z, float w);
	Vector4& operator=(const Vector4& copy);

	bool Equals(Vector4 vector);

	Vector4 Scale(float scale);
	Vector4 Set(float x, float y, float z, float w);
	Vector4 GetNormalized(void);

	static float* GetValue(Vector4& value);
	static const float* GetValue(const Vector4& value);

	static float Dot(const Vector4& a, const Vector4& b);
	static float Angle(const Vector4& a, const Vector4& b);
	static float Magnitude(const Vector4& a);
	static float Distance(const Vector4& a, const Vector4& b);
	static Vector4 Min(const Vector4& a, const Vector4& b);
	static Vector4 Max(const Vector4& a, const Vector4& b);
	static Vector4 Add(const Vector4& a, const Vector4& b);
	static Vector4 Sub(const Vector4& a, const Vector4& b);
	static Vector4 Mul(float a, const Vector4& b) ;
	static Vector4 Mul(const Vector4& a, const Vector4& b);

	static float getClamp(float value, float minVal, float maxVal);

	static Vector4 Clamp(const Vector4& vector, float minVal, float maxVal);

	friend bool operator==(Vector4& lhs, const Vector4& rhs);
	friend bool operator!=(Vector4& lhs, const Vector4& rhs);
	friend Vector4 operator+(const Vector4& lhs, const Vector4& rhs);
	friend Vector4 operator+=(Vector4& lhs, const Vector4& rhs);
	friend Vector4 operator-(const Vector4& lhs, const Vector4& rhs);
	friend Vector4 operator-=(Vector4& lhs, const Vector4& rhs);
	friend Vector4 operator*(const Vector4& lhs, const float& rhs);
	friend Vector4 operator*=(Vector4& lhs, const float& rhs);
	friend Vector4 operator^(const Vector4& lhs, const Vector4& rhs);
	friend float operator&(const Vector4& lhs, const Vector4& rhs);

	friend std::ostream& operator<<(std::ostream& os, const Vector4& v);

};