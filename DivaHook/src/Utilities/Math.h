#pragma once
#include <cmath>
#include "Vec2.h"
#include "Vec3.h"

namespace DivaHook::Utilities
{
	float ToDegrees(float radians);
	float ToRadians(float degrees);

	Vec2 GetDirection(float degrees);
	Vec2 PointFromAngle(float degrees, float distance);
	float AngleFromPoints(Vec2 p0, Vec2 p1);
}