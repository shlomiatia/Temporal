#pragma once

static const float  PI = 3.14159265358979f;

inline float toRadians(float angle) { return angle * PI / 180; }
inline float toDegrees(float angle) { return angle * 180 / PI; }