#include "Minijuegohistoria.h"
#include "ColaPreguntas.h"

#include <QPixmap>
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QApplication>
#include <QRandomGenerator>
#include <QList>

MinijuegoHistoria::MinijuegoHistoria(Personaje* jugadorExistente, QWidget* parent, int EstadoActual)
    : ControlPersonaje(jugadorExistente, parent), EstadoActual(EstadoActual)
{
    this->resize(848, 1200);
    this->setWindowTitle("Minijuego Historia");

    fondoLabel = new QLabel(this);
    fondoLabel->setGeometry(0, 0, width(), height());
    fondoLabel->lower();

    configurarEscena();
    configurarObstaculos();
    ActualizarCorazones();

    jugador = jugadorExistente;
    jugador->setParent(this);


    jugador->move(400,818);

    jugador->show();
    jugador->raise();


    // --- LABEL DE PUERTA ---
    labelPuerta = new QLabel("Presiona Z para ver la Pregunta", this);
    labelPuerta->setStyleSheet(
        "background: rgba(0,0,0,180); color: white; padding: 6px; "
        "border-radius: 6px; font: bold 14px 'Courier New';"
        );
    labelPuerta->setAlignment(Qt::AlignCenter);
    labelPuerta->setFixedSize(220, 30);
    labelPuerta->hide();

    // Pregunta y Respuestas
    labelPregunta = new QLabel(this);
    labelPregunta->setStyleSheet("background: rgba(0,0,0,180); color: white; padding: 6px; border-radius: 6px; font: bold 14px 'Courier New';");
    labelPregunta->setAlignment(Qt::AlignCenter);
    labelPregunta->setFixedSize(800, 50);
    labelPregunta->move(24, 20);
    labelPregunta->show();

    labelRespuestas = new QLabel(this);
    labelRespuestas->setStyleSheet("background: rgba(0,0,0,180); color: white; padding: 6px; border-radius: 6px; font: bold 14px 'Courier New';");
    labelRespuestas->setAlignment(Qt::AlignLeft);
    labelRespuestas->setFixedSize(800, 150);
    labelRespuestas->move(24, 80);
    labelRespuestas->hide();

    // --- Inicializar preguntas ---
    preguntas.cargarPreguntasPorTipo("Historia");
    cargarPreguntaActual();


    Movimientos();


}

void MinijuegoHistoria::configurarEscena(){
    int pasillo=EstadoActual;
    QString rutaFondo;

    switch (pasillo) {
    case 1: //normal
        rutaFondo = "Sprites/Castle/Minijuegos/Bridges/AllBridges.png";
        break;
    case 2: //A X
        rutaFondo = "Sprites/Castle//MinijuegosBridges/A Wrong.png";
        break;
    case 3: //B X
        rutaFondo = "Sprites/Castle//MinijuegosBridges/B Wrong.png";
        break;

    case 4: //C X
        rutaFondo = "Sprites/Castle//MinijuegosBridges/C Wrong.png";
        break;
    case 5: //D X
        rutaFondo = "Sprites/Castle//MinijuegosBridges/D Wrong.png";
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

    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    fondoLabel->lower();
    fondoLabel->show();

}

void MinijuegoHistoria::configurarObstaculos(){
    obstaculos.clear();
    int pasillo=EstadoActual;

    switch (pasillo) {
    case 1:

        break;

    case 2:

        break;

    case 3:

        break;

    case 4:

        break;
    }
}





void MinijuegoHistoria::onMovimientoUpdate()
{

    QRect rectJugador = jugador->geometry();
    detectarZonaPuerta();

    if (rectJugador.intersects(zonaPuerta)) {
        mostrarHintPuerta(zonaPuerta);
        hayPuertaCerca = true;
    } else {
        ocultarHintPuerta();
        hayPuertaCerca = false;
    }


}

void MinijuegoHistoria::detectarZonaPuerta() {
    if (!jugador) return;

    QPoint pos = jugador->pos();


    // --- Detectar la puerta según el pasillo actual ---
    switch (EstadoActual) {
    case 1:
        zonaPuerta = QRect(400, 888, 180, 100);
        break;
    case 2:
        zonaPuerta = QRect(820, 600, 150, 100);
        break;
    case 3:
        zonaPuerta = QRect(408,680, 150, 100);
        break;


    case 4:
        zonaPuerta = QRect(400, 888, 180, 100); //ruleta
        break;
    case 5:
        zonaPuerta = QRect(-48,574, 50, 50); //Arte
        break;
    case 6:
        zonaPuerta = QRect(86,506, 50, 50); //Politica
        break;
    case 7:
        zonaPuerta = QRect(374,446, 50, 50); //Ciencia
        break;
    case 8:
        zonaPuerta = QRect(718,590, 50, 50); //Historia
        break;


    default:
        zonaPuerta = QRect();
        break;
    }


    if (zonaPuerta.contains(pos)) {
        mostrarHintPuerta(zonaPuerta);
        hayPuertaCerca = true;
    } else {
        ocultarHintPuerta();
        hayPuertaCerca = false;
    }
}


void MinijuegoHistoria::mostrarHintPuerta(const QRect& zonaPuerta) {
    if (!labelPuerta->isVisible()) {

        QString textoHint="Presiona Z para ver las Respuestas";



        labelPuerta->setText(textoHint);
        labelPuerta->move(
            zonaPuerta.center().x() - labelPuerta->width() / 2,
            zonaPuerta.top() - labelPuerta->height() - 10
            );
        labelPuerta->show();
        labelPuerta->raise();
    }
}


void MinijuegoHistoria::ocultarHintPuerta() {
    labelPuerta->hide();
}


void MinijuegoHistoria::keyPressEvent(QKeyEvent* event) {
    ControlPersonaje::keyPressEvent(event);

    if (event->key() == Qt::Key_Z) {
        if (!preguntaActual.texto.isEmpty()) {
            QString textoRespuestas;
            textoRespuestas += "A: " + preguntaActual.respuestaA + "\n";
            textoRespuestas += "B: " + preguntaActual.respuestaB + "\n";
            textoRespuestas += "C: " + preguntaActual.respuestaC + "\n";
            textoRespuestas += "D: " + preguntaActual.respuestaD + "\n";

            labelRespuestas->setText(textoRespuestas);
            labelRespuestas->show();
        }
    }
}

void MinijuegoHistoria::SalirMinijuego(){
    if (!jugador) return;

    ResetearMovimiento();


    this->close();
}




void MinijuegoHistoria::actualizarEstado(int indice) {
    //this->EstadoActual = 5 + indice;
    this->EstadoActual=8;
    configurarEscena();
    configurarObstaculos();
    this->update();
}


void MinijuegoHistoria::mousePressEvent(QMouseEvent* event)
{
    //qDebug() << "Coordenadas del click: " << event->pos();
    qDebug() << "Jugador en:" << jugador->pos();

    this->ActualizarCorazones(true);
    qDebug() << "Vidas:" << jugador->getCorazones();

    if(EstadoActual>4){
        EstadoActual=4;
        configurarEscena();
        configurarObstaculos();
    }

}

void MinijuegoHistoria::cargarPreguntaActual() {
    if (!preguntas.isEmpty()) {
        preguntaActual = preguntas.dequeue();
        labelPregunta->setText(preguntaActual.texto);
        labelRespuestas->hide(); // ocultar respuestas al cargar nueva pregunta
    } else {
        qDebug() << "No hay más preguntas en la cola.";
        labelPregunta->setText("¡Has respondido todas las preguntas!");
        labelRespuestas->hide();
    }
}
