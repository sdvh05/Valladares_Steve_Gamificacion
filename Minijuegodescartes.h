#ifndef MINIJUEGODESCARTES_H
#define MINIJUEGODESCARTES_H

#include "ControlPersonaje.h"
#include "ColaPreguntas.h"
#include "npc.h"

#include <QLabel>
#include <QRect>
#include <QList>

//Descartes -> Lab


class MinijuegoDescartes : public ControlPersonaje {
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
    void actualizarDialogoDescartes();
    void MostrarPistas(int Estado);

    void responderAlDilema();

    Npc* npcDescartes;

    void VerificarRespuesta(char letra);
    void ActualizarPregunta();





protected:
    void onMovimientoUpdate() override;

public:
    explicit MinijuegoDescartes (Personaje* jugadorExistente, QWidget* parent = nullptr, int Actual=0);

    int EstadoActual;
    int NumPregunta;
    void actualizarEstado(int indice);
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
};




#endif // MINIJUEGODESCARTES_H
