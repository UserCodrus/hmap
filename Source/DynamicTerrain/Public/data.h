#pragma once

struct Vector2
{
	Vector2() : x(0.0f), y(0.0f) {};
	Vector2(float _x, float _y) : x(_x), y(_y) {};

	float x, y;
};

struct Vector3
{
	Vector3() : x(0.0f), y(0.0f), z(0.0f) {};
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};

	inline float magnitude() const;

	float x, y, z;
};

// Return a unit vector in the same direction as the input vector
Vector3 normalize(Vector3 vec);
// Return the cross product of two vectors
Vector3 cross(Vector3 a, Vector3 b);