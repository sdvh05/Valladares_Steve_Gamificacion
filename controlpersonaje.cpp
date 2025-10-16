#include "ControlPersonaje.h"
#include <QPixmap>
#include <QPainter>

ControlPersonaje::ControlPersonaje(Personaje *jugadorExistente, QWidget *parent)
    : QWidget(parent),
    jugador(jugadorExistente),
    movimientoTimer(new QTimer(this)),
    shiftPresionado(false),
    izquierdaPresionada(false),
    derechaPresionada(false),
    arribaPresionado(false),
    abajoPresionado(false),
    cantidadCorazones(0)
{
    // Inicializar corazones vacíos
    for (int i = 0; i < 4; ++i) {
        QLabel* corazon = new QLabel(this);
        corazon->setPixmap(QPixmap(":/recursos/corazon_vacio.png").scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        corazon->setGeometry(10 + i * 36, 10, 32, 32);
        corazon->show();
        iconosCorazones.append(corazon);
    }
}

ControlPersonaje::~ControlPersonaje() {
    if (movimientoTimer) movimientoTimer->stop();
}

void ControlPersonaje::inicializarJugador() {
    if (!jugador) {
        jugador = new Personaje(this);
        auto anim = jugador->obtenerAnimacion("idle");
        jugador->SetAnimacion(anim.ruta, anim.frames);
    } else {
        jugador->setParent(this);
    }
    jugador->show();
    jugador->raise();
}

void ControlPersonaje::Movimientos() {
    connect(movimientoTimer, &QTimer::timeout, this, [=]() {

        bool moviendo = false;
        int dx = 0, dy = 0;

        // Determinar dirección del movimiento
        if (izquierdaPresionada)  { dx -= jugador->getVelocidadMovimiento(); moviendo = true; }
        if (derechaPresionada)    { dx += jugador->getVelocidadMovimiento(); moviendo = true; }
        if (arribaPresionado)     { dy -= jugador->getVelocidadMovimiento(); moviendo = true; }
        if (abajoPresionado)      { dy += jugador->getVelocidadMovimiento(); moviendo = true; }

        // Actualizar animación según estado
        if (moviendo) {
            if (shiftPresionado) {
                jugador->SetAnimacionMovimiento(6);
                auto anim = jugador->obtenerAnimacion("run");
                jugador->SetAnimacion(anim.ruta, anim.frames);
            } else {
                jugador->SetAnimacionMovimiento(2);
                auto anim = jugador->obtenerAnimacion("walk");
                jugador->SetAnimacion(anim.ruta, anim.frames);
            }

            // Movimiento con colisiones
            jugador->MoverSiNoColisiona(dx, dy, obstaculos);

        } else {
            movimientoTimer->stop();
        }

        // Actualiza zonas interactivas (como la puerta)
        onMovimientoUpdate();
    });

    movimientoTimer->setInterval(30);
}

void ControlPersonaje::keyPressEvent(QKeyEvent* event) {
    if (!jugador) return;

    switch (event->key()) {
    case Qt::Key_Shift: shiftPresionado = true; break;
    case Qt::Key_Left: izquierdaPresionada = true; if (!movimientoTimer->isActive()) movimientoTimer->start(); break;
    case Qt::Key_Right: derechaPresionada = true; if (!movimientoTimer->isActive()) movimientoTimer->start(); break;
    case Qt::Key_Up: arribaPresionado = true;
        if (!movimientoTimer->isActive())
            movimientoTimer->start();
        break;
    case Qt::Key_Down: abajoPresionado = true; if (!movimientoTimer->isActive()) movimientoTimer->start(); break;
    default: break;
    }
}

void ControlPersonaje::keyReleaseEvent(QKeyEvent* event) {
    if (!jugador) return;

    switch (event->key()) {
    case Qt::Key_Shift: shiftPresionado = false; break;
    case Qt::Key_Left: izquierdaPresionada = false; break;
    case Qt::Key_Right: derechaPresionada = false; break;
    case Qt::Key_Up: arribaPresionado = false; break;
    case Qt::Key_Down: abajoPresionado = false; break;
    }

    // Si no hay movimiento activo, volver a idle
    if (!izquierdaPresionada && !derechaPresionada && !arribaPresionado && !abajoPresionado) {
        jugador->DetenerAnimacion();
        auto anim = jugador->obtenerAnimacion("idle");
        jugador->SetAnimacion(anim.ruta, anim.frames);
    }
}

void ControlPersonaje::ResetearMovimiento() {
    if (movimientoTimer && movimientoTimer->isActive())
        movimientoTimer->stop();

    shiftPresionado = izquierdaPresionada = derechaPresionada =
        arribaPresionado = abajoPresionado = false;

    if (jugador) {
        jugador->DetenerAnimacion();
        auto anim = jugador->obtenerAnimacion("idle");
        jugador->SetAnimacion(anim.ruta, anim.frames);
    }
}

void ControlPersonaje::ActualizarCorazones(int nuevaCantidad) {
    if (nuevaCantidad < 0) nuevaCantidad = 0;
    if (nuevaCantidad > 4) nuevaCantidad = 4;
    cantidadCorazones = nuevaCantidad;

    for (int i = 0; i < 4; ++i) {
        if (i < cantidadCorazones)
            iconosCorazones[i]->setPixmap(QPixmap(":/recursos/corazon_lleno.png").scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        else
            iconosCorazones[i]->setPixmap(QPixmap(":/recursos/corazon_vacio.png").scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

//controlPersonaje->ActualizarCorazones(controlPersonaje->cantidadCorazones + 1); al completar las misiones







