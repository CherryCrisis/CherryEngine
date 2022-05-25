#include <cmath>


namespace CCMaths
{
	inline Vector4 Vector4::operator+(const Vector4& rhs) const
	{
		return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
	}

	inline Vector4 Vector4::operator+(const float rhs) const
	{
		return { x + rhs, y + rhs, z + rhs, w + rhs };
	}

	inline Vector4& Vector4::operator+=(const Vector4& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	inline Vector4& Vector4::operator+=(const float rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	inline Vector4 Vector4::operator-() const
	{
		return { -x, -y, -z, -w };
	}

	inline Vector4 Vector4::operator-(const Vector4& rhs) const
	{
		return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
	}

	inline Vector4 Vector4::operator-(const float rhs) const
	{
		return { x - rhs, y - rhs, z - rhs, w - rhs };
	}

	inline Vector4& Vector4::operator-=(const Vector4& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	inline Vector4& Vector4::operator-=(const float rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	inline float Vector4::operator*(const Vector4& rhs) const
	{
		return Vector4::Dot(*this, rhs);
	}

	inline Vector4 Vector4::operator*(const float rhs) const
	{
		return { x * rhs, y * rhs, z * rhs, w * rhs };
	}

	inline Vector4& Vector4::operator*=(const float rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	inline Vector4 Vector4::operator/(const float rhs) const
	{
		return { x / rhs, y / rhs, z / rhs, w / rhs };
	}

	inline Vector4& Vector4::operator/=(const float rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	inline bool Vector4::operator==(const Vector4& rhs) const
	{
		return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
	}

	inline bool Vector4::operator!=(const Vector4& rhs) const
	{
		return !(*this == rhs);
	}

	inline bool Vector4::operator<=(const Vector4& rhs) const
	{
		return (x <= rhs.x && y <= rhs.y && z <= rhs.z && w <= rhs.w);
	}

	inline bool Vector4::operator<(const Vector4& rhs) const
	{
		return (x < rhs.x && y < rhs.y && z < rhs.z && w < rhs.w);
	}

	inline bool Vector4::operator>=(const Vector4& rhs) const
	{
		return (x >= rhs.x && y >= rhs.y && z >= rhs.z && w >= rhs.w);
	}

	inline bool Vector4::operator>(const Vector4& rhs) const
	{
		return (x > rhs.x && y > rhs.y && z > rhs.z && w > rhs.w);
	}

	inline float Vector4::Dot(const Vector4& rhs) const
	{
		return Vector4::Dot(*this, rhs);
	}

	inline float Vector4::Dot(const Vector4& lhs, const Vector4& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
	}

	inline float Vector4::SquareLength() const
	{
		return this->Dot(*this);
	}

	inline float Vector4::Length() const
	{
		return std::sqrt(this->SquareLength());
	}

	inline Vector4& Vector4::Normalize()
	{
		return Vector4::Normalize(*this);
	}

	inline Vector4& Vector4::Normalize(Vector4& outVector)
	{
		outVector = Normalized(outVector);
		return outVector;
	}

	inline Vector4 Vector4::Normalized() const
	{
		return Vector4::Normalized(*this);
	}

	inline Vector4 Vector4::Normalized(const Vector4& inVector)
	{
		float length = inVector.Length();
		float invLength = 1.f / (length == 0 ? 1.f : length);

		return inVector * invLength;
	}
}