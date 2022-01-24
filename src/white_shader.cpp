#include "shader.h"

namespace vinh
{
	WhiteShader::WhiteShader()
	{
		type = White;
	}

	Vector3f WhiteShader::fragment(float alpha, float beta, float gamma)
	{
		return { 255.f, 255.f, 255.f };
	}
}