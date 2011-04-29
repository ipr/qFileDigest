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
    TosecName.cpp \
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
    UnLzx.cpp \
    AnsiFile.cpp

HEADERS  += mainwindow.h \
    TosecName.h \
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
    UnLzx.h \
    AnsiFile.h

FORMS    += mainwindow.ui \
    detailsdialog.ui
