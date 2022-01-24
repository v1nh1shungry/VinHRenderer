#ifndef MODEL_H
#define MODEL_H

#include "tgaimage.h"
#include "matrix.h"

#include <vector>

namespace vinh
{
	enum TextureType { Diffuse, Normal, Specular };

	class Model
	{
	public:
		Model(const char* filename, bool isMMD);
		~Model();

		size_t nverts() const;
		size_t nfaces() const;
		std::vector<size_t> face(size_t index) const;
		Vector3f vert(size_t iface, size_t nthvert) const;
		Vector3f vert(size_t index) const;
		Vector3f normal(size_t iface, size_t nthvert) const;
		Vector3f normal(const Vector2f& uv) const;
		Vector2f uv(size_t iface, size_t nthvert) const;
		Vector3f diffuse(const Vector2f& uv) const;
		float specular(const Vector2f& uv) const;
		void loadTexture(const char* filename, TextureType type);

		// textures
		TGAImage* diffuseMap{ nullptr };
		TGAImage* normalMap{ nullptr };
		TGAImage* specularMap{ nullptr };

	private:
		std::vector<Vector3f> verts;
		std::vector<Vector2f> uvs;
		std::vector<Vector3f> norms;
		std::vector<std::vector<size_t>> faces;
	};
}

#endif
