#include "gl.h"
#include "log.h"

#include <cmath>

#define FLT_MAX (100000.f)

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace vinh;

static std::tuple<float, float, float> barycentric(float x, float y, const Vector3f* v)
{
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	return { c1, c2, 1 - c1 - c2 };
}

// return `true` if should be clipped
static bool backfaceClip(const Vector3f* ndc)
{
	const Vector3f& a = ndc[0];
	const Vector3f& b = ndc[1];
	const Vector3f& c = ndc[2];
	float signed_area = a.x() * b.y() - a.y() * b.x() +
		b.x() * c.y() - b.y() * c.x() +
		c.x() * a.y() - c.y() * a.x();   //|AB AC|
	return signed_area <= 0;
}

static bool insideTriangle(float alpha, float beta, float gamma)
{
	return -EPSILON < alpha && -EPSILON < beta && -EPSILON < gamma;
}

enum ClipSide
{
	W_PLANE,
	X_RIGHT,
	X_LEFT,
	Y_TOP,
	Y_BOTTOM,
	Z_NEAR,
	Z_FAR
};

bool insidePlane(const Vector4f& v, ClipSide side)
{
	switch (side)
	{
	case W_PLANE:
		return v.w() <= -EPSILON;
	case X_RIGHT:
		return v.x() >= v.w();
	case X_LEFT:
		return v.x() <= -v.w();
	case Y_TOP:
		return v.y() >= v.w();
	case Y_BOTTOM:
		return v.y() <= -v.w();
	case Z_NEAR:
		return v.z() >= v.w();
	case Z_FAR:
		return v.z() <= -v.w();
	default:
		ERRORING("Unexpected valude '", side, "' in function 'insidePlane'.");
		return false;
	}
}

static float getIntersectRatio(const Vector4f& prev, const Vector4f& curv, ClipSide side)
{
	switch (side)
	{
	case W_PLANE:
		return (prev.w() + EPSILON) / (prev.w() - curv.w());
	case X_RIGHT:
		return (prev.w() - prev.x()) / ((prev.w() - prev.x()) - (curv.w() - curv.x()));
	case X_LEFT:
		return (prev.w() + prev.x()) / ((prev.w() + prev.x()) - (curv.w() + curv.x()));
	case Y_TOP:
		return (prev.w() - prev.y()) / ((prev.w() - prev.y()) - (curv.w() - curv.y()));
	case Y_BOTTOM:
		return (prev.w() + prev.y()) / ((prev.w() + prev.y()) - (curv.w() + curv.y()));
	case Z_NEAR:
		return (prev.w() - prev.z()) / ((prev.w() - prev.z()) - (curv.w() - curv.z()));
	case Z_FAR:
		return (prev.w() + prev.z()) / ((prev.w() + prev.z()) - (curv.w() + curv.z()));
	default:
		ERRORING("Unexpected value '", side, "' in function 'getIntersectRatio'.");
		return 0.f;
	}
}

static int clipSinglePlane(Payload& payload, int& num, ClipSide side)
{
	int count = 0;
	Vector2f* inUV, * outUV;
	Vector3f* inNormal, * outNormal, * inWorld, * outWorld;
	Vector4f* inClip, * outClip;
	if (!(side % 2))
	{
		inUV = payload.inUV;
		outUV = payload.outUV;
		inNormal = payload.inNormal;
		outNormal = payload.outNormal;
		inWorld = payload.inWorld;
		outWorld = payload.outWorld;
		inClip = payload.inClip;
		outClip = payload.outClip;
	}
	else
	{
		inUV = payload.outUV;
		outUV = payload.inUV;
		inNormal = payload.outNormal;
		outNormal = payload.inNormal;
		inWorld = payload.outWorld;
		outWorld = payload.inWorld;
		inClip = payload.outClip;
		outClip = payload.inClip;
	}
	for (int i = 0; i < num; ++i)
	{
		int curr = i, prev = i - 1;
		if (prev < 0) prev += num;
		bool isCurrInside = insidePlane(inClip[curr], side);
		bool isPrevInside = insidePlane(inClip[prev], side);
		if (isCurrInside != isPrevInside)
		{
			float ratio = getIntersectRatio(inClip[prev], inClip[curr], side);
			outUV[count] = lerp(inUV[prev], inUV[curr], ratio);
			outNormal[count] = lerp(inNormal[prev], inNormal[curr], ratio);
			outWorld[count] = lerp(inWorld[prev], inWorld[curr], ratio);
			outClip[count] = lerp(inClip[prev], inClip[curr], ratio);
			++count;
		}
		if (isCurrInside)
		{
			outUV[count] = inUV[curr];
			outNormal[count] = inNormal[curr];
			outWorld[count] = inWorld[curr];
			outClip[count] = inClip[curr];
			++count;
		}
	}
	return num = count;
}

// return the number of vertex after being clipped
// however, the homo clip now is broken, it can only protect program from crashing
static int homoClip(Payload& payload)
{
	int num = 3;
	for (int side = W_PLANE; side <= Z_FAR; ++side)
	{
		clipSinglePlane(payload, num, (ClipSide)side);
	}
	return num;
}

// just move the final vertex from output to attri
// so stupid~ :(
static void transformAttri(int index[], Payload& payload)
{
	for (int i = 0; i < 3; ++i)
	{
		payload.uv[i] = payload.outUV[index[i]];
		payload.normal[i] = payload.outNormal[index[i]];
		payload.worldcoord[i] = payload.outWorld[index[i]];
		payload.clipcoord[i] = payload.outClip[index[i]];
	}
}

