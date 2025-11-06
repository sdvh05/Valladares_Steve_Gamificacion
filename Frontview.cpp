#include "FrontView.h"
#include "interior.h"

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
    ActualizarCorazones();
    //mostrarHitboxes();

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


    //prueba
    // --- NPC ---
    npcPrueba = new Npc(this);
    npcPrueba->setNombre("Guardia del Castillo");
    npcPrueba->move(428,810);
    npcPrueba->show();

    // Diálogo del NPC
    QStringList dialogo;
    dialogo << "¡Hola Guapo!"
            << "Subi y pasa al Castillo"
            << "Saludame al Inge Erick";

    npcPrueba->setDialogos(dialogo);


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
    obstaculos.append(QRect(0, 0, width(), 100));      // arriba
    obstaculos.append(QRect(0, height() - 50, width(), 50)); // abajo
    obstaculos.append(QRect(0, 0, 40, height()));      // izquierda
    obstaculos.append(QRect(width() - 40, 0, 40, height())); // derecha

    // Puerta
    obstaculos.append(QRect(280, 650, 180, 40));

    //Puente
    obstaculos.append(QRect(70,730, 10,10));

    obstaculos.append(QRect(102,706, 10,10));
    obstaculos.append(QRect(134,690, 10,10));
    obstaculos.append(QRect(134,690, 10,10));
    obstaculos.append(QRect(152,686, 10,10));

    obstaculos.append(QRect(660,842, 5,5));
    obstaculos.append(QRect(600,804, 5,5));
    obstaculos.append(QRect(548,770, 5,5));
    obstaculos.append(QRect(506,738, 5,5));
    obstaculos.append(QRect(480,720, 5,5));
    obstaculos.append(QRect(70,730, 5,5));
    obstaculos.append(QRect(462,690, 5,5));
    obstaculos.append(QRect(462,668, 5,5));


}


void FrontView::mostrarHitboxes()
{
    if (!hitboxLabels.isEmpty()) {
        qDeleteAll(hitboxLabels);
        hitboxLabels.clear();
    }

    for (int i = 0; i < obstaculos.size(); ++i) {
        QRect obstaculo = obstaculos[i];

        QLabel *label = new QLabel(this);
        label->setGeometry(obstaculo);

        // Color diferente para bordes vs obstáculos pequeños
        QString style;

            style = "background-color: rgba(255, 0, 0, 150); border: 2px solid red; color: white; font: bold 8pt;";

        label->setStyleSheet(style);
        label->setText(QString("%1").arg(i));
        label->setAlignment(Qt::AlignCenter);
        label->show();

        hitboxLabels.append(label);

        // Debug en consola
        qDebug() << "Hitbox" << i
                 << "Pos: (" << obstaculo.x() << "," << obstaculo.y() << ")"
                 << "Size:" << obstaculo.width() << "x" << obstaculo.height();
    }
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

    Interior* interior = new Interior(jugador);
    interior->show();

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
    if (event->key() == Qt::Key_E && npcPrueba) {
        npcPrueba->Interactuar(jugador);
    }


}


void FrontView::mousePressEvent(QMouseEvent* event)
{
    //qDebug() << "Coordenadas del click: " << event->pos();
    qDebug() << "Jugador en:" << jugador->pos();
    this->ActualizarCorazones(true);

    jugador->Bando++;
    if(jugador->Bando==3) jugador->Bando=0;
    ResetearMovimiento();
    jugador->move(400,818);
    qDebug() << jugador->Bando;


}
