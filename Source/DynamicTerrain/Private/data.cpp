#include "data.h"

#include <math.h>

float Vector3::magnitude() const
{
	return sqrt(x * x + y * y + z * z);
}

Vector3 normalize(Vector3 vec)
{
	float mag = vec.magnitude();
	return Vector3(vec.x / mag, vec.y / mag, vec.z / mag);
}

Vector3 cross(Vector3 a, Vector3 b)
{
	return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}