######################################
# Poppy - http REST script launcher  #
# with popup screen capability       #
# triggered by network messages.     #
# Copyright (c) 2025 by Mike Inman   #
# aka MangoCats, all rights reserved #
# Free, Open Source, MIT license     #
#                                    #
######################################

include(httpserver/httpserver.pri)

QT += core gui widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS +=            \
        global.h       \
        requestmapper.h \
        playController.h \
        scriptPlayer.h    \
        splashController.h \
        splash.h

SOURCES +=             \
        global.cpp      \
        main.cpp         \
        requestmapper.cpp \
        playController.cpp \
        scriptPlayer.cpp    \
        splashController.cpp \
        splash.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    poppy.qrc
