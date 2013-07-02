#ifndef CUSTOM_MATH_H
#define CUSTOM_MATH_H

namespace {

bool constraintErr(unsigned short var_ukur, unsigned short nilai_pasti, unsigned short ralat, unsigned short max_ukur = 360)
{
    static unsigned short min, max;
    min = nilai_pasti-ralat;
    max = nilai_pasti+ralat;
    if(ralat > nilai_pasti)
        min = ralat-nilai_pasti;
    if(max > max_ukur)
        max = max_ukur-nilai_pasti+ralat;
    if(ralat > nilai_pasti && var_ukur <= max_ukur && var_ukur >= max_ukur-min)
        return true;
    if(ralat > nilai_pasti && var_ukur < min)
        return true;
    if(min<=var_ukur && var_ukur <= max)
        return true;
    return false;
}

}

#endif // CUSTOM_MATH_H