void Rasterizer::rasterizeTriangle()
{
	Vector3f ndc[3], screen[3];
	Vector4f* clipcoord = shader->payload.clipcoord;
	for (int i = 0; i < 3; ++i)
	{
		ndc[i][0] = clipcoord[i][0] / clipcoord[i].w();
		ndc[i][1] = clipcoord[i][1] / clipcoord[i].w();
		ndc[i][2] = clipcoord[i][2] / clipcoord[i].w();
	}
	// which helps a little but not beautiful
	// if (backfaceClip(ndc)) return;
	for (int i = 0; i < 3; ++i)
	{
		screen[i][0] = .5 * (width - 1) * (ndc[i][0] + 1.0);
		screen[i][1] = .5 * (height - 1) * (ndc[i][1] + 1.0);
		screen[i][2] = -clipcoord[i].w();
	}
	float minX = FLT_MAX, maxX = -FLT_MAX, minY = FLT_MAX, maxY = -FLT_MAX;
	for (auto & i : screen)
	{
		minX = std::min(minX, i[0]);
		maxX = std::max(maxX, i[0]);
		minY = std::min(minY, i[1]);
		maxY = std::max(maxY, i[1]);
	}
	auto xMin = static_cast<int>(minX), xMax = static_cast<int>(maxX),
		yMin = static_cast<int>(minY), yMax = static_cast<int>(maxY);
	for (int x = xMin; x <= xMax; ++x)
	{
		for (int y = yMin; y <= yMax; ++y)
		{
			auto [alpha, beta, gamma] = barycentric(x + .5f, y + .5f, screen);
			if (insideTriangle(alpha, beta, gamma))
			{
				float z = interpolate(alpha, beta, gamma, screen[0].z(), screen[1].z(), screen[2].z(),
					clipcoord[0].w(), clipcoord[1].w(), clipcoord[2].w());
				int index = getIndex(x, y);
				if (zBuf[index] > z)
				{
					zBuf[index] = z;
					Vector3f color = shader->fragment(alpha, beta, gamma);
					for (int i = 0; i < 3; ++i)
					{
						color[i] = clamp(color[i], 0.f, 255.f);
						frameBuf[index][i] = static_cast<unsigned char>(color[i]);
					}
				}
			}
		}
	}
}

void Rasterizer::drawTriangle(int iface)
{
	for (int i = 0; i < 3; ++i)
	{
		shader->vertex(iface, i);
	}
	int num = homoClip(shader->payload);
	for (int i = 0; i < num - 2; ++i)
	{
		int index[]{ i, i + 1, i + 2 };
		transformAttri(index, shader->payload);
		rasterizeTriangle();
	}
}

void Rasterizer::rasterizeModel(const Model& model, const Camera& camera, const Light& light)
{
	shader->payload.model = &model;
	for (int i = 0; i < model.nfaces(); ++i)
	{
		drawTriangle(i);
	}
}

// okay, here's the case:
// models into `draw` func, and then each model into `rasterizeModel`
// then each triangle of that model into rasterizeTriangle
// that's the workflow
void Rasterizer::draw(const std::vector<Model*>& models, const Camera& camera, const Light& light)
{
	Matrix4f modelview = lookat(camera),
		projection = getProjection(60.f, (float)width / height, -.1f, -10000.f);
	shader->payload.camera = &camera;
	shader->payload.light = &light;
	shader->payload.mvp = projection * modelview;
	for (auto& model : models)
	{
		rasterizeModel(*model, camera, light);
	}
	drawFrame(frameBuf);
}

Rasterizer::Rasterizer(int _width /* = WIDTH */, int _height /* = HEIGHT */)
	: width(_width), height(_height)
{
	frameBuf.resize(width * height);
	zBuf.resize(width * height, FLT_MAX);
}

Rasterizer::~Rasterizer()
{
	delete shader;
}

void Rasterizer::clear()
{
	for (int i = width * height; --i; )
	{
		frameBuf[i][0] = 56;
		frameBuf[i][1] = 56;
		frameBuf[i][2] = 80;
	}
	std::fill(zBuf.begin(), zBuf.end(), FLT_MAX);
}

void Rasterizer::setShader(int type)
{
	if (shader->type != type)
	{
		delete shader;
		switch (type)
		{
		case vinh::White:
			shader = new WhiteShader;
			break;
		case vinh::Depth:
			shader = new DepthShader;
			break;
		case vinh::Phong:
			shader = new PhongShader;
			break;
		case vinh::Texture:
			shader = new TextureShader;
			break;
		default:
			ERRORING("Unexpected shader type '", type, "' in function 'Rasterizer::setShader'.");
			break;
		}
	}
}

int Rasterizer::getIndex(int x, int y) const
{
	return (height - 1 - y) * width + x;
}

Matrix4f Rasterizer::lookat(const Camera& camera) const
{
	Vector3f z = (camera.eye - camera.target).normalize();
	Vector3f x = camera.up.cross(z).normalize();
	Vector3f y = z.cross(x);
	Matrix4f res = Matrix4f::identity();
	for (int i = 0; i < 3; ++i)
	{
		res[0][i] = x[i];
		res[1][i] = y[i];
		res[2][i] = z[i];
	}
	res[0][3] = -(x * camera.eye);
	res[1][3] = -(y * camera.eye);
	res[2][3] = -(z * camera.eye);
	return res;
}

Matrix4f Rasterizer::getProjection(float fovy, float aspect, float zNear, float zFar) const
{
	float top = fabs(zNear) * tanf(fovy / 2.f / 180.f * PI);
	float right = top * aspect;
	Matrix4f res;
	res[0][0] = zNear / right;
	res[1][1] = zNear / top;
	res[2][2] = (zNear + zFar) / (zNear - zFar);
	res[2][3] = -2 * zNear * zFar / (zNear - zFar);
	res[3][2] = 1;
	return res;
}