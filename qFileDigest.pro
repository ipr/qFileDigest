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
    DigestFile.cpp \
    FiletimeHelper.cpp \
    DigestList.cpp \
    BufferedFile.cpp \
    MemoryMappedFile.cpp \
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
    DigestFile.h \
    FiletimeHelper.h \
    DigestList.h \
    BufferedFile.h \
    MemoryMappedFile.h \
    detailsdialog.h \
    PowerPacker.h \
    AnsiFile.h \
    ImploderExploder.h

FORMS    += mainwindow.ui \
    detailsdialog.ui

INCLUDEPATH += ../qLhA ../qLZXLib
LIBS += -lqLhA -lqLZXLib

CONFIG (debug, debug|release) 
{
    LIBS += -L../qLhA-build-desktop/debug
    LIBS += -L../qLZXLib-build-desktop/debug
} 
CONFIG (release, debug|release) 
{
    LIBS += -L../qLhA-build-desktop/release
    LIBS += -L../qLZXLib-build-desktop/release
}


