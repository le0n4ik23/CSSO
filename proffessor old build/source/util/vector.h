#pragma once
#include <numbers>
#include <cstdint>
#include <cmath>

constexpr float Deg2Rad(const float deg) noexcept
{
	return deg * (std::numbers::pi_v<float> / 180.f);
}

class Vector3
{
public:
	// array access...
	float* operator[](int i) const;
	float& operator[](int i);


	constexpr Vector3 operator+(const Vector3& other) const noexcept
	{
		return { x + other.x, y + other.y, z + other.z };
	}

	constexpr Vector3 operator-(const Vector3& other) const noexcept
	{
		return { x - other.x, y - other.y, z - other.z };
	}

	constexpr Vector3 operator*(const Vector3& other) const noexcept
	{
		return { x * other.x, y * other.y, z * other.z };
	}

	constexpr Vector3 operator*(const float scale) const noexcept
	{
		return { x * scale, y * scale, z * scale };
	}

	constexpr Vector3 operator/(const Vector3& other) const noexcept
	{
		return { x / other.x, y / other.y, z / other.z };
	}

	constexpr Vector3 Scale(float factor) const noexcept
	{
		return { x * factor, y * factor, z * factor };
	}

	inline Vector3 ToAngle() const noexcept
	{
		return {
			std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>),
			std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>),
			0.0f
		};
	}

	//void Math::AngleVectors(const QAngle& angles, Vector& forward)
	//{
	//	Assert(s_bMathlibInitialized);
	//	Assert(forward);

	//	float sp, sy, cp, cy;

	//	SinCosX(DEG2RAD(angles[1]), sy, cy); // YAW = 1 (y)
	//	SinCosX(DEG2RAD(angles[0]), sp, cp); // PITCH = 0 (x)

	//	forward.x = cp * cy;
	//	forward.y = cp * sy;
	//	forward.z = -sp;
	//}
	inline void Normalize()
	{
		if (x > 90.0)
			y -= 180.0;
		else if (y < -90.0)
			y += 180.0;

		if (y > 180.0)
			y -= 360.0;
		else if (y < -180.0)
			y += 360.0;
	}

	inline Vector3 ToVector() const noexcept
	{
		return {
			std::cos(Deg2Rad(x)) * std::cos(Deg2Rad(y)),
			std::cos(Deg2Rad(x)) * std::sin(Deg2Rad(y)),
			-std::sin(Deg2Rad(x))
		};
	}

	inline void Init(float ix, float iy, float iz)
	{
		x = ix; y = iy; z = iz;
	}
	float LengthSqr(void) { return (x * x + y * y + z * z); }

	float Length2D(void) { return sqrt(x * x + y * y); }

	float x{ }, y{ }, z{ };

#define PI 3.1415927f

	Vector3 Subtract(Vector3 src, Vector3 dst)
	{
		Vector3 diff;
		diff.x = src.x - dst.x;
		diff.y = src.y - dst.y;
		diff.z = src.z - dst.z;
		return diff;
	}

	Vector3 Divide(Vector3 src, float num)
	{
		Vector3 vec;
		vec.x = src.x / num;
		vec.y = src.y / num;
		vec.z = src.z / num;

		return vec;
	}

	float Magnitude(Vector3 vec)
	{
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	float DotProduct(Vector3 src, Vector3 dst)
	{
		return src.x * dst.x + src.y * dst.y + src.z * dst.z;
	}

	Vector3 Forward()
	{
		return {x/179, y/89, z/49};
	}
};

struct Vector2
{
	float x, y;
};


struct Vector4
{
	float x, y, z, w;
};

class CMatrix3x4
{
public:
	constexpr float* operator[](size_t index) noexcept
	{
		return data[index];
	}

	constexpr Vector3 Origin() noexcept
	{
		return { data[0][3], data[1][3], data[2][3] };
	}

	float data[3][4];
};

class CMatrix4x4
{
public:
	float data[4][4];
};


struct MousePos
{
	long x;
	long y;
};