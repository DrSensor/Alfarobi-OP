#include "circledetection.h"

CircleDetection::CircleDetection()
{
}

void CircleDetection::loadINISettings(minIni *ini, const std::string &section)
{
    inverseRatioResolution_of_image = ini->getd(section, "inverse_ratio_resolution_of_image_(dp)");
    minDist_of_detectedCircle = ini->getd(section, "minimum_distribution_of_detected_circle");
    threshCannyEdge = ini->getd(section, "threshold_parameter_CannyEdge");
    threshCenterDetection = ini->getd(section, "threshold_parameter_CenterDetection");
    radiusMin = ini->geti(section, "min_radius");
    radiusMax = ini->geti(section, "max_radius");
}

void CircleDetection::saveINISettings(minIni *ini, const std::string &section)
{
    ini->put(section, "inverse_ratio_resolution_of_image_(dp)", inverseRatioResolution_of_image);
    ini->put(section, "minimum_distribution_of_detected_circle", minDist_of_detectedCircle);
    ini->put(section, "threshold_parameter_CannyEdge", threshCannyEdge);
    ini->put(section, "threshold_parameter_CenterDetection", threshCenterDetection);
    ini->put(section, "min_radius", radiusMin);
    ini->put(section, "max_radius", radiusMax);
}

void CircleDetection::setHoughParam(double inverseRatioResolution_of_image, double minDist_of_detectedCircle, double threshCannyEdge, double threshCenterDetection, int radiusMin, int radiusMax)
{
    this->inverseRatioResolution_of_image = inverseRatioResolution_of_image;
    this->minDist_of_detectedCircle = minDist_of_detectedCircle;
    this->threshCannyEdge = threshCannyEdge;
    this->threshCenterDetection = threshCenterDetection;
    this->radiusMin = radiusMin;
    this->radiusMax = radiusMax;
}

void CircleDetection::threshold(cv::Mat *InputOutputImage)
{
    cv::cvtColor(*InputOutputImage, *InputOutputImage, CV_BGR2GRAY);
    cv::equalizeHist(*InputOutputImage, *InputOutputImage);
//    cv::threshold(*InputOutputImage, *InputOutputImage, minval, maxval, CV_THRESH_OTSU);
}

int CircleDetection::houghTransform(cv::Mat *InputOutputImage)
{
    cv::HoughCircles(*InputOutputImage, circles, CV_HOUGH_GRADIENT, inverseRatioResolution_of_image, minDist_of_detectedCircle, threshCannyEdge, threshCenterDetection, radiusMin, radiusMax);
    for (uint i = 0; i < circles.size(); ++i) {
        if(circles[i][2] > radius) {
            radius = circles[i][2];
            x = circles[i][0];
            y = circles[i][1];
        }
    }
    return circles.size();
}

