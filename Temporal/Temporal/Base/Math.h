#pragma once

static const float  PI = 3.14159265358979f;

static float toRadians(float angle) { return angle * PI / 180; }
static float toDegrees(float angle) { return angle * 180 / PI; }