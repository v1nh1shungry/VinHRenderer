#ifndef GRAPHICS_MATH_H
#define GRAPHICS_MATH_H

#include "matrix.h"

namespace vinh
{
	template <typename T>
	T lerp(const T& begin, const T& end, float factor) noexcept
	{
		return begin + (end - begin) * factor;
	}

	template <typename T>
	T clamp(T val, T min, T max)
	{
		return (val < min) ? min : ((max < val) ? max : val);
	}

	template <typename T>
	T interpolate(float alpha, float beta, float gamma,
		const T& val1, const T& val2, const T& val3,
		float correct1, float correct2, float correct3)
	{
		float Z = 1.f / (alpha / correct1 + beta / correct2 + gamma / correct3);
		return (alpha * val1 / correct1 + beta * val2 / correct2 + gamma * val3 / correct3) * Z;
	}
}

#endif
