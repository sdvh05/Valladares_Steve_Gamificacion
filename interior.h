#ifndef INTERIOR_H
#define INTERIOR_H

#include "ControlPersonaje.h"
#include <QLabel>
#include <QList>
#include <QPushButton>

class Interior : public ControlPersonaje {
    Q_OBJECT

private:
    QLabel* labelPuerta;
    bool hayPuertaCerca = false;

    void detectarZonaPuerta();
    void SalirCastillo();
    void mostrarHintPuerta();
    void ocultarHintPuerta();

    void configurarEscena() override;
    void configurarObstaculos() override;

protected:
    void onMovimientoUpdate() override;

public:
    explicit Interior(Personaje* jugadorExistente, QWidget* parent = nullptr, int PasilloActual=1);
    int pasilloActual;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
};

#endif // Interior_H
