#define NUM_TAPER_POINTS 5
const float deadzone_width = 0.1f;

class Taper
{
public:
  Taper() {}
  void Init();

  // Log taper
  inline float Log(float x) { return x < 0.5f ? x * 0.2f : x * 1.8f - 0.8f; }

  // Inverse log taper
  inline float InvLog(float x) { return x < 0.5f ? x * 1.8f : x * 0.2f + 0.8f; }

  // Dead zone taper
  float DeadZone(float x);

  // Log with dead zone
  float LogDeadZone(float x);

private:
  struct Point {
    float x, y;
  };

  struct Line {
    float a, b;
  };

  Line lines[NUM_TAPER_POINTS - 1];
  Point points[NUM_TAPER_POINTS] = {{0, 0}, {deadzone_width, 0}, {0.5f, 0.1f}, {1.0f - deadzone_width, 1.0f}, {1.0f, 1.0f}};

  Line CalcLine(Point p1, Point p2);
  void CalcLogWithDeadzones();
};

