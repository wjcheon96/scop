#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>

struct Vector3 {
	float x, y, z;

	static Vector3 Zero;
	static Vector3 One;
	static Vector3 Forward;
	static Vector3 Backward;
	static Vector3 Up;
	static Vector3 Down;
	static Vector3 Left;
	static Vector3 Right;

	Vector3(void);
	Vector3(float value);
	Vector3(const Vector3& copy);
	Vector3(float x, float y, float z);
	Vector3& operator=(const Vector3& copy);

	bool Equals(Vector3 vector);

	Vector3 Scale(float scale);
	Vector3 Set(float x, float y, float z);
	Vector3 GetNormalized(void);

	static float* GetValue(Vector3& value);
	static const float* GetValue(const Vector3& value);

	static float Dot(const Vector3& a, const Vector3& b);
	static Vector3 Cross(const Vector3& a, const Vector3& b);
	static float Angle(const Vector3& a, const Vector3& b);
	static float Magnitude(const Vector3& a);
	static float Distance(const Vector3& a, const Vector3& b);
	static Vector3 Min(const Vector3& a, const Vector3& b);
	static Vector3 Max(const Vector3& a, const Vector3& b);
	static Vector3 Add(const Vector3& a, const Vector3& b);
	static Vector3 Sub(const Vector3& a, const Vector3& b);
	static Vector3 Mul(float a, const Vector3& b) ;
	static Vector3 Mul(const Vector3& a, const Vector3& b);

	static float getClamp(float value, float minVal, float maxVal);

	static Vector3 Clamp(const Vector3& vector, float minVal, float maxVal);

	friend bool operator==(Vector3& lhs, const Vector3& rhs);
	friend bool operator!=(Vector3& lhs, const Vector3& rhs);
	friend Vector3 operator+(const Vector3& lhs, const Vector3& rhs);
	friend Vector3 operator+=(Vector3& lhs, const Vector3& rhs);
	friend Vector3 operator-(const Vector3& lhs, const Vector3& rhs);
	friend Vector3 operator-=(Vector3& lhs, const Vector3& rhs);
	friend Vector3 operator*(const Vector3& lhs, const float& rhs);
	friend Vector3 operator*=(Vector3& lhs, const float& rhs);
	friend Vector3 operator^(const Vector3& lhs, const Vector3& rhs);
	friend float operator&(const Vector3& lhs, const Vector3& rhs);

	friend std::ostream& operator<<(std::ostream& os, const Vector3& v);

};