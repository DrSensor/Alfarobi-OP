#include "cameracontroll.h"

/*!
 * \brief CameraControll::CameraControll
 * \param device
 */
CameraControll::CameraControll(const int device)
{
    uvcdynctrl = "uvcdynctrl -d ";
    uvcdynctrl += "video";
    uvcdynctrl += to_string(device);
}

/*!
 * \brief CameraControll::set
 * \param ctrl_arg
 * \param value
 */
void CameraControll::set(const char *ctrl_arg, const int value)
{
    uvcdynctrl += " -s \"";
    uvcdynctrl += ctrl_arg;
    uvcdynctrl += "\" ";
    uvcdynctrl += to_string(value);

    system(uvcdynctrl.c_str());

    uvcdynctrl.erase(uvcdynctrl.find("-s")-1, uvcdynctrl.size()-uvcdynctrl.find_last_of("-s")+2);
}

/*!
 * \brief CameraControll::save
 * \param filename
 */
void CameraControll::save(const char *filename)
{
    uvcdynctrl += " -S ";
    uvcdynctrl += filename;

    system(uvcdynctrl.c_str());

    uvcdynctrl.erase(uvcdynctrl.find("-S")-1, uvcdynctrl.size()-uvcdynctrl.find_last_of("-S")+2);
}

/*!
 * \brief CameraControll::load
 * \param filename
 */
void CameraControll::load(const char *filename)
{
    uvcdynctrl += " -L ";
    uvcdynctrl += filename;

    system(uvcdynctrl.c_str());

    uvcdynctrl.erase(uvcdynctrl.find("-L")-1, uvcdynctrl.size()-uvcdynctrl.find_last_of("-L")+2);
}
