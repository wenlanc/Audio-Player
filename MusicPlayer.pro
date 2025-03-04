#-------------------------------------------------
#
# Project created by QtCreator 2016-01-31T00:45:25
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia
CONFIG   += c++14
QMAKE_INFO_PLIST= $${PWD}/Info.plist
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicPlayer
TEMPLATE = app


win32:INCLUDEPATH += $$PWD
win32:RC_ICONS += icon.ico
macx:ICON = $${PWD}/icon.icns
SOURCES += main.cpp\
        mainwindow.cpp \
    playlist.cpp \
    track.cpp \
    musicitem.cpp \
    globals.cpp

HEADERS  += mainwindow.h \
    playlist.h \
    track.h \
    utils.h \
    musicitem.h \
    globals.h

FORMS    += mainwindow.ui

DISTFILES +=

RESOURCES += \
    res.qrc
