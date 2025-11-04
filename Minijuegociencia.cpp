#include "Minijuegociencia.h"
#include "interior.h"
#include "inventario.h"

#include <QPixmap>
#include <QDebug>
#include <QTimer>
#include <QMouseEvent>
#include <QApplication>

extern Inventario* inventarioGlobal;

MinijuegoCiencia::MinijuegoCiencia(Personaje* jugadorExistente, QWidget* parent, int Actual)
    : ControlPersonaje(jugadorExistente, parent),
    EstadoActual(Actual),
    ganaste(true),
    respuestasActivas(false),
    timerPatron(nullptr),
    patronIndex(0),
    reproduciendoPatron(false)
{
    qDebug() << "🟢 Iniciando constructor MinijuegoCiencia...";

    this->resize(848, 1200);
    this->setWindowTitle("Minijuego Ciencia");

    fondoLabel = new QLabel(this);
    fondoLabel->setGeometry(0, 0, width(), height());
    fondoLabel->lower();

    labelPregunta = new QLabel(this);
    labelPregunta->setStyleSheet("background: rgba(0,0,0,180); color: white; padding: 6px; border-radius: 6px; font: bold 14px 'Courier New';");
    labelPregunta->setAlignment(Qt::AlignCenter);
    labelPregunta->setFixedSize(800, 80);
    labelPregunta->move(24, 30);

    labelRespuestas = new QLabel(this);
    labelRespuestas->setStyleSheet("background: rgba(0,0,0,180); color: white; padding: 6px; border-radius: 6px; font: bold 14px 'Courier New';");
    labelRespuestas->setAlignment(Qt::AlignLeft);
    labelRespuestas->setFixedSize(800, 100);
    labelRespuestas->move(24, 130);
    labelRespuestas->hide();

    qDebug() << "Labels creados";

    // PUERTA
    labelPuerta = new QLabel(this);
    labelPuerta->setStyleSheet("background: rgba(0,0,0,180); color: yellow;");
    labelPuerta->setAlignment(Qt::AlignCenter);
    labelPuerta->setFixedSize(260, 40);
    labelPuerta->hide();

    // Configuración escena antes de cargar preguntas
    configurarEscena();
    configurarObstaculos();
    ActualizarCorazones();

    // Reubicar jugador SIN cambiar parent
    jugador = jugadorExistente;
    if(jugador){
        jugador->setParent(this);
        jugador->move(400,818);
        jugador->show();
        jugador->raise();
        qDebug() << "Jugador colocado" << jugador->pos();
    }

    // Timer del patrón
    timerPatron = new QTimer(this);
    timerPatron->setInterval(1000);
    connect(timerPatron, &QTimer::timeout, this, &MinijuegoCiencia::reproducirSiguientePasoPatron);

    // Colas
    preguntas.cargarPreguntasPorTipo("Ciencia");
    patronCorrecto.clear();
    patronJugador.clear();
    playbackList.clear();


    cargarPreguntaActual();
    Movimientos();

    qDebug() << "Constructor MinijuegoCiencia terminado sin crashes";
}



void MinijuegoCiencia::configurarEscena() {
    QString ruta = "Sprites/Castle/Minijuegos/Ciencia/Ciencia.png";
    QPixmap fondo(ruta);


    if(fondo.isNull()) {
        qDebug() << "No se encontró la imagen" << ruta;
    }

    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    fondoLabel->lower();
    this->update();
    labelPregunta->update();
}

void MinijuegoCiencia::cambiarEscena(int valor)
{
    QString letra = letraParaIndice(valor);
    QString ruta = "Sprites/Castle/Minijuegos/Ciencia/Ciencia " + letra + ".png";
    QPixmap fondo(ruta);
     qDebug() << ruta;

    if(fondo.isNull()) {
        qDebug() << "No se encontró la imagen" << ruta;
        return;
    }

    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    QTimer::singleShot(2000, this, [this]() {
        configurarEscena();
    });
}



void MinijuegoCiencia::configurarObstaculos()
{
    obstaculos.clear();

}

QString MinijuegoCiencia::letraParaIndice(int i) const
{
    switch (i) {
    case 0: return "A";
    case 1: return "B";
    case 2: return "C";
    case 3: return "D";
    default: return "A";
    }
}

