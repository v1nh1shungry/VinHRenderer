#include "shader.h"
#include "gmath.h"

namespace vinh
{
	DepthShader::DepthShader()
	{
		type = Depth;
	}

	Vector3f DepthShader::fragment(float alpha, float beta, float gamma)
	{
		// the nearer object looks blacker, cause abs(z)
		Vector4f* clipcoord = payload.clipcoord;
		float z = interpolate(alpha, beta, gamma, -clipcoord[0].w(), -clipcoord[1].w(), -clipcoord[2].w(),
			clipcoord[0].w(), clipcoord[1].w(), clipcoord[2].w());
		return { 70.f * z, 70.f * z, 70.f * z };
	}
}