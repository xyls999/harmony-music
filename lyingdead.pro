QT += core gui dtkwidget multimedia svg sql network
QT +=multimediawidgets
QT +=concurrent
CONFIG += rtti
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lyingdead
TEMPLATE = app
CONFIG += link_pkgconfig  c++11 opengl
PKGCONFIG += taglib

SOURCES += \
        main.cpp \
    mymainwindow.cpp \
    footwidget.cpp \
    volumepopup.cpp \
    titlebarwidget.cpp \
    mainpage.cpp \
    musicitemwidget.cpp \
    scrolllabel.cpp \
    lyric.cpp \
    clickablelabel.cpp \
    mainwindow.cpp \
    audiomodelclient.cpp \
    aibutton.cpp \
    recommender.cpp \
    databasehandler.cpp \
    playlistitemwidget.cpp \
    thememanager.cpp \
    covercache.cpp \
    videomodelclienttwo.cpp \
    rrecommendwidget.cpp \
    videoplayer.cpp

RESOURCES +=         resources.qrc \
    videoimages.qrc

FORMS += \
    footwidget.ui \
    titlebarwidget.ui \
    mainpage.ui \
    scrolllabel.ui \
    lyric.ui \
    mainwindow.ui \
    rrecommendwidget.ui \
    videoplayer.ui

HEADERS += \
    mymainwindow.h \
    footwidget.h \
    volumepopup.h \
    titlebarwidget.h \
    mainpage.h \
    musicitemwidget.h \
    scrolllabel.h \
    lyric.h \
    clickablelabel.h \
    mainwindow.h \
    audiomodelclient.h \
    aibutton.h \
    recommender.h \
    databasehandler.h \
    playlistitemwidget.h \
    thememanager.h \
    covercache.h \
    videomodelclienttwo.h \
    rrecommendwidget.h \
    videoplayer.h \
    themes.h \
    playlistitem.h

DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -Wno-deprecated-declarations
