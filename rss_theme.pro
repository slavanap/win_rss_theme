QT       += core network
QT       -= gui

TARGET = rss_creator

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

HEADERS += rss_creator.h
SOURCES += rss_creator.cpp

win32 {
    LIBS += -L"C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib"
    INCLUDEPATH += .. "C:/Program Files/Microsoft SDKs/Windows/v6.0A/include"
}
