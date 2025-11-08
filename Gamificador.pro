QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QT += core gui widgets multimedia

QT += core gui multimedia multimediawidgets
CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ColaPreguntas.cpp \
    Combate.cpp \
    Frontview.cpp \
    Mainmenu.cpp \
    Minijuegoarte.cpp \
    Minijuegociencia.cpp \
    Minijuegodescartes.cpp \
    Minijuegohistoria.cpp \
    Minijuegokant.cpp \
    Minijuegopolitica.cpp \
    Personaje.cpp \
    Ruleta.cpp \
    Videoplayer.cpp \
    controlpersonaje.cpp \
    grafo.cpp \
    grafomapavisual.cpp \
    interior.cpp \
    inventario.cpp \
    inventariowidget.cpp \
    main.cpp \
    mainwindow.cpp \
    npc.cpp

HEADERS += \
    ColaPreguntas.h \
    Combate.h \
    Frontview.h \
    Mainmenu.h \
    Minijuegoarte.h \
    Minijuegociencia.h \
    Minijuegodescartes.h \
    Minijuegohistoria.h \
    Minijuegokant.h \
    Minijuegopolitica.h \
    Personaje.h \
    Pregunta.h \
    Ruleta.h \
    Videoplayer.h \
    controlpersonaje.h \
    grafo.h \
    grafomapavisual.h \
    interior.h \
    inventario.h \
    inventariowidget.h \
    mainwindow.h \
    npc.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
