#ifndef COMBATE_H
#define COMBATE_H

#include "ControlPersonaje.h"
#include "ColaPreguntas.h"
#include "Npc.h"


#include <QLabel>
#include <QRect>
#include <QList>
#include <QTimer>
#include <QPushButton>
#include <QRandomGenerator>
#include <QMediaPlayer>
#include <QAudioOutput>


class Combate : public ControlPersonaje {
    Q_OBJECT

private:
    Npc* npc;
    void npcMostrarRespuesta(char letra);

    QLabel* fondoLabel;
    QLabel* labelPregunta;
    QLabel* labelRespuestas;

    QLabel* bannerEmpirista;
    QLabel* bannerRacionalista;

    QPushButton* A;
    QPushButton* B;
    QPushButton* C;
    QPushButton* D;

    QLabel* labelPuerta;
    bool hayPuertaCerca = false;
    bool termino = false;
    bool zonaTermino = false;
    bool ultimaPregunta = false;
    bool respuestasActivas = false;
    bool jugadorRespondio = false;
    bool enemigoRespondio = false;

    QList<bool> AnsError = {true, true, true, true}; // Colores de respuestas (A-D)

    ColaPreguntas preguntas;
    Pregunta preguntaActual;

    // Corazones enemigos
    QVector<QLabel*> iconosCorazonesEnemigo;
    int CorazonesEnemigo = 4;

    // Control de combate
    bool ganaste = true;
    int Bando;
    int NumPregunta = 0;
    int corazonesPrevios;

    // "IA"
    QTimer* timerRespuesta;
    void EnemigoResponder();

    // Funciones
    void configurarEscena() override;
    void configurarObstaculos() override;
    void cambiarEscena();
    void cargarPreguntaActual();
    void actualizarRespuestas();
    void responder(char letra);
    void verificarVictoria();
    void mostrarResultadoTemporal(bool reiniciar, bool aciertoJugador, int Quien);
    void iniciarTurno();

    void detectarZonaPuerta();
    void mostrarHintPuerta(const QRect& zonaPuerta);
    void ocultarHintPuerta();
    void SalirMinijuego();

    void ActualizarCorazonesEnemigos();
    void AnimarAtaque(bool jugadorAtaca, std::function<void()> onFinalizado);


    //Post Battalla
    void ReproducirCancion(bool gano);
    void MostrarVictoria(bool gano);
    void FadeOutMusica(int duracion);

    QMediaPlayer* playerMusica = nullptr;
    QAudioOutput* audioOutput = nullptr;


protected:
    void onMovimientoUpdate() override;

public:
    explicit Combate(Personaje* jugadorExistente, QWidget* parent = nullptr, int Bando = 1);
    //~Combate();
    void keyPressEvent(QKeyEvent* event) override;
};

#endif // COMBATE_H

