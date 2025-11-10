#include "Minijuegoarte.h"


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

MinijuegoArte::MinijuegoArte(Personaje* jugadorExistente, QWidget* parent, int EstadoActual)
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

    qDebug() << ">>> Entrando a MinijuegoArte constructor";
    qDebug() << "   jugadorExistente pointer:" << jugadorExistente;
    qDebug() << "   parent pointer:" << parent;



    jugador->move(132,508);

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
    preguntas.cargarPreguntasPorTipo("Arte");
    cargarPreguntaActual();


    Movimientos();


}

void MinijuegoArte::configurarEscena(){
    cambiarEscena();
    fondoLabel->lower();
    fondoLabel->show();
}

void MinijuegoArte::cambiarEscena() {

    QString ruta = "Sprites/Castle/Minijuegos/Arte/Diana.png";

    QPixmap fondo(ruta);
    if (fondo.isNull()) {
        qDebug() << "Error al cargar:" << ruta;
        return;
    }

    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}


void MinijuegoArte::configurarObstaculos(){
    obstaculos.clear();

    /*obstaculos.append(QRect(50,734, 2, 320));
    obstaculos.append(QRect(206,734, 2, 320));
    obstaculos.append(QRect(532,734, 2, 320));
    obstaculos.append(QRect(696,734, 2, 320));
    obstaculos.append(QRect(50,734, 2, 320)); */

    \
}





void MinijuegoArte::onMovimientoUpdate() {
    QRect rectJugador = jugador->geometry();
    detectarZonaPuerta();


    ZonaA = QRect(650,256, 80, 100);
    ZonaB = QRect(650,414, 80, 100);
    ZonaC = QRect(650,578, 80, 100);
    ZonaD = QRect(650,800, 80, 100);


    QRect zonaFinal(300,112,1000,10);
    if (rectJugador.intersects(zonaFinal) && termino) {
        SalirMinijuego();
    }


}


void MinijuegoArte::detectarZonaPuerta() {
    if (!jugador) return;

    QPoint pos = jugador->pos();


    // --- Detectar la puerta según el pasillo actual ---
    if (!termino)
        zonaPuerta = QRect(400, 888, 180, 100);
    else
        zonaPuerta = QRect(222,200, 200, 100);




    if (zonaPuerta.contains(pos)) {
        mostrarHintPuerta(zonaPuerta);
        hayPuertaCerca = true;
    } else {
        ocultarHintPuerta();
        hayPuertaCerca = false;
    }
}


void MinijuegoArte::mostrarHintPuerta(const QRect& zonaPuerta) {
    if (!labelPuerta->isVisible()) {


        QString textoHint;
        if(termino){
            textoHint="Presiona Z para ver las Respuestas";
        }else{
            textoHint="Presiona Q para SALIR";
        }


        labelPuerta->setText(textoHint);
        labelPuerta->move(
            zonaPuerta.center().x() - labelPuerta->width() / 2,
            zonaPuerta.top() - labelPuerta->height() - 10
            );
        labelPuerta->show();
        labelPuerta->raise();
    }
}


void MinijuegoArte::ocultarHintPuerta() {
    labelPuerta->hide();
}


void MinijuegoArte::keyPressEvent(QKeyEvent* event) {
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

    if(event->key() == Qt::Key_Q && termino){
        SalirMinijuego();
    }

    if (event->key() == Qt::Key_Space) {
        lanzarPiedra();
    }

}


