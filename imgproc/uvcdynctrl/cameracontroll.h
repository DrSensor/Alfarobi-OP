#ifndef CAMERACONTROLL_H
#define CAMERACONTROLL_H

#include <unistd.h>
#include <stdlib.h>
#include <sstream>

#define to_string(x) static_cast<std::ostringstream*>( &(std::ostringstream() << x) )->str()

class CameraControll
{
public:
    CameraControll(const int device);

    void set(const char *ctrl_arg, const int value);
    void save(const char *filename);
    void load(const char *filename);

private:
    std::string uvcdynctrl;
};

#endif // CAMERACONTROLL_H
