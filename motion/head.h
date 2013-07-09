#define HEAD_PAN_MAX    1024
#define HEAD_PAN_MIN    512
#define HEAD_TILT_MAX   1024
#define HEAD_TILT_MIN   512

#ifndef HEAD_H
#define HEAD_H

#include <stdint-gcc.h>
#include <cmath>
#include "../std/minIni/minIni.h"
#include "subcontroller.h"

//#define PI
#define PAN_CONV    false
#define TILT_CONV   true

class Head
{
public:
    static Head* GetInstance() { return m_UniqueInstance; }
    Head();

    // high level
    void targetTracking(uint16_t frame_x, uint16_t frame_y, const uint8_t kalman_iteration = 1);
    void sinusoidalSearch(const uint16_t degree_increment, const useconds_t delay_steps);

    // medium level
    void sweepLeft(const uint16_t panAngle_offset, const uint16_t tiltAngle_increment, const useconds_t delay_step);
    void sweepRight(const uint16_t panAngle_offset, const uint16_t tiltAngle_increment, const useconds_t delay_step);
    void shakking(const uint16_t panAngle_offset, const uint16_t tiltAngle_increment, const useconds_t delay_step);
    void nod(const uint16_t tiltAngle_offset, const uint16_t panAngle_increment, const useconds_t delay_steps);

    // low level
    void panning(const uint16_t angle);
    void tilting(const uint16_t angle);
    void moveAtAngle(const uint16_t tiltAngle, const uint16_t panAngle);
    void cosinusMove(const uint16_t degree, const int multiplier);

    // setting for targetTracking();
    void setPanningPID(const double P_gain, const double D_gain) { this->P_gain.pan = P_gain; this->D_gain.pan = D_gain; }
    void setTiltingPID(const int P_gain, const int D_gain) { this->P_gain.tilt = P_gain; this->D_gain.tilt = D_gain; }
    void setFrameXKalmanNoise(const double Q, const double R) { P_noise.frame_x = Q; K_noise.frame_x = R; }
    void setFrameYKalmanNoise(const double Q, const double R) { P_noise.frame_y = Q; K_noise.frame_y = R; }

    // setting interupt flag for high end medium level (except targetTracking();)
    void setInterruptState(bool *flag) { flag_INT = flag; }

    void loadINI(minIni *ini, const std::string &section);
    void saveINI(minIni *ini, const std::string &section);

    double getPanAngle() const { return (pan-HEAD_PAN_MIN)*360/raw_pan_size; }
    double getTiltAngle() const { return (tilt-HEAD_TILT_MIN)*360/raw_tilt_size; }

private:
    static Head* m_UniqueInstance;
    uint32_t pan, tilt;
    uint32_t raw_pan_size, raw_tilt_size;
    bool *flag_INT;

    uint16_t centerFrame_x, centerFrame_y;

    struct P { double pan, tilt; } P_gain;
    struct D { double pan, tilt; } D_gain;
    // PIDControll() variable
    int err, lastErr, sumErr;

    struct Q { double frame_x, frame_y; } P_noise;
    struct R { double frame_x, frame_y; } K_noise;
    // kalmanFilter() variable
    double P_last;
    uint16_t raw_est, raw_est_last;
    uint16_t error_kalman;
    bool *flag_measure;

    uint32_t degree2raw(const uint16_t degree, bool type);
    uint32_t kalmanFilter(uint16_t measured_val, const uint8_t iteration, bool begin_measure, const double Q, const double R);
    int PIDControll(uint16_t measured_val, const uint16_t ideal_val, double Kp, double Kd, double Ki = 0);
};

#endif // HEAD_H
