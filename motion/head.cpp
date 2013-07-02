#include "head.h"

Head* Head::m_UniqueInstance = new Head();

Head::Head()
{
    raw_pan_size = HEAD_PAN_MAX - HEAD_PAN_MIN;
    raw_tilt_size = HEAD_TILT_MAX - HEAD_TILT_MIN;
}

void Head::targetLock(uint16_t frame_x, uint16_t frame_y)
{
    // TODO : use PID Darwin
}

void Head::sinusoidalSearch(const uint16_t degree_increment, const useconds_t delay_steps)
{
    for (int i = 0; i <= 360; i += degree_increment) {
        cosinusMove(i, 2);
        usleep(delay_steps);
    }
    for (int i = 360; i >= 0; i -= degree_increment) {
        cosinusMove(i, -2);
        usleep(delay_steps);
    }
}

void Head::sweepLeft(const uint16_t panAngle_offset, const uint16_t tiltAngle_increment, const useconds_t delay_step)
{
    pan = degree2raw(panAngle_offset, false);
    port_write(pan);
    for (tilt = HEAD_PAN_MIN; tilt < HEAD_PAN_MAX; tilt += degree2raw(tiltAngle_increment, true))
        port_write(tilt);
}

void Head::sweepRight(const uint16_t panAngle_offset, const uint16_t tiltAngle_increment, const useconds_t delay_step)
{
    pan = degree2raw(panAngle_offset, false);
    port_write(pan);
    for (tilt = HEAD_PAN_MIN; tilt < HEAD_PAN_MAX; tilt += degree2raw(tiltAngle_increment, true))
        port_write(tilt);
}

void Head::moveAt(const uint16_t x, const uint16_t y)
{
    pan = x;
    tilt = y;
    port_write(pan);
    port_write(tilt);
}

void Head::cosinusMove(const uint16_t degree, const int multiplier)
{
    pan = (degree*raw_pan_size/360)+HEAD_PAN_MIN;
    port_write(pan);
    tilt = ((cos(degree*M_PI/180)*multiplier)/abs(multiplier))+(2*HEAD_TILT_MIN);
    port_write(tilt);
}

uint32_t Head::degree2raw(const uint16_t degree, bool type)
{
    if (type)     // tilt
        return (degree*raw_tilt_size/360)+HEAD_TILT_MIN;
    else          // pan
        return (degree*raw_pan_size/360)+HEAD_PAN_MIN;
}
