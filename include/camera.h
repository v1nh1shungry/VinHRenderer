#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"
#include "sigslot.h"

namespace vinh
{
	struct Camera : HASSLOT
	{
		Camera(const Vector3f& _eye, const Vector3f& _target, const Vector3f& _up);
		~Camera() = default;

		// slots
		void setup(const Vector3f& _eye, const Vector3f& _target, const Vector3f& _up);
		void update(Vector2f orbitDelta, Vector2f fvDelta,
			float wheelDelta, int width, int height);

		Vector3f eye;
		Vector3f target;
		Vector3f up;
	};
}

#endif
