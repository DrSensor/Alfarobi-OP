#include "head.h"

#define SPEED   1
#define HEAD_SECTION    "Motion Head"

Head* Head::m_UniqueInstance = new Head();

Head::Head()
{
    raw_pan_size = HEAD_PAN_MAX - HEAD_PAN_MIN;
    raw_tilt_size = HEAD_TILT_MAX - HEAD_TILT_MIN;
    lastErr = 0;
    raw_est_last = 0;
    P_last = 0;
}

void Head::targetTracking(uint16_t frame_x, uint16_t frame_y, const uint8_t kalman_iteration)
{
    // TODO : use PID Darwin
    /*
     * BallTracker.cpp -> pixel per image
     *
     */
    // PID jimmy
    frame_y = kalmanFilter(frame_y, kalman_iteration, *flag_measure, P_noise.frame_y, K_noise.frame_y);
    pan += PIDControll(frame_y, centerFrame_y, P_gain.pan, D_gain.pan);
    if (pan > HEAD_PAN_MAX) pan = HEAD_PAN_MAX;
    else if (pan < HEAD_PAN_MIN) pan = HEAD_PAN_MIN;
    port_write(pan);

    frame_x = kalmanFilter(frame_x, kalman_iteration, *flag_measure, P_noise.frame_x, K_noise.frame_x);
    tilt += PIDControll(frame_x, centerFrame_x, P_gain.tilt, D_gain.tilt);
    if (tilt > HEAD_TILT_MAX) tilt = HEAD_TILT_MAX;
    else if (tilt < HEAD_TILT_MIN) tilt = HEAD_TILT_MIN;
    port_write(tilt);
}

void Head::sinusoidalSearch(const uint16_t degree_increment, const useconds_t delay_steps)
{
    for (int i = 0; i <= 360; i += degree_increment) {
        if (*flag_INT) break;
        cosinusMove(i, 2);
        usleep(delay_steps);
    }
    for (int i = 360; i >= 0; i -= degree_increment) {
        if (*flag_INT) break;
        cosinusMove(i, -2);
        usleep(delay_steps);
    }
}

void Head::sweepLeft(const uint16_t panAngle_offset, const uint16_t tiltAngle_increment, const useconds_t delay_step)
{
    pan = degree2raw(panAngle_offset, false);
    port_write(pan);
    for (tilt = HEAD_PAN_MIN; tilt < HEAD_PAN_MAX; tilt += degree2raw(tiltAngle_increment, TILT_CONV)) {
        if (*flag_INT) break;
        usleep(delay_step);
        port_write(tilt);
    }
}

void Head::sweepRight(const uint16_t panAngle_offset, const uint16_t tiltAngle_increment, const useconds_t delay_step)
{
    pan = degree2raw(panAngle_offset, false);
    port_write(pan);
    for (tilt = HEAD_TILT_MIN; tilt <= HEAD_TILT_MAX; tilt += degree2raw(tiltAngle_increment, TILT_CONV)) {
        if (*flag_INT) break;
        usleep(delay_step);
        port_write(tilt);
    }
}

void Head::nod(const uint16_t tiltAngle_offset, const uint16_t panAngle_increment, const useconds_t delay_steps)
{
    tilt = degree2raw(tiltAngle_offset, TILT_CONV);
    port_write(tilt);
    for (pan = HEAD_PAN_MAX; pan >= HEAD_PAN_MIN; pan -= degree2raw(panAngle_increment, PAN_CONV)) {
        if (*flag_INT) break;
        usleep(delay_steps);
        port_write(pan);
    }
    for (pan = HEAD_PAN_MIN; pan <= HEAD_PAN_MAX; pan += degree2raw(panAngle_increment, PAN_CONV)) {
        if (*flag_INT) break;
        usleep(delay_steps);
        port_write(pan);
    }
}

void Head::panning(const uint16_t angle)
{
    pan = degree2raw(angle, PAN_CONV);
    port_write(pan);
}

void Head::tilting(const uint16_t angle)
{
    tilt = degree2raw(angle, TILT_CONV);
    port_write(tilt);
}

void Head::moveAtAngle(const uint16_t tiltAngle, const uint16_t panAngle)
{
    pan = degree2raw(panAngle, PAN_CONV);
    port_write(pan);
    tilt = degree2raw(tiltAngle, TILT_CONV);
    port_write(tilt);
}

void Head::cosinusMove(const uint16_t degree, const int multiplier)
{
    pan = (degree*raw_pan_size/360)+HEAD_PAN_MIN;
    port_write(pan);
    tilt = ((cos(degree*M_PI/180)*multiplier+1)/(abs(multiplier))+1)+(2*HEAD_TILT_MIN);
    port_write(tilt);
}

void Head::loadINI(minIni *ini, const string &section)
{
    P_gain.pan = ini->getd(section, "pan_P_gain");
    D_gain.pan = ini->getd(section, "pan_D_gain");
    P_gain.tilt = ini->getd(section, "tilt_P_gain");
    D_gain.tilt = ini->getd(section, "tilt_D_gain");
}

void Head::saveINI(minIni *ini, const string &section)
{
    ini->put(section, "pan_P_gain", P_gain.pan);
    ini->put(section, "pan_D_gain", D_gain.pan);
    ini->put(section, "tilt_P_gain", P_gain.tilt);
    ini->put(section, "tilt_D_gain", D_gain.tilt);
}

uint32_t Head::degree2raw(const uint16_t degree, bool type)
{
    if (type)     // tilt
        return (degree*raw_tilt_size/360)+HEAD_TILT_MIN;
    else          // pan
        return (degree*raw_pan_size/360)+HEAD_PAN_MIN;
}

static bool temp_once = false;
uint32_t Head::kalmanFilter(uint16_t measured_val, const uint8_t iteration, bool begin_measure, const double Q, const double R)
{
    static double K, P, P_temp;
    static uint16_t raw_temp_est;
    static bool once;

    /* ambigu mau tak pke ga ?
    // 1 0 1
    // do flip-flop for var once
    if (begin_measure && !(temp_once)) {
        once = true;
        temp_once = true;
    } else if (!(begin_measure) && temp_once)
        temp_once = false;

    // initialize once
    if (once) {
        raw_est_last = measured_val;
        once = false;
    }
    */

    if (begin_measure) {
        for (int i = 0; i < iteration; ++i) {
            // do prediction
            raw_temp_est = raw_est_last;
            P_temp = P_last + Q;
            // calculate Kalman gain
            K = P_temp * (1/(P_temp + R));
            // correction
            raw_est = raw_temp_est + K * (measured_val - raw_temp_est);
            P = (1 - K) * P_temp;
            // update our last's
            P_last = P;
            raw_est_last = raw_est;
            // measure error but not use in correction
            error_kalman = raw_est - measured_val;
        }
        return raw_est;
    } else
        return measured_val;
}

int Head::PIDControll(const uint16_t measured_val, const uint16_t ideal_val, double Kp, double Kd, double Ki)
{
    static int P, I, D;

    err = ideal_val - measured_val;
    sumErr += lastErr;
    P = err*Kp;
    D = (err - lastErr)*Kd;
    I = sumErr*Ki;
    lastErr = err;

    return (P+I+D);
}
