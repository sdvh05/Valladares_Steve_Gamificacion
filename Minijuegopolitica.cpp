#include "MinijuegoPolitica.h"
#include "ColaPreguntas.h"
#include "interior.h"
#include "inventario.h"

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

extern Inventario* inventarioGlobal;

MinijuegoPolitica::MinijuegoPolitica(Personaje* jugadorExistente, QWidget* parent)
    : ControlPersonaje(jugadorExistente, parent)
{
    this->resize(848, 1200);
    this->setWindowTitle("Minijuego Politica");

    fondoLabel = new QLabel(this);
    fondoLabel->setGeometry(0, 0, width(), height());
    fondoLabel->lower();

    configurarEscena();
    configurarObstaculos();
    ActualizarCorazones();

    MoleActual=0;

    jugador = jugadorExistente;
    jugador->setParent(this);


    jugador->move(228,488);

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
    Preguntas.cargarPreguntasPorTipo("Politica");

        preguntaActual = Preguntas.dequeue();
        labelPregunta->setText(preguntaActual.texto);


    // --- Iniciar timer del topo ---
    iniciarTimerMole();



    Movimientos();

}


void MinijuegoPolitica::iniciarTimerMole() {
    timerMole = new QTimer(this);
    connect(timerMole, &QTimer::timeout, this, &MinijuegoPolitica::cambiarMoleAleatorio);
    timerMole->start(3000);
}

void MinijuegoPolitica::cambiarMoleAleatorio() {
    int nuevo;
    do {
        nuevo = QRandomGenerator::global()->bounded(1, 5); // 1–4
    } while (nuevo == MoleActual);

    MoleAnterior = MoleActual;
    MoleActual = nuevo;

    qDebug() << "Nuevo mole:" << MoleActual;
    cambiarEscena();
}


void MinijuegoPolitica::configurarEscena(){
    cambiarEscena();
    fondoLabel->lower();
    fondoLabel->show();
}

void MinijuegoPolitica::cambiarEscena() {
    QString rutaFondo;

    if (MoleActual == 0)
        rutaFondo = "Sprites/Castle/Minijuegos/Mole/No Mole.png";
    else
        rutaFondo = QString("Sprites/Castle/Minijuegos/Mole/Mole %1.png")
                        .arg(QChar('A' + MoleActual - 1));

    QPixmap fondo(rutaFondo);
    if (fondo.isNull()) {
        qDebug() << "Error al cargar fondo:" << rutaFondo;
        return;
    }

    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}



void MinijuegoPolitica::mostrarHit() {
    timerMole->stop(); // Pausar movimiento de topo

    QString rutaFondo = QString("Sprites/Castle/Minijuegos/Mole/Mole %1 Hit.png")
                            .arg(QChar('A' + MoleActual - 1));

    QPixmap fondo(rutaFondo);
    if (fondo.isNull()) {
        qDebug() << "Error al cargar:" << rutaFondo;
        return;
    }

    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    QTimer::singleShot(1000, [this]() {
        // Regresar al topo normal
        cambiarEscena();
        timerMole->start(3000);
    });
}



void MinijuegoPolitica::configurarObstaculos(){
    obstaculos.clear();




}





void MinijuegoPolitica::onMovimientoUpdate() {
    QRect rectJugador = jugador->geometry();

    if (!jugador) return;

    detectarZonaPuerta();
    detectarZonaSeleccion();


    if (rectJugador.intersects(QRect(732,512, 15, 100)) && termino) {
        SalirMinijuego();
        qDebug() << "Salir";
    }


}



void MinijuegoPolitica::detectarZonaPuerta() {
    if (!jugador) return;

    QPoint pos = jugador->pos();



    zonaPuerta = QRect(400, 888, 180, 100);

    if (zonaPuerta.contains(pos)) {
        mostrarHintPuerta(zonaPuerta);
        hayPuertaCerca = true;
    } else {
        ocultarHintPuerta();
        hayPuertaCerca = false;
    }




}


void MinijuegoPolitica::mostrarHintPuerta(const QRect& zonaPuerta) {
    if (!labelPuerta->isVisible()) {


        QString textoHint;

        textoHint="Presiona Z para ver las Respuestas";

        labelPuerta->setText(textoHint);
        labelPuerta->move(
            zonaPuerta.center().x() - labelPuerta->width() / 2,
            zonaPuerta.top() - labelPuerta->height() - 10
            );
        labelPuerta->show();
        labelPuerta->raise();
    }
}


void MinijuegoPolitica::ocultarHintPuerta() {
    labelPuerta->hide();
}


void MinijuegoPolitica::keyPressEvent(QKeyEvent* event) {
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

    if (event->key() == Qt::Key_Q) {
        detectarZonaSeleccion();

        int respuesta = -1;
        if (cercaA && MoleActual == 1) respuesta = 'A';
        else if (cercaB && MoleActual == 2) respuesta = 'B';
        else if (cercaC && MoleActual == 3) respuesta = 'C';
        else if (cercaD && MoleActual == 4) respuesta = 'D';

        if (respuesta != -1) {
            mostrarHit();

            if (preguntaActual.respuestaCorrecta == respuesta) {
                mostrarResultadoTemporal(false);
            } else {
                mostrarResultadoTemporal(true);
            }
        }
    }
}



