#ifndef LIGHT_H
#define LIGHT_H

#include "matrix.h"
#include "sigslot.h"

namespace vinh
{
	struct Light : HASSLOT
	{
		Light(float x = 1.f, float y = 1.f, float z = 1.f, float _identity = 2.f);

		void setup(float x, float y, float z, float _identity);

		Vector3f position;
		Vector3f identity;
	};
}

#endif
