#ifndef MINIJUEGOPOLITICA_H
#define MINIJUEGOPOLITICA_H

#include "ControlPersonaje.h"
#include "ColaPreguntas.h"
#include "Pregunta.h"

#include <QLabel>
#include <QRect>
#include <QTimer>

class MinijuegoPolitica : public ControlPersonaje {
    Q_OBJECT

private:
    QLabel* labelPuerta;
    QLabel* fondoLabel;
    QLabel* labelPregunta;
    QLabel* labelRespuestas;

    bool hayPuertaCerca = false;
    bool termino = false;
    bool zonaTermino = false;
    bool ganaste = true;
    bool respuestasActivas = false;
    bool hit = false;

    QRect zonaPuerta;
    QRect zonaA, zonaB, zonaC, zonaD;
    bool cercaA = false, cercaB = false, cercaC = false, cercaD = false;

    int MoleActual = 0;       // 0 = ninguno, 1–4 = posiciones A–D
    int MoleAnterior = 0;

    QTimer* timerMole;        // Timer para cambiar topo
    ColaPreguntas Preguntas;
    Pregunta preguntaActual;

    void configurarEscena() override;
    void configurarObstaculos() override;
    void cambiarEscena();
    void mostrarHit();
    void actualizarRespuestas();
    void mostrarResultadoTemporal(bool reiniciar);
    void detectarZonaSeleccion();

    void detectarZonaPuerta();
    void mostrarHintPuerta(const QRect& zonaPuerta);
    void ocultarHintPuerta();

    void seleccionarLetra(int letra);

    void iniciarTimerMole();
    void cambiarMoleAleatorio();

    void SalirMinijuego();

protected:
    void onMovimientoUpdate() override;

public:
    explicit MinijuegoPolitica(Personaje* jugadorExistente, QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
};

#endif // MINIJUEGOPOLITICA_H
