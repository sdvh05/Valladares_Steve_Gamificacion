#ifndef CONTROLPERSONAJE_H
#define CONTROLPERSONAJE_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QKeyEvent>
#include <QRect>
#include <QList>
#include "Personaje.h"

class ControlPersonaje : public QWidget {
    Q_OBJECT

protected:
    Personaje* jugador;
    QTimer* movimientoTimer;
    QList<QRect> obstaculos;
    QList<bool> puertas = {false, false, false, false};

    QList<QLabel*> hitboxLabels;

    // Estado de teclas
    bool shiftPresionado;
    bool izquierdaPresionada;
    bool derechaPresionada;
    bool arribaPresionado;
    bool abajoPresionado;


    // Corazones
    QVector<QLabel*> iconosCorazones;
    int cantidadCorazones =0; // 0 a 4

    virtual void configurarEscena() = 0;
    virtual void configurarObstaculos() = 0;
    virtual void configurarNPCs() {}
    virtual void onMovimientoUpdate() {}

public:
    explicit ControlPersonaje(QWidget* parent = nullptr);
    explicit ControlPersonaje(Personaje* jugadorExistente, QWidget* parent = nullptr);
    virtual ~ControlPersonaje();

    void inicializarJugador();
    void Movimientos();

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void ResetearMovimiento();

    void ActualizarCorazones(bool gano = false);

    void PuertaSeleccionada(int indice);
};

#endif // CONTROLPERSONAJE_H
