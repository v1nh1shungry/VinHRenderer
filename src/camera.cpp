#include "camera.h"
#include "config.h"

#include <cmath>

using namespace vinh;

Camera::Camera(const Vector3f& _eye, const Vector3f& _target, const Vector3f& _up) 
	: eye(_eye), target(_target), up(_up)
{
	;
}

void Camera::setup(const Vector3f& _eye, const Vector3f& _target, const Vector3f& _up)
{
	eye = _eye;
	target = _target;
	up = _up;
}

void Camera::update(Vector2f orbitDelta, Vector2f fvDelta,
	float wheelDelta, int width, int height)
{
	Vector3f z = (eye - target).normalize();
	Vector3f x = up.cross(z).normalize();
	Vector3f y = z.cross(x);
	Vector3f fromTarget = eye - target;
	float radius = fromTarget.norm();
	float phi = atan2f(fromTarget[0], fromTarget[2]);	// angle between z-axis and eye-target, i guess :)
	float theta = acosf(fromTarget[1] / radius);	// clearly angle between eye-target and y-axis, and remember 'val' should be [-1, 1]
	float deltaX = orbitDelta[0] / width;
	float deltaY = orbitDelta[1] / height;
	// wheel
	radius *= powf(0.95f, wheelDelta);
	// left button
	float factor = 1.5f * PI;
	phi += factor * deltaX;
	theta += factor * deltaY;
	if (PI < theta) theta = PI - EPSILON * 100;
	if (theta < 0) theta = EPSILON * 100;
	eye[0] = target[0] + radius * sinf(phi) * sinf(theta);
	eye[1] = target[1] + radius * cosf(theta);
	eye[2] = target[2] + radius * cosf(phi) * sinf(theta);
	// right button
	factor = radius * tanf(60.f / 360.f * PI) * 2.2f;
	deltaX = fvDelta[0] / width;
	deltaY = fvDelta[1] / height;
	Vector3f left = x * deltaX * factor;
	Vector3f _up = y * deltaY * factor;
	eye = eye + (left - _up);
	target = target + (left - _up);
}