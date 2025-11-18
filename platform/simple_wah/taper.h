// inverse log taper
inline float inv_log_taper(float x) { return x < 0.5f ? x * 1.8f : x * 0.2f + 0.8f; }

// log taper
inline float log_taper(float x) { return x < 0.5f ? x * 0.2f : x * 1.8f - 0.8f; }

// dead zone taper
inline float dead_zone_taper(float x) { 
    if(x < 0.1) return 0.0f;
    else if(x > 0.9) return 1.0f;
    else return 1.25f * x - 0.125f; 
 }
