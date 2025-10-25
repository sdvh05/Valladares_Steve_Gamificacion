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
    labelPregunta->setFixedSize(800, 80);
    labelPregunta->move(24, 30);
    labelPregunta->show();

    labelRespuestas = new QLabel(this);
    labelRespuestas->setStyleSheet("background: rgba(0,0,0,180); color: white; padding: 6px; border-radius: 6px; font: bold 14px 'Courier New';");
    labelRespuestas->setAlignment(Qt::AlignLeft);
    labelRespuestas->setFixedSize(800, 100);
    labelRespuestas->move(24, 130);
    labelRespuestas->hide();

    // --- Inicializar preguntas ---
    preguntas.cargarPreguntasPorTipo("Historia");
    cargarPreguntaActual();


    Movimientos();


}

void MinijuegoHistoria::configurarEscena(){
    calcularEstadoDesdePuentes();
    fondoLabel->lower();
    fondoLabel->show();
}

void MinijuegoHistoria::calcularEstadoDesdePuentes() {
    QString nombreArchivoBase = "Sprites/Castle/Minijuegos/Bridges/";

    QString errorCombo = "";
    if (!puentes[0]) errorCombo += "A";
    if (!puentes[1]) errorCombo += "B";
    if (!puentes[2]) errorCombo += "C";
    if (!puentes[3]) errorCombo += "D";

    if (errorCombo.isEmpty()) {
        EstadoActual = 1;
        fondoLabel->setPixmap(QPixmap(nombreArchivoBase + "AllBridges.png")
                                  .scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        return;
    }

    // Crear nombre del archivo tipo "AB Wrong.png"
    QString ruta = nombreArchivoBase + errorCombo + " Wrong.png";

    QPixmap fondo(ruta);
    if (fondo.isNull()) {
        qDebug() << "Error al cargar:" << ruta;
        return;
    }

    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    //EstadoActual = 2;
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





void MinijuegoHistoria::onMovimientoUpdate() {
    QRect rectJugador = jugador->geometry();
    detectarZonaPuerta();

    if (rectJugador.intersects(zonaPuerta)) {
        mostrarHintPuerta(zonaPuerta);
        hayPuertaCerca = true;
    } else {
        ocultarHintPuerta();
        hayPuertaCerca = false;
    }

    // --- PUENTES ---
    bool huboCambio = false;

    if (rectJugador.intersects(QRect(116,524, 15, 100)) && preguntaActual.respuestaCorrecta != 'A' && puentes[0]) {
        puentes[0] = false;
        huboCambio = true;
        qDebug() << "A cayó";
    }
    else if (rectJugador.intersects(QRect(278,524, 15, 100)) && preguntaActual.respuestaCorrecta != 'B' && puentes[1]) {
        puentes[1] = false;
        huboCambio = true;
        qDebug() << "B cayó";
    }
    else if (rectJugador.intersects(QRect(438,524, 15, 100)) && preguntaActual.respuestaCorrecta != 'C' && puentes[2]) {
        puentes[2] = false;
        huboCambio = true;
        qDebug() << "C cayó";
    }
    else if (rectJugador.intersects(QRect(608,524, 15, 100)) && preguntaActual.respuestaCorrecta != 'D' && puentes[3]) {
        puentes[3] = false;
        huboCambio = true;
        qDebug() << "D cayó";
    }

    if (huboCambio) {
        calcularEstadoDesdePuentes();
        configurarObstaculos();
        ResetearMovimiento();
        jugador->move(400,818);
        this->update();
        ganaste = false;
        actualizarRespuestas();
    }

    // --- Final ---
    if (rectJugador.intersects(QRect(222,200, 200, 100))) {
        jugador->move(400,818);
        ResetearMovimiento();
        cargarPreguntaActual();
        respuestasActivas = false;
        labelRespuestas->hide();
        puentes = {true, true, true, true}; // restaurar todos
        calcularEstadoDesdePuentes();
        actualizarRespuestas();

        if (preguntas.isEmpty()) {
            qDebug() << "Se terminó el minijuego";
            ActualizarCorazones(ganaste);
        }
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

    if (event->key() == Qt::Key_Z && !preguntaActual.texto.isEmpty()) {
        respuestasActivas = !respuestasActivas;

        if (respuestasActivas) {
            actualizarRespuestas();
            labelRespuestas->show();
        } else {
            labelRespuestas->hide();
        }
    }
}


void MinijuegoHistoria::actualizarRespuestas() {
    if (!respuestasActivas || preguntaActual.texto.isEmpty()) return;

    QString textoRespuestas;

    QString colorA = puentes[0] ? "white" : "red";
    QString colorB = puentes[1] ? "white" : "red";
    QString colorC = puentes[2] ? "white" : "red";
    QString colorD = puentes[3] ? "white" : "red";

    textoRespuestas += QString("<font color='%1'>A: %2</font><br>").arg(colorA, preguntaActual.respuestaA);
    textoRespuestas += QString("<font color='%1'>B: %2</font><br>").arg(colorB, preguntaActual.respuestaB);
    textoRespuestas += QString("<font color='%1'>C: %2</font><br>").arg(colorC, preguntaActual.respuestaC);
    textoRespuestas += QString("<font color='%1'>D: %2</font>").arg(colorD, preguntaActual.respuestaD);

    labelRespuestas->setTextFormat(Qt::RichText);
    labelRespuestas->setText(textoRespuestas);
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
    qDebug() << "Respuesta: " <<preguntaActual.respuestaCorrecta;
    ResetearMovimiento();

    this->ActualizarCorazones(true);
    qDebug() << "Vidas:" << jugador->getCorazones();



}

void MinijuegoHistoria::cargarPreguntaActual() {
    if (!preguntas.isEmpty()) {
        preguntaActual = preguntas.dequeue();
        labelPregunta->setText(preguntaActual.texto);
        labelRespuestas->hide();
    } else {
        qDebug() << "No hay más preguntas en la cola.";
        labelPregunta->setText("¡Has Completado el Minijuegos!");
        labelRespuestas->hide();
    }
}
