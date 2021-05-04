#pragma once

#include <cmath>

struct Vector2
{
	float x;
	float y;
	inline Vector2(float x, float y):
		x(x), y(y)
	{}

	inline Vector2 operator*(float b) {
		return Vector2(x * b, y * b);
	}

	inline Vector2 operator/(float b) {
		return Vector2(x / b, y / b);
	}

	inline Vector2 operator+(Vector2 b) {
		return Vector2(x + b.x, y + b.y);
	}

	inline Vector2 operator-(Vector2 b) {
		return Vector2(x - b.x, y - b.y);
	}

	inline void operator +=(Vector2 b) {
		x += b.x;
		y += b.y;
	}

	inline static float distance(Vector2 a, Vector2 b) {
		return std::sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	}

	inline static float distance_squared(Vector2 a, Vector2 b) {
		return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
	}

	inline static Vector2 zero() {
		return Vector2(0.0f, 0.0f);
	}

	inline Vector2 normalized() {
		float norm = std::sqrtf(x*x + y*y);
		if (norm == 0)
			return zero();
		return (*this) / norm;
	}
};

