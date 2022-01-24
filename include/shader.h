#ifndef SHADER_H
#define SHADER_H

#include "model.h"
#include "camera.h"
#include "light.h"
#include "config.h"

namespace vinh
{
	struct Payload
	{
		const Model* model{};
		const Camera* camera{};
		const Light* light{};

		Matrix4f mvp;

		Vector2f uv[3];
		Vector3f normal[3];
		Vector3f worldcoord[3];
		Vector4f clipcoord[3];

		Vector2f inUV[MAX_VERTEX];
		Vector3f inNormal[MAX_VERTEX];
		Vector3f inWorld[MAX_VERTEX];
		Vector4f inClip[MAX_VERTEX];

		Vector2f outUV[MAX_VERTEX];
		Vector3f outNormal[MAX_VERTEX];
		Vector3f outWorld[MAX_VERTEX];
		Vector4f outClip[MAX_VERTEX];
	};

	enum
	{
		White,
		Depth,
		Phong,
		Texture
	};

	struct Shader
	{
		Payload payload;
		int type;

		virtual ~Shader() = default;
		virtual void vertex(int iface, int nthvert)
		{
			Vector3f vert = payload.model->vert(iface, nthvert);
			payload.inUV[nthvert] = payload.model->uv(iface, nthvert);
			payload.inWorld[nthvert] = vert;
			payload.inNormal[nthvert] = payload.model->normal(iface, nthvert);
			payload.inClip[nthvert] = payload.mvp * Vector4f{ vert.data[0], vert.data[1], vert.data[2], 1.f };
		}
		virtual Vector3f fragment(float alpha, float beta, float gamma) = 0;
	};

	struct WhiteShader : public Shader
	{
		WhiteShader();
		~WhiteShader() = default;
		Vector3f fragment(float alpha, float beta, float gamma) override;
	};

	struct DepthShader : public Shader
	{
		DepthShader();
		~DepthShader() = default;
		Vector3f fragment(float alpha, float beta, float gamma) override;
	};

	struct PhongShader : public Shader
	{
		PhongShader();
		~PhongShader() = default;
		Vector3f fragment(float alpha, float beta, float gamma) override;
	};
	
	struct TextureShader : public Shader
	{
		TextureShader();
		~TextureShader() = default;
		Vector3f fragment(float alpha, float beta, float gamma) override;
	};
}

#endif
