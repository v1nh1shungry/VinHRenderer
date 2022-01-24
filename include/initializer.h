#ifndef INITIALIZER_H
#define INITIALIZER_H

#include "model.h"
#include "sigslot.h"

#include <vector>
#include <memory>
#include <fstream>
#include <functional>

namespace vinh
{
	class Initializer
	{
	public:
		Initializer(const char* _filename);
		~Initializer() = default;
		std::vector<Model*> parse();
		std::string title();

		SIGNAL<int> setShader;
		SIGNAL<const Vector3f&, const Vector3f&, const Vector3f&> setCamera;
		SIGNAL<float, float, float, float> setLight;

	private:
		std::string filename;
	};
}

#endif
