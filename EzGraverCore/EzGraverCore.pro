include(../common.pri)

QT += core
QT += serialport

TARGET = EzGraverCore
TEMPLATE = lib

DEFINES += EZGRAVERCORE_LIBRARY

SOURCES += ezgraver.cpp \
    ezgraver_v1.cpp \
    ezgraver_v2.cpp \
    factory.cpp \
    ezgraver_v3.cpp \
    ezgraver_v4.cpp

HEADERS += ezgraver.h\
        ezgravercore_global.h \
    ezgraver_v1.h \
    ezgraver_v2.h \
    specifications.h \
    factory.h \
    ezgraver_v3.h \
    ezgraver_v4.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
