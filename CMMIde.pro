#-------------------------------------------------
#
# Project created by QtCreator 2016-12-30T11:00:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CMMIde
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ErrorHandler.cpp \
    GrammarParser.cpp \
    GrammarTest.cpp \
    IdTable.cpp \
    QuaternaryGenerator.cpp \
    TokenParser.cpp \
    CMMCore.cpp

HEADERS  += mainwindow.h \
    ErrorHandler.h \
    GrammarParser.h \
    IdTable.h \
    QuaternaryGenerator.h \
    TokenParser.h \
    util.h \
    cmmcompiler.h \
    CMMCore.h

FORMS    += mainwindow.ui
