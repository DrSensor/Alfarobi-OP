#define HEAD_PAN_MAX    1024
#define HEAD_PAN_MIN    512
#define HEAD_TILT_MAX   1024
#define HEAD_TILT_MIN   512

#ifndef HEAD_H
#define HEAD_H

#include <stdint-gcc.h>
#include <cmath>
#include "subcontroller.h"

//#define PI

class Head
{
public:
    static Head* GetInstance() { return m_UniqueInstance; }

    void targetLock(uint16_t frame_x, uint16_t frame_y);
    void sinusoidalSearch(const uint16_t degree_increment, const useconds_t delay_steps);

    void sweepLeft(const uint16_t panAngle_offset, const uint16_t tiltAngle_increment, const useconds_t delay_step);
    void sweepRight(const uint16_t panAngle_offset);

    void moveAt(const uint16_t x, const uint16_t y);
    void cosinusMove(const uint16_t degree, const int multiplier);

    double getPanAngle() const { return (pan-HEAD_PAN_MIN)*360/raw_pan_size; }
    double getTiltAngle() const { return (tilt-HEAD_TILT_MIN)*360/raw_tilt_size; }

private:
    static Head* m_UniqueInstance;
    uint32_t pan, tilt, raw_pan_size, raw_tilt_size;

    uint32_t degree2raw(const uint16_t degree);
};

#endif // HEAD_H
