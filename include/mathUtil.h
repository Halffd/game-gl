float lerp(float delta, float start, float end) {
    return start + delta * (end - start);
}double clampedLerp(double start, double end, double delta) {
    if (delta < (double)0.0F) {
        return start;  // Don't go past the start
    } else {
        return delta > (double)1.0F ? end : lerp(delta, start, end); // Don't go past the end
    }
}