#include "taper.h"

void Taper::Init() { CalcLogWithDeadzones(); }

Taper::Line Taper::CalcLine(Point p1, Point p2)
{
  // calculate the slope
  float a = (p2.y - p1.y) / (p2.x - p1.x);
  // calculate the intercept
  float b = p1.y - a * p1.x;

  Line line = {a, b};
  return line;
}

void Taper::CalcLogWithDeadzones()
{
  for (int p = 0; p < NUM_TAPER_POINTS - 1; p++) {
    lines[p] = CalcLine(points[p], points[p + 1]);
  }
}

float Taper::LogDeadZone(float x)
{
  for (int p = 0; p < NUM_TAPER_POINTS - 1; p++) {
    if (x >= points[p].x && x < points[p + 1].x) {
      return lines[p].a * x + lines[p].b;
    }
  }

  return 0;
}

float Taper::DeadZone(float x)
{
  if (x < 0.1)
    return 0.0f;
  else if (x > 0.9)
    return 1.0f;
  else
    return 1.25f * x - 0.125f;
}

