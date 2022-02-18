#include <cmath>

namespace CCMaths
{
	inline Vector3 Vector3::operator+(const Vector3& rhs) const
	{
		return { x + rhs.x, y + rhs.y, z + rhs.z };
	}

	inline Vector3 Vector3::operator+(const float rhs) const
	{
		return { x + rhs, y + rhs, z + rhs };
	}

	inline Vector3& Vector3::operator+=(const Vector3& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	inline Vector3& Vector3::operator+=(const float rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	inline Vector3 Vector3::operator-() const
	{
		return { -x, -y, -z };
	}

	inline Vector3 Vector3::operator-(const Vector3& rhs) const
	{
		return { x - rhs.x, y - rhs.y, z - rhs.z };
	}

	inline Vector3 Vector3::operator-(const float rhs) const
	{
		return { x - rhs, y - rhs, z - rhs };
	}

	inline Vector3& Vector3::operator-=(const Vector3& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	inline Vector3& Vector3::operator-=(const float rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	inline float Vector3::operator*(const Vector3& rhs) const
	{
		return Vector3::Dot(*this, rhs);
	}

	inline Vector3 Vector3::operator*(const float rhs) const
	{
		return { x * rhs, y * rhs, z * rhs };
	}

	inline Vector3& Vector3::operator*=(const float rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	inline Vector3 Vector3::operator^(const Vector3& rhs) const
	{
		return Vector3::Cross(*this, rhs);
	}

	inline Vector3 Vector3::operator/(const float rhs) const
	{
		return { x / rhs, y / rhs, z / rhs };
	}

	inline Vector3& Vector3::operator/=(const float rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	inline bool Vector3::operator==(const Vector3& rhs) const
	{
		return (x == rhs.x && y == rhs.y && z == rhs.z);
	}

	inline bool Vector3::operator!=(const Vector3& rhs) const
	{
		return !(*this == rhs);
	}

	inline float Vector3::Dot(const Vector3& rhs) const
	{
		return Vector3::Dot(*this, rhs);
	}

	inline float Vector3::Dot(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	inline Vector3 Vector3::Cross(const Vector3& rhs) const
	{
		return Vector3::Cross(*this, rhs);
	}

	inline Vector3 Vector3::Cross(const Vector3& lhs, const Vector3& rhs)
	{
		return
		{
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}

	inline float Vector3::SquareLength() const
	{
		return this->Dot(*this);
	}

	inline float Vector3::Length() const
	{
		return std::sqrt(this->SquareLength());
	}

	inline Vector3& Vector3::Normalize()
	{
		return Vector3::Normalize(*this);
	}

	inline Vector3& Vector3::Normalize(Vector3& lhs)
	{
		lhs = Normalized(lhs);
		return lhs;
	}

	inline Vector3 Vector3::Normalized() const
	{
		return Vector3::Normalized(*this);
	}

	inline Vector3 Vector3::Normalized(const Vector3& lhs)
	{
		float length = lhs.Length();
		float invLength = 1.f / (length == 0.f ? 1.f : length);

		return lhs * invLength;
	}
}