void MinijuegoPolitica::detectarZonaSeleccion()
{
    if (!jugador) return;

    // Refrescar rectángulos por si el tamaño cambia
    zonaA = QRect(640,556, 60, 120);
    zonaB = QRect(474,590, 60, 120);
    zonaC = QRect(306,628, 60, 120);
    zonaD = QRect(118,674, 60, 120);

    QRect r = jugador->geometry();

    //Reiniciar estado siempre
    bool prevA = cercaA;
    bool prevB = cercaB;
    bool prevC = cercaC;
    bool prevD = cercaD;

    cercaA = r.intersects(zonaA);
    cercaB = r.intersects(zonaB);
    cercaC = r.intersects(zonaC);
    cercaD = r.intersects(zonaD);

    // Debug solo si cambió algo
    if (cercaA && !prevA) qDebug() << "Cerca del Agujero Rojo (A)";
    if (cercaB && !prevB) qDebug() << "Cerca del Agujero Azul (B)";
    if (cercaC && !prevC) qDebug() << "Cerca del Agujero Amarillo (C)";
    if (cercaD && !prevD) qDebug() << "Cerca del Agujero Verde (D)";
    if (!cercaA && !cercaB && !cercaC && !cercaD &&
        (prevA || prevB || prevC || prevD)) {
        qDebug() << "Lejos de todos los Agujero";
    }
}

void MinijuegoPolitica::seleccionarLetra(int letra)
{

    actualizarRespuestas();
}


void MinijuegoPolitica::actualizarRespuestas() {
    if (!respuestasActivas || preguntaActual.texto.isEmpty()) return;

    QString textoRespuestas;
    textoRespuestas += QString("<font color='white'>A: %1</font><br>").arg(preguntaActual.respuestaA);
    textoRespuestas += QString("<font color='white'>B: %1</font><br>").arg(preguntaActual.respuestaB);
    textoRespuestas += QString("<font color='white'>C: %1</font><br>").arg(preguntaActual.respuestaC);
    textoRespuestas += QString("<font color='white'>D: %1</font>").arg(preguntaActual.respuestaD);

    labelRespuestas->setTextFormat(Qt::RichText);
    labelRespuestas->setText(textoRespuestas);
    this->update();
}



void MinijuegoPolitica::SalirMinijuego(){
    if (!jugador) return;

    ResetearMovimiento();
    jugador->puntos+=this->puntos;
    jugador->guardarPuntos(jugador->nombre,jugador->puntos);

    Interior* interior = new Interior(jugador,nullptr,4);

    jugador->move(150,500);
    interior->show();
    ResetearMovimiento();


    this->close();
}







void MinijuegoPolitica::mousePressEvent(QMouseEvent* event)
{
    //qDebug() << "Coordenadas del click: " << event->pos();
    qDebug() << "Jugador en:" << jugador->pos();
    qDebug() << "Respuesta: " <<preguntaActual.respuestaCorrecta;
    ResetearMovimiento();

    //this->ActualizarCorazones(true);
    qDebug() << "Vidas:" << jugador->getCorazones();


}




void MinijuegoPolitica::mostrarResultadoTemporal(bool reiniciar) {
    // Mostrar mensaje temporal según resultado
    QString texto = reiniciar ? "¡Respuesta Incorrecta!" : "¡Respuesta Correcta";
    QString color = reiniciar ? "red" : "lightgreen";

    if(reiniciar){
        errores++;
        this->puntos -=10;
        if(puntos <=0)
            puntos=0;
    }

    labelPregunta->setStyleSheet(QString(
                                     "background: rgba(0,0,0,200); color: %1; padding: 8px; border-radius: 8px;"
                                     ).arg(color));
    labelPregunta->setText(texto);

    // Espera 1.5 segundos antes de continuar
    QTimer::singleShot(1500, [this, reiniciar]() {
        labelPregunta->setStyleSheet(
            "background: rgba(0,0,0,180); color: white; padding: 6px; border-radius: 6px;"
            );

        if (reiniciar) {
            // se equivoco
            labelPregunta->setText(preguntaActual.texto);
        } else {
            // Bien
            if (!Preguntas.isEmpty()) {
                preguntaActual = Preguntas.dequeue();
                labelPregunta->setText(preguntaActual.texto);
            } else {
                // C acabo
                ganaste = errores < 3;
                labelPregunta->setText("¡Has completado el minijuego!");
                ActualizarCorazones(ganaste);
                if(ganaste){
                    inventarioGlobal->agregarMedallaPolitica();
                    jugador->ganadas[1]=true;
                    puntos+=30;
                }
                timerMole->stop();
                termino = true;
            }
        }

        // Actualizar las respuestas visibles
        actualizarRespuestas();
    });
}




