
QT += widgets

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(QtAwesome/QtAwesome.pri)

SOURCES += \
        main.cpp \
    mainwindow.cpp \
    framewindow.cpp \
    controlboard.cpp \
    imagelabel.cpp

HEADERS += \
    mainwindow.h \
    framewindow.h \
    controlboard.h \
    imagelabel.h

RESOURCES +=

INCLUDEPATH += /usr/local/opencv3/include/
LIBS += -L/usr/local/opencv3/lib/ \
-lopencv_core \
-lopencv_imgcodecs \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_videoio
