#ifndef FRONTVIEW_H
#define FRONTVIEW_H

#include "ControlPersonaje.h"
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "npc.h"

class FrontView : public ControlPersonaje {
    Q_OBJECT

private:
    QLabel* labelPuerta;
    bool hayPuertaCerca = false;

    void detectarZonaPuerta();
    void EntrarCastillo();
    void mostrarHintPuerta();
    void ocultarHintPuerta();
    void configurarEscena() override;
    void configurarObstaculos() override;

    void crearVistaDebug();
    void dibujarHitboxesEnScene();
    void mostrarHitboxes();

    Npc* npcPrueba;



protected:
    void onMovimientoUpdate() override;

public:
    explicit FrontView(Personaje* jugadorExistente, QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mostrarPreguntaVideo();
};

#endif // FRONTVIEW_H
