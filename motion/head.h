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
#define PAN_CONV    false
#define TILT_CONV   true

class Head
{
public:
    static Head* GetInstance() { return m_UniqueInstance; }

    void targetLock(uint16_t frame_x, uint16_t frame_y);
    void sinusoidalSearch(const uint16_t degree_increment, const useconds_t delay_steps);

    void sweepLeft(const uint16_t panAngle_offset, const uint16_t tiltAngle_increment, const useconds_t delay_step);
    void sweepRight(const uint16_t panAngle_offset, const uint16_t tiltAngle_increment, const useconds_t delay_step);

    void panning(const uint16_t angle);
    void tilting(const uint16_t angle);
    void moveAtAngle(const uint16_t tiltAngle, const uint16_t panAngle);
    void cosinusMove(const uint16_t degree, const int multiplier);

    void setPanningPID(const double P_gain, const double D_gain) { pan_P_gain = P_gain; pan_D_gain = D_gain; }
    void setTiltingPID(const int P_gain, const int D_gain) { tilt_P_gain = P_gain; tilt_D_gain = D_gain; }
    double getPanAngle() const { return (pan-HEAD_PAN_MIN)*360/raw_pan_size; }
    double getTiltAngle() const { return (tilt-HEAD_TILT_MIN)*360/raw_tilt_size; }

private:
    static Head* m_UniqueInstance;
    uint32_t pan, tilt;
    uint32_t raw_pan_size, raw_tilt_size;

    // PID variable
    uint16_t centerFrame_x, centerFrame_y;
    double pan_P_gain, pan_D_gain, tilt_P_gain, tilt_D_gain;
    int pan_err, pan_lastErr, tilt_err, tilt_lastErr;
    int P_pan, D_pan, P_tilt, D_tilt;

    uint32_t degree2raw(const uint16_t degree, bool type);
};

#endif // HEAD_H