void MinijuegoCiencia::cargarPreguntaActual()
{

    if (!preguntas.isEmpty()) {
        preguntaActual = preguntas.dequeue();
        labelPregunta->setText(preguntaActual.texto);

        patronCorrecto.enqueue(preguntaActual.respuestaCorrecta);

        respuestasActivas = false;
        labelRespuestas->hide();

        QTimer::singleShot(1000, this, [this]() {
            reproducirPatron();
        });


    } else {
        labelPregunta->setText("¡Has completado el minijuego Ciencia!");
        labelRespuestas->hide();
        bool correcto = true;
        termino=true;
        if (patronJugador.size() != patronCorrecto.size()) correcto = false;
        else {
            QQueue<char> copiaC = patronCorrecto;
            QQueue<char> copiaJ = patronJugador;
            while (!copiaC.isEmpty()) {
                if (copiaC.dequeue() != copiaJ.dequeue()) { correcto = false; break; }
            }
        }
        if (ganaste){
            ActualizarCorazones(true);
            inventarioGlobal->agregarMedallaCiencia();
        }
        else ActualizarCorazones(false);
    }
}



void MinijuegoCiencia::actualizarRespuestas(int nuevaRespuesta){
    if (!respuestasActivas) return;

    char letra = 'A' + nuevaRespuesta;
    patronJugador.enqueue(letra);
    qDebug() << "Jugador eligió:" << letra;


    if (patronJugador.back() != patronCorrecto.at(patronJugador.size() - 1)) {
        qDebug() << "Error, reiniciando turno.";

        //mensaje de error en rojo
        labelPregunta->setStyleSheet("background: rgba(0,0,0,180); color: red; font-weight: bold;");
        labelPregunta->setText("Error, reiniciando turno...");

        QString ruta = "Sprites/Castle/Minijuegos/Ciencia/CienciaError.png";
        QPixmap fondo(ruta);
        fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

        patronJugador.clear();
        respuestasActivas = false;


        QTimer::singleShot(1200, this, [this]() {
            // Restaurar estilo original
            labelPregunta->setStyleSheet("background: rgba(0,0,0,180); color: white;");
            labelPregunta->setText(preguntaActual.texto);
            reproducirPatron();
        });
        return;
    }

    //Si se completó correctamente el patrón
    if (patronJugador.size() == patronCorrecto.size()) {
        qDebug() << "Secuencia correcta. Cargando siguiente pregunta...";
        respuestasActivas = false;
        patronJugador.clear();

        labelPregunta->setStyleSheet("background: rgba(0,0,0,180); color: lightgreen; font-weight: bold;");
        labelPregunta->setText("Correcto, avanzando...");

        QTimer::singleShot(1000, this, [this]() {
            labelPregunta->setStyleSheet("background: rgba(0,0,0,180); color: white;");
            cargarPreguntaActual();
        });
    }
}





void MinijuegoCiencia::reproducirPatron()
{
    if (reproduciendoPatron) return;
    if (patronCorrecto.isEmpty()) return;

    reproduciendoPatron = true;
    patronIndex = 0;
    playbackList.clear();

    QQueue<char> copia = patronCorrecto;
    int copiasize = copia.size();

    //no reproducir el ultimo
    for (int i = 0; i < copiasize - 1; ++i) {
        char letra = copia.dequeue();
        int indice = 0;
        if (letra == 'A') indice = 0;
        else if (letra == 'B') indice = 1;
        else if (letra == 'C') indice = 2;
        else if (letra == 'D') indice = 3;
        playbackList.append(indice);
    }

    qDebug() << "Reproduciendo patrón:" << playbackList;
    patronIndex = 0;
    timerPatron->start(700);
}



void MinijuegoCiencia::reproducirSiguientePasoPatron()
{
    if (patronIndex < playbackList.size()) {
        int indice = playbackList[patronIndex];
        cambiarEscena(indice);
        patronIndex++;
    } else {
        timerPatron->stop();
        reproduciendoPatron = false;
        qDebug() << "Fin de reproducción del patrón.";

        configurarEscena();
        respuestasActivas = true;
        patronJugador.clear();
    }
}




void MinijuegoCiencia::ConfirmarRespuestas()
{
    if (patronJugador.isEmpty()) return;

    QQueue<char> copiaCorrecto = patronCorrecto;
    QQueue<char> copiaJugador = patronJugador;

    bool correctoHastaAhora = true;

    int index = 1;
    while (!copiaJugador.isEmpty() && !copiaCorrecto.isEmpty()) {
        char respJ = copiaJugador.dequeue();
        char respC = copiaCorrecto.dequeue();

        if (respJ != respC) {
            correctoHastaAhora = false;
            qDebug() << "Error en la respuesta #" << index
                     << "Jugador:" << respJ << "Correcta:" << respC;
            break;
        }
        index++;
    }

    if (correctoHastaAhora) {
        if (patronJugador.size() == patronCorrecto.size()) {
            qDebug() << "Patrón completo y correcto!";
            labelPregunta->setText("¡Has completado correctamente el patrón!");
            ActualizarCorazones(true);
            QTimer::singleShot(2000, this, [this]() { SalirMinijuego(); });
        } else {
            qDebug() << "Correcto hasta ahora (" << patronJugador.size() << "/" << patronCorrecto.size() << ")";
        }
    } else {
        qDebug() << "Error: patrón incorrecto. Reiniciando...";
        ActualizarCorazones(false);
        patronJugador.clear();
    }
}



