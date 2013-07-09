#include "momentdetection.h"

cv::Mat rgb, hsv;

MomentDetection::MomentDetection()
{
    hsvLutFilled = false;
    rgb.create(cv::Size(256,256), CV_8UC3);
    hsv.create(cv::Size(256,256), CV_8UC3);
}

void MomentDetection::loadINISettings(minIni *ini, const std::string &section)
{
    setHSVmin(ini->geti(section, "hue_min"), ini->geti(section, "saturation_min"), ini->geti(section, "value_min"));
    setHSVmax(ini->geti(section, "hue_max"), ini->geti(section, "saturation_max"), ini->geti(section, "value_max"));
}

void MomentDetection::saveINISettings(minIni *ini, const std::string &section)
{
    ini->put(section, "hue_min", HSVColor.H.min);
    ini->put(section, "hue_max", HSVColor.H.max);
    ini->put(section, "saturation_min", HSVColor.S.min);
    ini->put(section, "saturation_max", HSVColor.S.max);
    ini->put(section, "value_min", HSVColor.V.min);
    ini->put(section, "value_max", HSVColor.V.max);
}

void MomentDetection::setHSVmin(const uint8_t H, const uint8_t S, const uint8_t V)
{
    HSVColor.H.min = H;
    HSVColor.S.min = S;
    HSVColor.V.min = V;
}

void MomentDetection::setHSVmax(const uint8_t H, const uint8_t S, const uint8_t V)
{
    HSVColor.H.max = H;
    HSVColor.S.max = S;
    HSVColor.V.max = V;
}

void MomentDetection::generateLUT_HSV()
{
    // red & green fill
    for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < 256; ++j) {
            rgb.ptr<cv::Vec3b>(i)[j][2] = (uchar) i;
            rgb.ptr<cv::Vec3b>(i)[j][1] = (uchar) j;
        }
    }

    for (int k = 0; k < 256; ++k) {
        // blue color loop
        for (int i = 0; i < 256; ++i) {
            for (int j = 0; j < 256; ++j) {
                rgb.ptr<cv::Vec3b>(i)[j][0] = (uchar) k;
            }
        }
        cvtColor(rgb, hsv, CV_BGR2HSV);

        for (int i = 0; i < 256; ++i) {
            for (int j = 0; j < 256; ++j) {
                idx = (k*256+j)*256 + i;

                hsvLut[ idx ].h = hsv.ptr<cv::Vec3b>(i)[j][0];
                hsvLut[ idx ].s = hsv.ptr<cv::Vec3b>(i)[j][1];
                hsvLut[ idx ].v = hsv.ptr<cv::Vec3b>(i)[j][2];
            }
        }
    }
    hsvLutFilled = true;
}

void MomentDetection::threshold(cv::Mat *InputOutputImage, const bool inverse_binary)
{
    cv::equalizeHist(*InputOutputImage, *InputOutputImage);
    if (hsvLutFilled) {
        fastBGR2HSVfilter(*InputOutputImage,
                          cv::Scalar(HSVColor.H.min, HSVColor.S.min, HSVColor.V.min),
                          cv::Scalar(HSVColor.H.max, HSVColor.S.max, HSVColor.V.max),
                          InputOutputImage, inverse_binary);
    } else {
        cv::cvtColor(*InputOutputImage, *InputOutputImage, CV_BGR2HSV);
        cv::inRange(*InputOutputImage,
                    cv::Scalar(HSVColor.H.min, HSVColor.S.min, HSVColor.V.min),
                    cv::Scalar(HSVColor.H.max, HSVColor.S.max, HSVColor.V.max),
                    *InputOutputImage);
        if (inverse_binary) cv::threshold(*InputOutputImage, *InputOutputImage, 0, 255, CV_THRESH_BINARY_INV);
    }
}

void MomentDetection::moment(cv::Mat *InputOutputImage)
{
    moments = cv::moments(*InputOutputImage);
    x = moments.m10 / moments.m00;
    y = moments.m01 / moments.m00;
    area = moments.m00;
}

void MomentDetection::fastBGR2HSVfilter(const cv::Mat InputImage,
                                        const cv::Scalar min, const cv::Scalar max, cv::Mat *OutputImage,
                                        const bool inverse)
{
    OutputImage->create(InputImage.rows, InputImage.cols, cv::DataType<bool>::type);
    for (int i = 0; i < InputImage.rows; ++i) {
        for (int j = 0; j < InputImage.cols; ++j) {
            idx = InputImage.ptr<cv::Vec3b>(i)[j][0]*65536 + InputImage.ptr<cv::Vec3b>(i)[j][1]*256 + InputImage.ptr<cv::Vec3b>(i)[j][2];

            if (between(min[0], hsvLut[idx].h, max[0])
                    && between(min[1], hsvLut[idx].s, max[1])
                    && between(min[2], hsvLut[idx].v, max[2]))
                if (inverse) OutputImage->ptr(i)[j] = 0;
                else OutputImage->ptr(i)[j] = 0xff;
            else
                if (inverse) OutputImage->ptr(i)[j] = 0xff;
                else OutputImage->ptr(i)[j] = 0;
        }
    }
}
