#include "Interior.h"
#include "Frontview.h"

#include <QPixmap>
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QApplication>

Interior::Interior(Personaje* jugadorExistente, QWidget* parent, int PasilloActual)
    : ControlPersonaje(jugadorExistente, parent), pasilloActual(PasilloActual)
{
    this->resize(1000, 1200);
    this->setWindowTitle("Interior del Castillo");



    configurarEscena();
    configurarObstaculos();
    ActualizarCorazones();

    jugador = jugadorExistente;

    jugador->setParent(this);

    jugador->move(400,818);
    jugador->show();
    jugador->raise();


    // --- LABEL DE PUERTA ---
    labelPuerta = new QLabel("Presiona Q para salir", this);
    labelPuerta->setStyleSheet(
        "background: rgba(0,0,0,180); color: white; padding: 6px; "
        "border-radius: 6px; font: bold 14px 'Courier New';"
        );
    labelPuerta->setAlignment(Qt::AlignCenter);
    labelPuerta->setFixedSize(220, 30);
    labelPuerta->hide();

    Movimientos();
}

void Interior::configurarEscena(){
    int pasillo=pasilloActual;
    QString rutaFondo;

    switch (pasillo) {
    case 1: //entrada
        rutaFondo = "Sprites/Castle/Interior.jpg";
        break;
    case 2: //izquierda
        rutaFondo = "Sprites/Castle/Interior_2.jpg";
        break;
    case 3: //derecha
        rutaFondo = "Sprites/Castle/Interior_3.jpg";
        break;
    default:
        rutaFondo = "Sprites/Castle/Interior.jpg";
        break;
    }

    QPixmap fondo(rutaFondo);
    if (fondo.isNull()) {
        qDebug() << "Error al cargar el fondo:" << rutaFondo;
        return;
    }

    QLabel* fondoLabel = new QLabel(this);
    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    fondoLabel->setGeometry(0, 0, width(), height());
    fondoLabel->lower();
}

void Interior::configurarObstaculos(){
    obstaculos.clear();
    int pasillo=pasilloActual;

    switch (pasillo) {
    case 1:
        obstaculos.append(QRect(-30, 770, 1000, 4)); // suelo
        break;

    case 2:

        break;

    case 3:

        break;
    }
}





void Interior::onMovimientoUpdate()
{

    QRect rectJugador = jugador->geometry();
    QRect zonaPuerta(400,888, 180, 100);
    if (rectJugador.intersects(zonaPuerta)) {
        mostrarHintPuerta();
        hayPuertaCerca = true;
    } else {
        ocultarHintPuerta();
        hayPuertaCerca = false;
    }

    // --- Transiciones automáticas ---

        //MAIN ENTRADA
    if (pasilloActual == 1 && rectJugador.intersects(QRect(998, 784, 10, 100))) {
        //pasilloActual=2;
        //cambiarPasillo();
        qDebug()<<"transision izquierda";
    }
    else if (pasilloActual == 1 && rectJugador.intersects(QRect(-54, 784, 10, 100))) {
        //cambiarPasillo();
        qDebug()<<"transision derecha";
    }

    //PASILLOS
}

void Interior::detectarZonaPuerta() {
    if (!jugador) return;

    QPoint pos = jugador->pos();

    // Rango en el que se considera "frente a la puerta"
    QRect zonaPuerta(400,888, 180, 100);

    if (zonaPuerta.contains(pos)) {
        mostrarHintPuerta();
    } else {
        labelPuerta->hide();
    }
}


void Interior::mostrarHintPuerta(){
    if (!labelPuerta->isVisible()) {
        QRect zonaPuerta(400,888, 180, 100);
        labelPuerta->move(zonaPuerta.center().x() - labelPuerta->width()/2,
                          zonaPuerta.top() - labelPuerta->height() - 10);
        labelPuerta->show();
        labelPuerta->raise();
    }
}

void Interior::SalirCastillo(){
    if (!jugador) return;

    ResetearMovimiento();

    // Crear la nueva escena pasando el mismo jugador
    FrontView* FV = new FrontView(jugador);
    FV->show();

    this->close();
}

void Interior::ocultarHintPuerta()
{
    labelPuerta->hide();

}




void Interior::keyPressEvent(QKeyEvent* event){
    ControlPersonaje::keyPressEvent(event);

    if (event->key() == Qt::Key_Q && hayPuertaCerca) {
        SalirCastillo();
    }
}


void Interior::mousePressEvent(QMouseEvent* event)
{
    //qDebug() << "Coordenadas del click: " << event->pos();
    qDebug() << "Jugador en:" << jugador->pos();

    this->ActualizarCorazones(true);
    qDebug() << "Vidas:" << jugador->getCorazones();

}
