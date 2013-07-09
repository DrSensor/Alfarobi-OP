#ifndef MOMENTDETECTION_H
#define MOMENTDETECTION_H

#include <stdint-gcc.h>
#include "../std/minIni/minIni.h"
#include <opencv2/imgproc/imgproc.hpp>

#define LUT_SIZE 16777216

#define between(min,var,max)    (min <= var && var <= max)

class MomentDetection
{
public:
    MomentDetection();

    void loadINISettings(minIni *ini, const std::string &section);
    void saveINISettings(minIni *ini, const std::string &section);

    void setHSVmin(const uint8_t H, const uint8_t S, const uint8_t V);
    void setHSVmax(const uint8_t H, const uint8_t S, const uint8_t V);

    void generateLUT_HSV();
    void threshold(cv::Mat *InputOutputImage, const bool inverse_binary = false);
    void moment(cv::Mat *InputOutputImage);

    double getX() const { return x; }
    double getY() const { return y; }
    double getArea() const { return area; }

private:
    cv::Moments moments;
    double x, y, area;

    struct MinMax { uint8_t min, max; };
    struct HSV { struct MinMax H, S, V; } HSVColor;

    bool hsvLutFilled;
    struct HueSatVal { uchar h, s, v; } hsvLut[LUT_SIZE];
    cv::Mat temp;
    uint idx;

    void fastBGR2HSVfilter(const cv::Mat InputImage,
                           const cv::Scalar min, const cv::Scalar max, cv::Mat *OutputImage,
                           const bool inverse = false);
};

#endif // MOMENTDETECTION_H