void MinijuegoCiencia::detectarZonaSeleccion()
{
    if (!jugador) return;

    // Refrescar rectángulos por si el tamaño cambia
    zonaA = QRect(44, 624, 60, 120);
    zonaB = QRect(184, 606, 60, 120);
    zonaC = QRect(356, 578, 60, 120);
    zonaD = QRect(492, 600, 60, 120);

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
    if (cercaA && !prevA) qDebug() << "Cerca del Frasco Rojo (A)";
    if (cercaB && !prevB) qDebug() << "Cerca del Frasco Amarillo (B)";
    if (cercaC && !prevC) qDebug() << "Cerca del Frasco Azul (C)";
    if (cercaD && !prevD) qDebug() << "Cerca del Frasco Verde (D)";
    if (!cercaA && !cercaB && !cercaC && !cercaD &&
        (prevA || prevB || prevC || prevD)) {
        qDebug() << "Lejos de todos los frascos";
    }
}


void MinijuegoCiencia::seleccionarLetra(int letra)
{
    if (reproduciendoPatron) return; // bloquear selección durante reproducción
    actualizarRespuestas(letra);
}

void MinijuegoCiencia::detectarZonaPuerta()
{
    if (!jugador) return;

    QPoint pos = jugador->pos();

    if (zonaPuerta.contains(pos)) {
        mostrarHintPuerta(zonaPuerta);
        hayPuertaCerca = true;
    } else {
        ocultarHintPuerta();
        hayPuertaCerca = false;
    }
}

void MinijuegoCiencia::mostrarHintPuerta(const QRect& zonaPuerta)
{
    if (!labelPuerta->isVisible()) {
        labelPuerta->setText("Presiona Z para ver respuestas");
        labelPuerta->move(zonaPuerta.center().x() - labelPuerta->width() / 2,
                          zonaPuerta.top() - labelPuerta->height() - 10);
        labelPuerta->show();
        labelPuerta->raise();
    }
}

void MinijuegoCiencia::ocultarHintPuerta()
{
    labelPuerta->hide();
}

void MinijuegoCiencia::onMovimientoUpdate()
{
    if (!jugador) return;

    detectarZonaPuerta();
    detectarZonaSeleccion();

        QRect rectJugador = jugador->geometry();
    if (rectJugador.intersects(QRect(732,512, 15, 100)) && termino) {
            SalirMinijuego();
            qDebug() << "Salir";
    }


}

void MinijuegoCiencia::keyPressEvent(QKeyEvent* event)
{
    ControlPersonaje::keyPressEvent(event);


    // Toggle respuestas con Z
    if (event->key() == Qt::Key_Z) {
        respuestasActivas = !respuestasActivas;
        if (respuestasActivas) {
            QString texto;
            texto += "A: " + preguntaActual.respuestaA + "\n";
            texto += "B: " + preguntaActual.respuestaB + "\n";
            texto += "C: " + preguntaActual.respuestaC + "\n";
            texto += "D: " + preguntaActual.respuestaD + "\n";
            labelRespuestas->setText(texto);
            labelRespuestas->show();
        } else {
            labelRespuestas->hide();
        }
    }

    // seleccionar letra con Q si estamos cerca
    if (event->key() == Qt::Key_Q) {
        detectarZonaSeleccion();

        int respuesta = -1;
        if (cercaA) { cambiarEscena(0); respuesta = 0; }
        else if (cercaB) { cambiarEscena(1); respuesta = 1; }
        else if (cercaC) { cambiarEscena(2); respuesta = 2; }
        else if (cercaD) { cambiarEscena(3); respuesta = 3; }

        if (respuesta != -1) {
            seleccionarLetra(respuesta);
        }



        if (hayPuertaCerca && zonaPuerta.contains(jugador->pos())) {
            SalirMinijuego();
        }
    }
}

void MinijuegoCiencia::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    qDebug() << "Jugador en:" << jugador->pos();
    ResetearMovimiento();

    if (cercaA) qDebug() << "Cerca A";
    else if (cercaB) qDebug() << "Cerca B";
    else if (cercaC) qDebug() << "Cerca C";
    else if (cercaD)  qDebug() << "Cerca D";
}

void MinijuegoCiencia::SalirMinijuego(){
    if (!jugador) return;

    ResetearMovimiento();

    if (erroress >= 3)
        ActualizarCorazones(true);


    Interior* interior = new Interior(jugador,nullptr,4);
    jugador->move(434,472);
    interior->show();


    this->close();
}


