#include "FrontView.h"
#include <QPixmap>
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QApplication>

FrontView::FrontView(Personaje* jugadorExistente, QWidget* parent)
    : ControlPersonaje(jugadorExistente, parent)
{
    this->resize(1000, 1200);
    this->setWindowTitle("FrontView - Entrada del Castillo");



    configurarEscena();
    configurarObstaculos();

    jugador = jugadorExistente;

    jugador->setParent(this);

    jugador->move(400,818);
    jugador->show();
    jugador->raise();

    // --- LABEL DE PUERTA ---
    labelPuerta = new QLabel("Presiona Q para entrar", this);
    labelPuerta->setStyleSheet(
        "background: rgba(0,0,0,180); color: white; padding: 6px; "
        "border-radius: 6px; font: bold 14px 'Courier New';"
        );
    labelPuerta->setAlignment(Qt::AlignCenter);
    labelPuerta->setFixedSize(220, 30);
    labelPuerta->hide();

    // --- Mostrar un hint temporal ---
    QLabel* hint = new QLabel(u8"Usa W A S D para moverte", this);
    hint->setStyleSheet(
        "color: white; background: rgba(0,0,0,150); padding: 5px;"
        "border-radius: 6px; font: bold 14px 'Segoe UI';"
        );
    hint->adjustSize();
    hint->move(width()/2 - hint->width()/2, height() - 80);
    hint->show();

    auto* op = new QGraphicsOpacityEffect(hint);
    hint->setGraphicsEffect(op);
    QTimer::singleShot(4000, this, [op, hint]() {
        auto* anim = new QPropertyAnimation(op, "opacity");
        anim->setDuration(2000);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
        QObject::connect(anim, &QPropertyAnimation::finished, hint, &QWidget::deleteLater);
    });

    Movimientos();
}

void FrontView::configurarEscena()
{
    QPixmap fondo("Sprites/Castle/FrontView.jpg");
    //QPixmap fondo("Sprites/Castle/FV1.png");

    if (fondo.isNull()) {
        qDebug() << "Error al cargar imagen del castillo (FrontView.jpg)";
    } else {
        QLabel* fondoLabel = new QLabel(this);
        // Escalar la imagen para que llene toda la pantalla sin mantener relación de aspecto
        fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        fondoLabel->setGeometry(0, 0, width(), height());
        fondoLabel->lower();
    }
}

void FrontView::configurarObstaculos()
{
    obstaculos.clear();

    // Bordes del mapa
    obstaculos.append(QRect(0, 0, width(), 100));      // parte superior
    obstaculos.append(QRect(0, height() - 50, width(), 50)); // parte inferior
    obstaculos.append(QRect(0, 0, 40, height()));      // izquierda
    obstaculos.append(QRect(width() - 40, 0, 40, height())); // derecha

    // Puerta
    obstaculos.append(QRect(280, 650, 180, 40)); // puerta, justo delante del jugador
}


void FrontView::onMovimientoUpdate()
{

    QRect rectJugador = jugador->geometry();
     QRect zonaPuerta(280, 680, 180, 100);
    if (rectJugador.intersects(zonaPuerta)) {
        mostrarHintPuerta();
        hayPuertaCerca = true;
    } else {
        ocultarHintPuerta();
        hayPuertaCerca = false;
    }
}

void FrontView::detectarZonaPuerta() {
    if (!jugador) return;

    QPoint pos = jugador->pos();

    // Rango en el que se considera "frente a la puerta"
    QRect zonaPuerta(280, 680, 180, 100);

    if (zonaPuerta.contains(pos)) {
        mostrarHintPuerta();
    } else {
        labelPuerta->hide();
    }
}


void FrontView::mostrarHintPuerta(){
    if (!labelPuerta->isVisible()) {
        QRect zonaPuerta(340, 680, 180, 100);
        labelPuerta->move(zonaPuerta.center().x() - labelPuerta->width()/2,
                          zonaPuerta.top() - labelPuerta->height() - 10);
        labelPuerta->show();
        labelPuerta->raise();
    }
}

void FrontView::EntrarCastillo(){
    if (!jugador) return;

    ResetearMovimiento();

    // Crear la nueva escena pasando el mismo jugador
    //InsideCastle* interior = new InsideCastle(jugador);
    //interior->show();

    this->close();
}


void FrontView::ocultarHintPuerta()
{
    labelPuerta->hide();

}

void FrontView::keyPressEvent(QKeyEvent* event){
    ControlPersonaje::keyPressEvent(event);

    if (event->key() == Qt::Key_Q && hayPuertaCerca) {
        EntrarCastillo();
    }
}


void FrontView::mousePressEvent(QMouseEvent* event)
{
    //qDebug() << "Coordenadas del click: " << event->pos();
    qDebug() << "Jugador en:" << jugador->pos();

}
