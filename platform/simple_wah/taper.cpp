#include "taper.h"

const float deadzone_width = 0.1f;

Line lines[NUM_TAPER_POINTS - 1];
Point points[NUM_TAPER_POINTS] = {{0, 0},
                            {deadzone_width, 0},
                            {0.5f, 0.1f},
                            {1.0f - deadzone_width, 1.0f},
                            {1.0f, 1.0f}};

Line calc_line(Point p1, Point p2) {
  // calculate the slope
  float a = (p2.y - p1.y) / (p2.x - p1.x);
  // calculate the intercept
  float b = p1.y - a * p1.x;

  Line line = {a, b};
  return line;
}

void calc_taper_with_deadzones() {
  for (int p = 0; p < NUM_TAPER_POINTS - 1; p++) {
    lines[p] = calc_line(points[p], points[p + 1]);
  }
}

float log_dead_zone_taper(float x) {
  for (int p = 0; p < NUM_TAPER_POINTS - 1; p++) {
    if (x >= points[p].x && x < points[p + 1].x) {
      return lines[p].a * x + lines[p].b;
    }
  }

  return 0;
}

