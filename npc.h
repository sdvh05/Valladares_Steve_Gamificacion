#ifndef NPC_H
#define NPC_H

#include "Personaje.h"
#include <QLabel>
#include <QTimer>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class Npc : public Personaje
{
    Q_OBJECT

public:
    explicit Npc(QWidget* parent = nullptr);

    // --- Interacciones ---
    void Interactuar(Personaje* jugador);
    void MostrarDialogo(const QStringList& lineasDialogo);
    void MostrarBubbleText(const QString& texto);
    void DarObjeto(const QString& nombreObjeto); // Placeholder

    // --- Configuración ---
    void setNombre(const QString& nombreNpc) { nombre = nombreNpc; }
    QString getNombre() const { return nombre; }

    void setDialogos(const QStringList& lista) { dialogos = lista; }

    void setRangoInteraccion(int rango) { rangoInteraccion = rango; }

    DatosAnimacion obtenerAnimacion(const QString& tipo);

private:
    QString nombre;
    QStringList dialogos;
    int rangoInteraccion;
    QLabel* labelDialogo;      // Label para MostrarDialogo
    QLabel* labelBubble;       // Label para MostrarBubbleText
    bool enDialogo;            // Controla si está mostrando diálogo actualmente

    void crearLabelDialogo();
    void crearLabelBubble();
    bool estaCercaDelJugador(Personaje* jugador);

protected:

signals:
    void dialogoTerminado();


};

#endif // NPC_H
