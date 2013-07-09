#ifndef CIRCLEDETECTION_H
#define CIRCLEDETECTION_H

#include "stdint-gcc.h"
#include "../std/minIni/minIni.h"
#include <opencv2/imgproc/imgproc.hpp>

class CircleDetection
{
public:
    CircleDetection();

    void loadINISettings(minIni *ini, const std::string &section);
    void saveINISettings(minIni *ini, const std::string &section);
    void setHoughParam(double inverseRatioResolution_of_image, double minDist_of_detectedCircle,
                       double threshCannyEdge, double threshCenterDetection,
                       int radiusMin, int radiusMax);

    void threshold(cv::Mat *InputOutputImage);
    int houghTransform(cv::Mat *InputOutputImage);

    double getX() const { return x; }
    double getY() const { return y; }
    double getRadius() const { return radius; }

private:
    std::vector<cv::Vec3f> circles;
    double inverseRatioResolution_of_image, minDist_of_detectedCircle, threshCannyEdge, threshCenterDetection;
    int radiusMin, radiusMax;
    double x, y, radius;
};

#endif // CIRCLEDETECTION_H
