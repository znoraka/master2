#-------------------------------------------------
#
# Project created by QtCreator 2015-09-29T17:32:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GameEngine
TEMPLATE = app


SOURCES += \
    engine/camera.cpp \
    engine/gamewindow.cpp \
    main.cpp \
    engine/openglwindow.cpp \
    engine/game.cpp \
    engine/scene.cpp \
    engine/entity.cpp \
    engine/components/component.cpp \
    game/trianglecomponent.cpp \
    engine/tools/pool.cpp \
    engine/components/positioncomponent.cpp \
    engine/systems/system.cpp \
    engine/systems/trianglesystem.cpp

HEADERS  += \
    engine/camera.h \
    engine/gamewindow.h \
    engine/openglwindow.h \
    engine/game.h \
    engine/scene.h \
    engine/entity.h \
    engine/components/component.h \
    game/trianglecomponent.h \
    engine/tools/pool.h \
    engine/components/positioncomponent.h \
    engine/systems/system.h \
    engine/systems/trianglesystem.h

FORMS    +=
QMAKE_CXXFLAGS += -std=c++11
