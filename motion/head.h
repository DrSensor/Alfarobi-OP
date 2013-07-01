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
    Head();

    void sinusoidalSearch(const uint16_t degree_increment, const useconds_t delay_steps);

    void moveAt(uint16_t x, uint16_t y);
    void cosinusMove(const uint16_t degree, const int multiplier);

    double getPan() const { return pan; }
    double getTilt() const { return tilt; }

private:
    uint32_t pan, tilt, raw_pan_size, raw_tilt_size;
};

#endif // HEAD_H
