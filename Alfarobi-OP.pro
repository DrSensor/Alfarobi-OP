TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += \
#-lopencv_core -lopencv_highgui -lopencv_imgproc
-lserial -lpthread

SOURCES += main.cpp \
    imgproc/momentdetection.cpp \
    imgproc/circledetection.cpp \
    motion/head.cpp \
    motion/action.cpp

OTHER_FILES += \
    imgproc/README.md \
    motion/README.md \
    std/README.md \
    README.md

HEADERS += \
    imgproc/momentdetection.h \
    imgproc/circledetection.h \
    motion/head.h \
    motion/action.h \
    motion/subcontroller.h \
    motion/data_subcontroller.h

