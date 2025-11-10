#ifndef INTERIOR_H
#define INTERIOR_H

#include "ControlPersonaje.h"
#include "Ruleta.h"
#include "npc.h"

#include <QLabel>
#include <QList>
#include <QPushButton>

class Interior : public ControlPersonaje {
    Q_OBJECT

private:
    QLabel* labelPuerta;
    bool hayPuertaCerca = false;
    QLabel* fondoLabel;
    QRect zonaPuerta;


    void detectarZonaPuerta();
    void SalirCastillo();
    void EntrarMinijuego();
    void mostrarHintPuerta(const QRect& zonaPuerta);
    void ocultarHintPuerta();

    void configurarEscena() override;
    void configurarObstaculos() override;

    RuletaWidget* ruleta = nullptr;

    QMap<int, Npc*> npcs; // mapa pasillo -> npc
    void crearNpcs();
    void actualizarNpc();
    void mostrarPreguntaNPC();

protected:
    void onMovimientoUpdate() override;

public:
    explicit Interior(Personaje* jugadorExistente, QWidget* parent = nullptr, int PasilloActual=1);

    int pasilloActual;
    void actualizarPasilloRuleta(int indice);
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
};

#endif // Interior_H
