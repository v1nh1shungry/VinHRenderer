#ifndef GL_H
#define GL_H

#include "gmath.h"
#include "model.h"
#include "camera.h"
#include "light.h"
#include "shader.h"
#include "config.h"
#include "sigslot.h"

#include <vector>
#include <array>
#include <functional>

namespace vinh
{
	class Rasterizer : HASSLOT
	{
	public:
		Rasterizer(int _width = WIDTH, int _height = HEIGHT);
		~Rasterizer();
		void draw(const std::vector<Model*>& models, const Camera& camera, const Light& light);
		void setShader(int type);
		void clear();
		
		SIGNAL<const std::vector<std::array<unsigned char, 3>>&> drawFrame;

	private:
		int getIndex(int x, int y) const;
		void rasterizeModel(const Model& model, const Camera& camera, const Light& light);
		void rasterizeTriangle();
		void drawTriangle(int iface);
		Matrix4f lookat(const Camera& camera) const;
		Matrix4f getProjection(float fovy, float aspect, float zNear, float zFar) const;

	private:
		bool msaa{ false };
		int width;
		int height;
		Shader* shader{ new WhiteShader };
		std::vector<std::array<unsigned char, 3>> frameBuf;
		std::vector<float> zBuf;
	};
}

#endif
