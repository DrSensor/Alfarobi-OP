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

    void sinusoidalSearch(const uint16_t degree_increment, const useconds_t delay_steps);

    void moveAt(uint16_t x, uint16_t y);
    void cosinusMove(const uint16_t degree, const int multiplier);

    double getPanAngle() const { return pan; }
    double getTiltAngle() const { return tilt; }

private:
    static Head* m_UniqueInstance;
    uint32_t pan, tilt, raw_pan_size, raw_tilt_size;
};

#endif // HEAD_H
