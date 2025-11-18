// inverse log taper
inline float inv_log_taper(float x) { return x < 0.5f ? x * 1.8f : x * 0.2f + 0.8f; }

// log taper
inline float log_taper(float x) { return x < 0.5f ? x * 0.2f : x * 1.8f - 0.8f; }