void MinijuegoArte::actualizarRespuestas() {
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


void MinijuegoArte::SalirMinijuego(){
    if (!jugador) return;

    ResetearMovimiento();

    bool ganaste = errores <= 3;
    ActualizarCorazones(ganaste);
    if (ganaste){
        inventarioGlobal->agregarMedallaArte();
        jugador->puertas[0]=true;
        this->puntos+=30;
    }

    jugador->puntos+=this->puntos;
    jugador->guardarPuntos(jugador->nombre,jugador->puntos);

    Interior* interior = new Interior(jugador,nullptr,4);
    jugador->move(734,568);
    interior->show();


    this->close();
}




void MinijuegoArte::actualizarEstado(int indice) {
    configurarEscena();
    configurarObstaculos();
    this->update();
}


void MinijuegoArte::mousePressEvent(QMouseEvent* event)
{
    //qDebug() << "Coordenadas del click: " << event->pos();
    qDebug() << "Jugador en:" << jugador->pos();
    qDebug() << "Respuesta: " <<preguntaActual.respuestaCorrecta;
    ResetearMovimiento();

    //this->ActualizarCorazones(true);
    qDebug() << "Vidas:" << jugador->getCorazones();


}

void MinijuegoArte::cargarPreguntaActual() {
    if (!preguntas.isEmpty()) {
        preguntaActual = preguntas.dequeue();
        labelPregunta->setText(preguntaActual.texto);
        labelRespuestas->hide();

        ultimaPregunta = preguntas.isEmpty();
    } else {
        qDebug() << "No hay más preguntas en la cola.";
        labelPregunta->setText("¡Has Completado el Minijuegos!");
        labelRespuestas->hide();
        termino=true;

        ultimaPregunta =true;
    }
}


void MinijuegoArte::verificarRespuesta(char letra) {
    if (preguntaActual.texto.isEmpty())
        return;

    bool esCorrecta = (letra == preguntaActual.respuestaCorrecta);

    QString mensaje = esCorrecta ? "¡Correcto!" : "Respuesta incorrecta...";
    QString color = esCorrecta ? "lightgreen" : "red";


    labelPregunta->setStyleSheet(QString(
                                     "background: rgba(0,0,0,180); color: %1; "
                                     "padding: 6px; border-radius: 6px; font: bold 18px 'Courier New';").arg(color));
    labelPregunta->setText(mensaje);

    actualizarRespuestas();

    // Esperar un segundo y restaurar la pregunta actual
    QTimer::singleShot(900, this, [=]() {
        labelPregunta->setStyleSheet(
            "background: rgba(0,0,0,180); color: white; padding: 6px; "
            "border-radius: 6px; font: bold 14px 'Courier New';");

        if (esCorrecta) {
            cargarPreguntaActual();
        } else {
            labelPregunta->setText(preguntaActual.texto);
            errores++;

            this->puntos -=10;
            if(puntos <=0)
                puntos=0;
        }

        actualizarRespuestas();
    });
}






//--------------------PIEDRA-------------------

void MinijuegoArte::lanzarPiedra() {
    if (!jugador || preguntaActual.texto.isEmpty()) return;

    auto anim = jugador->obtenerAnimacion("melee");
    jugador->SetAnimacion(anim.ruta, anim.frames);

    Piedra* p = new Piedra(this);
    p->label->setPixmap(QPixmap("Sprites/Castle/Minijuegos/Arte/Rock.png")
                            .scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    p->label->setGeometry(jugador->x() + 50, jugador->y()+50, 32, 32);
    p->label->show();
    p->label->raise();

    p->velocidad = QPoint(12, 0);



    QPointer<Piedra> weakP(p);
    connect(p->timer, &QTimer::timeout, this, [this, weakP]() {
        if (!weakP) return;
        moverPiedra(weakP);
    });

    p->timer->start(30);
    piedras.append(QPointer<Piedra>(p));
}

void MinijuegoArte::eliminarPiedra(Piedra* piedra) {
    if (!piedra) return;

    for (int i = piedras.size() - 1; i >= 0; --i) {
        if (piedras[i] == piedra) {
            piedras.removeAt(i);
            break;
        }
    }

    piedra->deleteLater();
}


void MinijuegoArte::moverPiedra(Piedra* piedra) {
    if (!piedra || !piedra->label) return;

    QPoint pos = piedra->label->pos();
    pos += piedra->velocidad;
    piedra->label->move(pos);

    // Verificar impacto
    verificarImpacto(piedra);

    // Si sale de la pantalla (arriba)
    if (piedra->label->x() > width()) {
        eliminarPiedra(piedra);
    }

}

// verificarImpacto: ajustada para trabajar con Piedra*
void MinijuegoArte::verificarImpacto(Piedra* piedra) {
    if (!piedra || !piedra->label) return;

    QRect rectPiedra = piedra->label->geometry();

    if (rectPiedra.intersects(ZonaA)) { verificarRespuesta('A'); eliminarPiedra(piedra); return; }
    if (rectPiedra.intersects(ZonaB)) { verificarRespuesta('B'); eliminarPiedra(piedra); return; }
    if (rectPiedra.intersects(ZonaC)) { verificarRespuesta('C'); eliminarPiedra(piedra); return; }
    if (rectPiedra.intersects(ZonaD)) { verificarRespuesta('D'); eliminarPiedra(piedra); return; }
}



