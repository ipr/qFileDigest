#-------------------------------------------------
#
# Project created by QtCreator 2011-03-06T19:44:23
#
#-------------------------------------------------

QT       += core gui

TARGET = qFileDigest
TEMPLATE = app

DEFINES += UNICODE _UNICODE
DEFINES += _CRT_SECURE_NO_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
    sha1.cpp \
    MD5.cpp \
    IMD5.cpp \
    FileType.cpp \
    FileListHandler.cpp \
    FiletimeHelper.cpp \
    DigestList.cpp \
    detailsdialog.cpp \
    PowerPacker.cpp \
    AnsiFile.cpp \
    ImploderExploder.cpp

HEADERS  += mainwindow.h \
    sha1.h \
    MD5.h \
    IMD5.h \
    FileType.h \
    FileListHandler.h \
    FiletimeHelper.h \
    DigestList.h \
    detailsdialog.h \
    PowerPacker.h \
    AnsiFile.h \
    ImploderExploder.h

FORMS    += mainwindow.ui \
    detailsdialog.ui

# TODO: replace old libraries&classes with wrapper
# to reduce repeated code and support for more packers&archivers
# INCLUDEPATH += ../qXpkLib
# LIBS += -lqXpkLib

INCLUDEPATH += ../qLhALib ../qLZXLib
LIBS += -lqLhAlib -lqLZXLib

CONFIG (debug, debug|release) 
{
    LIBS += -L../qLhAlib-build-desktop/debug
    LIBS += -L../qLZXLib-build-desktop/debug
} 
CONFIG (release, debug|release) 
{
    LIBS += -L../qLhAlib-build-desktop/release
    LIBS += -L../qLZXLib-build-desktop/release
}
