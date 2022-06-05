#include <cmath>


namespace CCMaths
{
	inline Vector2 Vector2::operator+(const Vector2& rhs) const
	{
		return { x + rhs.x, y + rhs.y };
	}

	inline Vector2 Vector2::operator+(const float rhs) const
	{
		return { x + rhs, y + rhs };
	}

	inline Vector2& Vector2::operator+=(const Vector2& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	inline Vector2& Vector2::operator+=(const float rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	inline Vector2 Vector2::operator-() const
	{
		return { -x, -y };
	}

	inline Vector2 Vector2::operator-(const Vector2& rhs) const
	{
		return { x - rhs.x, y - rhs.y };
	}

	inline Vector2 Vector2::operator-(const float rhs) const
	{
		return { x - rhs, y - rhs };
	}

	inline Vector2& Vector2::operator-=(const Vector2& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	inline Vector2& Vector2::operator-=(const float rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	inline float Vector2::operator*(const Vector2& rhs) const
	{
		return Vector2::Dot(*this, rhs);
	}

	inline Vector2 Vector2::operator*(const float rhs) const
	{
		return { x * rhs, y * rhs };
	}

	inline Vector2& Vector2::operator*=(const float rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	inline Vector2 Vector2::operator/(const float rhs) const
	{
		return { x / rhs, y / rhs };
	}

	inline Vector2& Vector2::operator/=(const float rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	inline bool Vector2::operator==(const Vector2& rhs) const
	{
		return (Abs(x - rhs.x) <= epsilon &&
				Abs(y - rhs.y) <= epsilon);
	}

	inline bool Vector2::operator!=(const Vector2& rhs) const
	{
		return !(*this == rhs);
	}

	inline bool Vector2::operator<=(const Vector2& rhs) const
	{
		return (x <= rhs.x && y <= rhs.y);
	}

	inline bool Vector2::operator<(const Vector2& rhs) const
	{
		return (x < rhs.x && y < rhs.y);
	}

	inline bool Vector2::operator>=(const Vector2& rhs) const
	{
		return (x >= rhs.x && y >= rhs.y);
	}

	inline bool Vector2::operator>(const Vector2& rhs) const
	{
		return (x > rhs.x && y > rhs.y);
	}

	inline float Vector2::Dot(const Vector2& rhs) const
	{
		return Vector2::Dot(*this, rhs);
	}

	inline float Vector2::Dot(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	inline float Vector2::SquareLength() const
	{
		return this->Dot(*this);
	}

	inline float Vector2::Length() const
	{
		return 1.f / InverseSqrt(this->SquareLength());
	}

	inline Vector2& Vector2::Normalize()
	{
		return Vector2::Normalize(*this);
	}

	inline Vector2& Vector2::Normalize(Vector2& lhs)
	{
		lhs = Normalized(lhs);
		return lhs;
	}

	inline Vector2 Vector2::Normalized() const
	{
		return Vector2::Normalized(*this);
	}

	inline Vector2 Vector2::Normalized(const Vector2& lhs)
	{
		float length = lhs.SquareLength();
		length = length == 0.0f ? 1.f : length;

		return lhs * InverseSqrt(length);
	}
}