#include "light.h"

using namespace vinh;

Light::Light(float x /* = 1.f */, float y /* = 1.f */, float z /* = 1.f */, float _identity /* = 10.f */)
{
	position[0] = x;
	position[1] = y;
	position[2] = z;
	identity[0] = identity[1] = identity[2] = _identity;
}

void Light::setup(float x, float y, float z, float _identity)
{
	position[0] = x;
	position[1] = y;
	position[2] = z;
	identity[0] = identity[1] = identity[2] = _identity;
}