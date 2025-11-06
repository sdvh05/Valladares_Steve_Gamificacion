#ifndef MINIJUEGOARTE_H
#define MINIJUEGOARTE_H

#include "ControlPersonaje.h"
#include "ColaPreguntas.h"

#include <QLabel>
#include <QRect>
#include <QList>
#include <QTimer>
#include <QPoint>
#include <QPointer>
#include <QObject>

class MinijuegoArte : public ControlPersonaje {
    Q_OBJECT

public:
    explicit MinijuegoArte(Personaje* jugadorExistente, QWidget* parent = nullptr, int EstadoActual = 1);
    ~MinijuegoArte() override = default;

    void actualizarEstado(int indice);
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

protected:
    void onMovimientoUpdate() override;

private:
    // UI / estado
    QLabel* labelPuerta = nullptr;
    bool hayPuertaCerca = false;
    bool termino = false;
    bool zonaTermino = false;
    bool ultimaPregunta = false;
    QLabel* fondoLabel = nullptr;
    QRect zonaPuerta;

    QList<bool> puentes = { true, true, true, true }; // A, B, C, D

    bool ganaste = true;
    bool respuestasActivas = false;

    // Preguntas
    QLabel* labelPregunta = nullptr;
    QLabel* labelRespuestas = nullptr;
    ColaPreguntas preguntas;
    Pregunta preguntaActual;

    // Zonas de impacto
    QRect ZonaA, ZonaB, ZonaC, ZonaD;

    // Correción / util
    void detectarZonaPuerta();
    void SalirMinijuego();
    void mostrarHintPuerta(const QRect& zonaPuerta);
    void ocultarHintPuerta();

    void configurarEscena() override;
    void configurarObstaculos() override;
    void cambiarEscena();
    void cargarPreguntaActual();
    void actualizarRespuestas();



    // ---------------- Piedra (proyectil) ----------------
    class Piedra : public QObject {
    public:
        explicit Piedra(QWidget* parent = nullptr)
            : QObject(parent),
            label(new QLabel(parent)),
            timer(new QTimer(parent)),
            velocidad(0, -12)
        {
            // Inicialización mínima; propiedades se ajustan en el .cpp
        }

        ~Piedra() override {
            // stop/delete son seguros; label y timer tienen parent y serán eliminados por deleteLater si se usa
            if (timer) {
                timer->stop();
                timer->deleteLater();
                timer = nullptr;
            }
            if (label) {
                label->deleteLater();
                label = nullptr;
            }
        }

        QLabel* label;
        QTimer* timer;
        QPoint velocidad;
    };

    // usamos QPointer para evitar accesos a punteros borrados desde lambdas de timers
    QList<QPointer<Piedra>> piedras;

    // Metodos para piedra
    void lanzarPiedra();
    void moverPiedra(Piedra* piedra);
    void eliminarPiedra(Piedra* piedra);
    void verificarImpacto(Piedra* piedra);

    // resultado/verificacion
    void verificarRespuesta(char letra);

    // estado
public:
    int EstadoActual = 1;
    int NumPregunta = 0;
};

#endif // MINIJUEGOARTE_H
