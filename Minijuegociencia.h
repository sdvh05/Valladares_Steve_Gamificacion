#ifndef MINIJUEGOCIENCIA_H
#define MINIJUEGOCIENCIA_H

#include "ControlPersonaje.h"
#include "ColaPreguntas.h"

#include <QLabel>
#include <QRect>
#include <QList>
#include <QQueue>
#include <QTimer>

//Simon

class MinijuegoCiencia : public ControlPersonaje {
    Q_OBJECT

private:
    QLabel* labelPuerta;
    bool hayPuertaCerca = false;
    QLabel* fondoLabel;
    QRect zonaPuerta;

    QList<bool> puentes = {true, true, true, true}; // A, B, C, D

    bool ganaste=true;
    bool respuestasActivas=false;

    QRect zonaA;
    QRect zonaB;
    QRect zonaC;
    QRect zonaD;

    bool cercaA = false;
    bool cercaB = false;
    bool cercaC = false;
    bool cercaD = false;

    QString letraParaIndice(int i) const;
    void seleccionarLetra(int letra);
    void detectarZonaSeleccion();
    void mostrarHintSeleccion(int indice);
    void ocultarHintSeleccion();

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

    QQueue<char> patronCorrecto;
    QQueue<char> patronJugador;

    void cargarPreguntaActual();
    void calcularEstadoDesdeFrascos();
    void actualizarRespuestas(int nuevaRespuesta);

    void cambiarEscena(int valor);

    void reproducirPatron();
    void ConfirmarRespuestas();

    QTimer* timerPatron;
    int patronIndex;
    bool reproduciendoPatron;
    QVector<int> playbackList;

    bool termino=false;
    int erroress=0;
    int puntos=150;

private slots:
    void reproducirSiguientePasoPatron();

protected:
    void onMovimientoUpdate() override;

public:
    explicit MinijuegoCiencia(Personaje* jugadorExistente, QWidget* parent = nullptr, int Actual=1);

    int EstadoActual;
    int NumPregunta;
    void actualizarEstado(int indice);
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
};

#endif // MINIJUEGOCIENCIA_H
