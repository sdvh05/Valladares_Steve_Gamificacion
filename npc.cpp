#include "Npc.h"
#include <inventario.h>
#include <QDebug>

extern Inventario* inventarioGlobal;

Npc::Npc(QWidget* parent)
    : Personaje(parent),
    rangoInteraccion(100),
    enDialogo(false)
{
    crearLabelDialogo();
    crearLabelBubble();

    // Animación inicial (idle)
    auto datos = obtenerAnimacion("idle");
    SetAnimacion(datos.ruta, datos.frames);
}

// -----------------------------
// Crear etiquetas visuales
// -----------------------------
void Npc::crearLabelDialogo()
{
    labelDialogo = new QLabel(parentWidget());
    labelDialogo->setStyleSheet(
        "background: rgba(0,0,0,200); color: white; "
        "border: 2px solid #c9a44c; border-radius: 8px; "
        "font: bold 14px 'Courier New'; padding: 6px;"
        );
    labelDialogo->setAlignment(Qt::AlignCenter);
    labelDialogo->hide();
}

void Npc::crearLabelBubble()
{
    labelBubble = new QLabel(parentWidget());
    labelBubble->setStyleSheet(
        "background: rgba(30, 30, 30, 200); color: white; "
        "border-radius: 10px; border: 2px solid #a18c5f;"
        "font: bold 13px 'Courier New'; padding: 4px;"
        );
    labelBubble->setAlignment(Qt::AlignCenter);
    labelBubble->hide();
}

// -----------------------------
// Comprobar distancia
// -----------------------------
bool Npc::estaCercaDelJugador(Personaje* jugador)
{
    if (!jugador) return false;

    QRect zonaNpc = this->geometry();
    QRect zonaJugador = jugador->geometry();

    int distancia = std::abs(zonaNpc.center().x() - zonaJugador.center().x()) +
                    std::abs(zonaNpc.center().y() - zonaJugador.center().y());

    return distancia < rangoInteraccion;
}

// -----------------------------
// Interactuar
// -----------------------------
void Npc::Interactuar(Personaje* jugador)
{
    if (!estaCercaDelJugador(jugador)) {
        qDebug() << "Demasiado lejos para interactuar con" << nombre;
        return;
    }

    if (dialogos.isEmpty()) {
        qDebug() << nombre << "no tiene diálogos configurados.";
        return;
    }

    MostrarDialogo(dialogos);
}

// -----------------------------
// Mostrar diálogo principal
// -----------------------------
void Npc::MostrarDialogo(const QStringList& lineasDialogo)
{
    if (enDialogo) return;
    enDialogo = true;

    int index = 0;
    labelDialogo->setText(lineasDialogo[index]);
    labelDialogo->adjustSize();
    labelDialogo->move(x() - labelDialogo->width()/2 + width()/2, y() - 70);
    labelDialogo->show();

    QTimer* timerDialogo = new QTimer(this);
    connect(timerDialogo, &QTimer::timeout, this, [=]() mutable {
        index++;
        if (index < lineasDialogo.size()) {
            labelDialogo->setText(lineasDialogo[index]);
            labelDialogo->adjustSize();
            labelDialogo->move(x() - labelDialogo->width()/2 + width()/2, y() - 70);
        } else {
            labelDialogo->hide();
            enDialogo = false;
            timerDialogo->stop();
            timerDialogo->deleteLater();

            // 🟢 Señal cuando termina el diálogo
            emit dialogoTerminado();
        }
    });

    timerDialogo->start(2500); // 2.5 segundos entre frases
}


// -----------------------------
// Mostrar burbuja de texto (combate)
// -----------------------------
void Npc::MostrarBubbleText(const QString& texto)
{
    if (!labelBubble) return;

    // Estilo visual del texto (más grande y legible)
    QFont font("Courier New", 18, QFont::Bold);  // tamaño 18, negrita
    labelBubble->setFont(font);

    labelBubble->setStyleSheet(
        "background-color: rgba(0, 0, 0, 180);"
        "color: white;"
        "padding: 8px 12px;"
        "border: 2px solid white;"
        "border-radius: 10px;"
        "font-weight: bold;"
        "text-align: center;"
        );

    labelBubble->setText(texto);
    labelBubble->adjustSize();
    labelBubble->move(x() + width()/2 - labelBubble->width()/2, y() - 50);
    labelBubble->show();
    labelBubble->raise();

    // Efecto de aparición/desaparición
    QGraphicsOpacityEffect* efecto = new QGraphicsOpacityEffect(labelBubble);
    labelBubble->setGraphicsEffect(efecto);

    QPropertyAnimation* anim = new QPropertyAnimation(efecto, "opacity");
    anim->setDuration(2500);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);

    anim->setParent(labelBubble);
    efecto->setParent(labelBubble);

    connect(anim, &QPropertyAnimation::finished, labelBubble, [this]() {
        if (labelBubble) labelBubble->hide();
        labelBubble->setGraphicsEffect(nullptr);
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}


// -----------------------------
// Placeholder: DarObjeto
// -----------------------------
void Npc::DarObjeto(const QString& nombreObjeto)
{
    qDebug() << nombre << "te entrega el objeto:" << nombreObjeto;

    if (nombreObjeto == "llave")
        inventarioGlobal->agregarLlave();
}

Personaje::DatosAnimacion Npc::obtenerAnimacion(const QString& tipo)
{
    DatosAnimacion datos;

    QString base = "Sprites/Personajes/Prota/";

    // Detectar sprite base según el nombre del NPC
    if (nombre.contains("Guardia", Qt::CaseInsensitive)) //gangster blanco
        base = "Sprites/Personajes/NPCs/Gangster/G2/";

    else if (nombre.contains("Mercader", Qt::CaseInsensitive))
        base = "Sprites/Personajes/NPCs/Mercader/";

    else if (nombre.contains("Aldeano", Qt::CaseInsensitive))
        base = "Sprites/Personajes/NPCs/Aldeano/";

    else if (nombre.contains("Rey", Qt::CaseInsensitive))
        base = "Sprites/Personajes/NPCs/Rey/";

    else if (nombre.contains("Sabio", Qt::CaseInsensitive))
        base = "Sprites/Personajes/NPCs/Sabio/";

    else if(nombre.contains("Empirista", Qt::CaseInsensitive))
        base = "Sprites/Personajes/NPCs/Rival/Emp/";

    else if(nombre.contains("Racionalista", Qt::CaseInsensitive))
        base = "Sprites/Personajes/NPCs/Rival/Rac/";

    else if(nombre.contains("Kant", Qt::CaseInsensitive))
        base = "Sprites/Personajes/NPCs/Kant/";

    else if(nombre.contains("Descartes", Qt::CaseInsensitive))
        base = "Sprites/Personajes/NPCs/Descartes/";


    // Tipos de animaciones comunes
    if (tipo == "idle")      datos = { base + "Idle.png", 6 };
    else if (tipo == "walk") datos = { base + "Walk.png", 8 };
    else if (tipo == "run")  datos = { base + "Run.png", 10 };
    else if (tipo == "talk") datos = { base + "Talk.png", 5 };
    else if (tipo == "hurt") datos = { base + "Hurt.png", 3};
    else if (tipo == "dead") datos = { base + "Dead.png", 4 };
    else                     datos = { base + "Idle.png", 6 };

    return datos;
}

