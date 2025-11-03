#ifndef MINIJUEGOHISTORIA_H
#define MINIJUEGOHISTORIA_H

#include "ControlPersonaje.h"
#include "ColaPreguntas.h"

#include <QLabel>
#include <QRect>
#include <QList>


class MinijuegoHistoria : public ControlPersonaje {
    Q_OBJECT

private:
    QLabel* labelPuerta;
    bool hayPuertaCerca = false;
    bool termino=false;
    bool zonaTermino=false;
    bool ultimaPregunta = false;
    QLabel* fondoLabel;
    QRect zonaPuerta;

     QList<bool> puentes = {true, true, true, true}; // A, B, C, D

    bool ganaste=true;
    bool respuestasActivas=false;

    void detectarZonaPuerta();
    void SalirMinijuego();
    void mostrarHintPuerta(const QRect& zonaPuerta);
    void ocultarHintPuerta();

    void configurarEscena() override;
    void configurarObstaculos() override;

    QLabel* labelPregunta;
    QLabel* labelRespuestas;

    ColaPreguntas preguntas;
    Pregunta preguntaActual;



    void cargarPreguntaActual();
    void cambiarEscena();
    void actualizarRespuestas();





protected:
    void onMovimientoUpdate() override;

public:
    explicit MinijuegoHistoria (Personaje* jugadorExistente, QWidget* parent = nullptr, int Actual=1);

    int EstadoActual;
    int NumPregunta;
    void actualizarEstado(int indice);
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
};

#endif // MINIJUEGOHISTORIA_H
