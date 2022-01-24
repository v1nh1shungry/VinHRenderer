#include "shader.h"
#include "gmath.h"

#include <cmath>

#undef max
#undef min

using namespace vinh;

TextureShader::TextureShader()
{
	type = Texture;
}

Vector3f TextureShader::fragment(float alpha, float beta, float gamma)
{
	auto uvs = payload.uv;
	auto normals = payload.normal;
	auto worldcoord = payload.worldcoord;
	auto clipcoord = payload.clipcoord;
	auto uv = interpolate(alpha, beta, gamma, uvs[0], uvs[1], uvs[2],
		clipcoord[0].w(), clipcoord[1].w(), clipcoord[2].w());
	auto normal = interpolate(alpha, beta, gamma, normals[0], normals[1], normals[2],
		clipcoord[0].w(), clipcoord[1].w(), clipcoord[2].w());
	auto world = interpolate(alpha, beta, gamma, worldcoord[0], worldcoord[1], worldcoord[2],
		clipcoord[0].w(), clipcoord[1].w(), clipcoord[2].w());

	Vector3f kd{ 0.7f, 0.7f, 0.7f };
	if (payload.model->diffuseMap)
	{
		kd = payload.model->diffuse(uv);
	}
	Vector3f ka = kd;
	Vector3f ks(0.8f, 0.8f, 0.8f);

	Vector3f ambIdensity(0.35f, 0.35f, 0.35f);

	float p = 150;
	Vector3f viewDir = (payload.camera->eye - world).normalize();
	Vector3f lightDir = payload.light->position - world;
	float r2 = lightDir * lightDir;
	lightDir.normalized();
	normal.normalized();
	Vector3f h = (lightDir + viewDir).normalize();

	Vector3f amb = ka.cwiseProduct(ambIdensity);
	Vector3f diff = kd.cwiseProduct(payload.light->identity / r2) * std::max(0.f, normal * lightDir);
	Vector3f spec = ks.cwiseProduct(payload.light->identity / r2) * pow(std::max(0.f, normal * h), p);
	
	return (amb + diff + spec) * 255.f;
}