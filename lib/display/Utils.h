#pragma once


long safeMap(long x, long in_min, long in_max, long out_min, long out_max, byte scale) {
    uint16_t scale_factor = 1;
    if (scale > 0) {
        for (byte i = 0; i < scale; i++) scale_factor *= 10;
    }
    in_max = in_max * scale_factor;

    if (in_max == in_min) return out_min; // защита от деления на 0
    if (x <= in_min) return out_min;
    if (x >= in_max) return out_max;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}