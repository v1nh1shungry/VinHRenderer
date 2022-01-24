#include "model.h"
#include "log.h"
#include "config.h"

#include <fstream>
#include <sstream>
#include <memory>
#include <functional>
#include <cmath>

using namespace std;
using namespace vinh;

Model::Model(const char* filename, bool isMMD)
{
	unique_ptr<ifstream, function<void(ifstream*)>> pin(new ifstream, [filename](ifstream* in) {
		LOGGING("Closing obj file '", filename, "' ...");
		in->close();
		LOGGING("Finished.");
		delete in;
		});
	ifstream& in = *pin;
	in.open(filename, ifstream::in);
	if (in.fail())
	{
		ERRORING("Can't load obj file '", filename, "'.");
		return;
	}

	LOGGING("Loading obj file '", filename, "' ...");
	string line;
	while (!in.eof())
	{
		getline(in, line);
		istringstream iss(line);
		char trash;
		if (!line.compare(0, 2, "v "))
		{
			Vector3f vert;
			iss >> trash;
			for (int i = 0; i < 3; ++i)
			{
				iss >> vert[i];
			}
			verts.push_back(vert);
		}
		else if (!line.compare(0, 3, "vn "))
		{
			Vector3f norm;
			iss >> trash >> trash;
			for (int i = 0; i < 3; ++i)
			{
				iss >> norm[i];
			}
			norms.push_back(norm);
		}
		else if (!line.compare(0, 3, "vt "))
		{
			Vector2f uv;
			iss >> trash >> trash;
			for (int i = 0; i < 2; ++i)
			{
				iss >> uv[i];
			}
			if (isMMD) {uv[1] = 1 - uv[1];}
			uvs.push_back(uv);
		}
		else if (!line.compare(0, 2, "f "))
		{
			std::vector<size_t> face;
			int vert, text, norm, cnt = 0;
			iss >> trash;
			while (iss >> vert >> trash >> text >> trash >> norm)
			{
				face.push_back(--vert);
				face.push_back(--text);
				face.push_back(--norm);
				++cnt;
			}
			if (cnt != 3)
			{
				ERRORING("The obj file is supposed to be triangled.");
				return;
			}
			faces.push_back(face);
		}
	}
	LOGGING("#f: ", faces.size(), ", #v: ", verts.size(), ", #vt: ", uvs.size(), ", #vn: ", norms.size());
	LOGGING("Finished.");
}

Model::~Model()
{
	delete diffuseMap;
	delete normalMap;
	delete specularMap;
}

size_t Model::nverts() const
{
	return verts.size();
}

size_t Model::nfaces() const
{
	return faces.size();
}

std::vector<size_t> vinh::Model::face(size_t index) const
{
	vector<size_t> res;
	for (size_t i = 0; i < 3; ++i)
	{
		res.push_back(faces[index][i * 3]);
	}
	return res;
}

Vector3f Model::vert(size_t iface, size_t nthvert) const
{
	return verts[faces[iface][nthvert * 3]];
}

Vector3f Model::vert(size_t index) const
{
	return verts[index];
}

Vector2f Model::uv(size_t iface, size_t nthvert) const
{
	return uvs[faces[iface][nthvert * 3 + 1]];
}

Vector3f Model::normal(size_t iface, size_t nthvert) const
{
	return norms[faces[iface][nthvert * 3 + 2]];
}

Vector3f Model::normal(const Vector2f& uv) const
{
	if (!normalMap)
	{
		WARNING("No normal texture.");
		return { 0, 0, 0 };
	}
	float u = static_cast<float>(fmod(uv.u(), 1)) * static_cast<float>(normalMap->get_width());
	float v = static_cast<float>(fmod(uv.v(), 1)) * static_cast<float>(normalMap->get_height());
	TGAColor color = normalMap->get(u, v);
	Vector3f res;
	for (size_t i = 0; i < 3; ++i)
	{
		res[2 - i] = static_cast<float>(color[i]) / 255.f * 2.f - 1.f;
	}
	return res;
}

Vector3f vinh::Model::diffuse(const Vector2f& uv) const
{
	if (!diffuseMap)
	{
		WARNING("No diffuse texture.");
		return {255, 255, 255};
	}
	float u = static_cast<float>(fmod(uv.u(), 1)) * static_cast<float>(diffuseMap->get_width());
	float v = static_cast<float>(fmod(uv.v(), 1)) * static_cast<float>(diffuseMap->get_height());
	auto color = diffuseMap->get(u, v);
	Vector3f res;
	for (size_t i = 0; i < 3; ++i)
	{
		res[2 - i] = static_cast<float>(color[i]) / 255.f;
	}
	return res;
}

float vinh::Model::specular(const Vector2f& uv) const
{
	if (!specularMap)
	{
		WARNING("No specular texture.");
		return .01f;
	}
	float u = static_cast<float>(fmod(uv.u(), 1)) * static_cast<float>(specularMap->get_width());
	float v = static_cast<float>(fmod(uv.v(), 1)) * static_cast<float>(specularMap->get_height());
	return specularMap->get(u, v)[0];
}

static void loadTextureHelper(const char* filename, TGAImage*& image)
{
	if (image)
	{
		WARNING("Texture has already been loaded, now reloading texture file '", filename, "' ...");
		delete image;
	}
	image = new TGAImage;
	LOGGING("Loading texture file '", filename, "' ...");
	if (image->read_tga_file(filename))
	{
		image->flip_vertically();
		LOGGING("Finished.");
	}
	else
	{
		ERRORING("Loading texture file '", filename, "' failed.");
		delete image;
		image = nullptr;
	}
}

void Model::loadTexture(const char* filename, TextureType type)
{
	switch (type)
	{
	case vinh::Diffuse:
		loadTextureHelper(filename, diffuseMap);
		break;
	case vinh::Normal:
		loadTextureHelper(filename, normalMap);
		break;
	case vinh::Specular:
		loadTextureHelper(filename, specularMap);
		break;
	default:
		break;
	}
}