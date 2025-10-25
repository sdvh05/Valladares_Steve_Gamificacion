#include "Minijuegociencia.h"

#include <QPixmap>
#include <QDebug>
#include <QTimer>
#include <QMouseEvent>
#include <QApplication>

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
    labelPregunta->setStyleSheet("background: rgba(0,0,0,180); color: white;");
    labelPregunta->setAlignment(Qt::AlignCenter);
    labelPregunta->setFixedSize(800, 80);
    labelPregunta->move(24, 30);

    labelRespuestas = new QLabel(this);
    labelRespuestas->setStyleSheet("background: rgba(0,0,0,180); color: white;");
    labelRespuestas->setAlignment(Qt::AlignLeft);
    labelRespuestas->setFixedSize(800, 100);
    labelRespuestas->move(24, 130);
    labelRespuestas->hide();

    qDebug() << "✅ Labels creados";

    // ✅ PUERTA
    labelPuerta = new QLabel(this);
    labelPuerta->setStyleSheet("background: rgba(0,0,0,180); color: yellow;");
    labelPuerta->setAlignment(Qt::AlignCenter);
    labelPuerta->setFixedSize(260, 40);
    labelPuerta->hide();

    // ✅ Configuración escena antes de cargar preguntas
    configurarEscena();
    configurarObstaculos();
    ActualizarCorazones();

    // ✅ Reubicar jugador SIN cambiar parent
    jugador = jugadorExistente;
    if(jugador){
        jugador->setParent(this);
        jugador->move(400,818);
        jugador->show();
        jugador->raise();
        qDebug() << "✅ Jugador colocado" << jugador->pos();
    }

    // ✅ Timer del patrón
    timerPatron = new QTimer(this);
    connect(timerPatron, &QTimer::timeout,
            this, &MinijuegoCiencia::reproducirSiguientePasoPatron);

    // ✅ Preparar colas
    preguntas.cargarPreguntasPorTipo("Ciencia");
    patronCorrecto.clear();
    patronJugador.clear();
    playbackList.clear();

    // ✅ Cargar primera pregunta AHORA
    cargarPreguntaActual();

    // ✅ Activar movimiento
    Movimientos();

    qDebug() << "✅ Constructor MinijuegoCiencia terminado sin crashes ✅";
}



void MinijuegoCiencia::configurarEscena()
{
    calcularEstadoDesdeFrascos();
    fondoLabel->lower();
    fondoLabel->show();
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
    // Toma la siguiente pregunta y la muestra; agrega su respuesta correcta a la cola patronCorrecto
    if (!preguntas.isEmpty()) {
        preguntaActual = preguntas.dequeue();
        labelPregunta->setText(preguntaActual.texto);

        // enqueue la respuesta correcta al patrón correcto
        patronCorrecto.enqueue(preguntaActual.respuestaCorrecta);

        // ocultar respuestas hasta que el jugador presione Z
        respuestasActivas = false;
        labelRespuestas->hide();

        // NO reproducir automáticamente el patrón al entrar; reproducción será manual con reproducirPatron()
        // Si quieres reproducirlo manualmente: llama reproducirPatron() desde UI o tras cierto evento.
    } else {
        // fin del minijuego
        labelPregunta->setText("¡Has completado el minijuego Ciencia!");
        labelRespuestas->hide();
        // evaluar resultado final (opcional)
        bool correcto = true;
        if (patronJugador.size() != patronCorrecto.size()) correcto = false;
        else {
            QQueue<char> copiaC = patronCorrecto;
            QQueue<char> copiaJ = patronJugador;
            while (!copiaC.isEmpty()) {
                if (copiaC.dequeue() != copiaJ.dequeue()) { correcto = false; break; }
            }
        }
        if (correcto) ActualizarCorazones(true);
        else ActualizarCorazones(false);
    }
}

void MinijuegoCiencia::calcularEstadoDesdeFrascos()
{
    // Imagen base
    const QString rutaBase = "Sprites/Castle/Minijuegos/Ciencia/Ciencia.png";
    QPixmap fondo(rutaBase);
    if (fondo.isNull()) {
        qDebug() << "No se pudo cargar fondo base:" << rutaBase;
    } else {
        fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
}

void MinijuegoCiencia::actualizarVisualPatron(int valor)
{
    // valor: 0->A,1->B,2->C,3->D ; muestra imagen "Ciencia A.png" etc.
    QString rutaBase = "Sprites/Castle/Minijuegos/Ciencia/";
    QString letra = letraParaIndice(valor);
    QString ruta = rutaBase + "Ciencia " + letra + ".png";
    QPixmap fondo(ruta);
    if (fondo.isNull()) {
        qDebug() << "No se pudo cargar imagen de letra:" << ruta;
        return;
    }
    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void MinijuegoCiencia::actualizarRespuestas(int nuevaRespuesta)
{
    // nuevaRespuesta: 0..3 -> A..D
    char letra = 'A';
    if (nuevaRespuesta == 0) letra = 'A';
    else if (nuevaRespuesta == 1) letra = 'B';
    else if (nuevaRespuesta == 2) letra = 'C';
    else if (nuevaRespuesta == 3) letra = 'D';

    patronJugador.enqueue(letra);

    // Mostrar la última elección visualmente
    actualizarVisualPatron(nuevaRespuesta);

    QTimer::singleShot(600, this, [this]() {
        cargarPreguntaActual();
    });
}

void MinijuegoCiencia::reproducirPatron()
{
    playbackList.clear();
    QQueue<char> copia = patronCorrecto;
    while (!copia.isEmpty()) {
        char c = copia.dequeue();
        int idx = 0;
        if (c == 'A') idx = 0;
        else if (c == 'B') idx = 1;
        else if (c == 'C') idx = 2;
        else if (c == 'D') idx = 3;
        playbackList.append(idx);
    }

    if (playbackList.isEmpty()) return;

    patronIndex = 0;
    reproduciendoPatron = true;
    if (timerPatron) timerPatron->start();
}

void MinijuegoCiencia::reproducirSiguientePasoPatron()
{
    if (!reproduciendoPatron) {
        if (timerPatron) timerPatron->stop();
        return;
    }

    if (patronIndex >= playbackList.size()) {
        // fin del patron: volver a fondo base
        calcularEstadoDesdeFrascos();
        if (timerPatron) timerPatron->stop();
        reproduciendoPatron = false;
        patronIndex = 0;
        return;
    }

    int idx = playbackList[patronIndex];
    actualizarVisualPatron(idx);

    patronIndex++;
}

void MinijuegoCiencia::detectarZonaSeleccion()
{
    if (!jugador) return;

    zonaA = QRect(44, 624, 60, 120);
    zonaB = QRect(184, 606, 60, 120);
    zonaC = QRect(356, 578, 60, 120);
    zonaD = QRect(492, 600, 60, 120);

    QRect r = jugador->geometry();

    //Tengo que mostrar y Ocultar el Hint Seleccion
    if (r.intersects(zonaA)){
         cercaA = r.intersects(zonaA);
        qDebug()<<"Cerca del Frasco Rojo (A)";
    }
    else if (r.intersects(zonaB)){
        cercaB = r.intersects(zonaB);
        qDebug()<<"Cerca del Frasco Amarillo (B)";
    }
    else if (r.intersects(zonaC)){
        cercaC = r.intersects(zonaC);
        qDebug()<<"Cerca del Frasco Azul (C)";
    }
    else if (r.intersects(zonaD)){
        cercaA = r.intersects(zonaD);
        qDebug()<<"Cerca del Frasco Verde (D)";
    } else{

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
        if (cercaA) seleccionarLetra(0);
        else if (cercaB) seleccionarLetra(1);
        else if (cercaC) seleccionarLetra(2);
        else if (cercaD) seleccionarLetra(3);

        // si estamos en la zonaPuerta y queremos salir
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
}

void MinijuegoCiencia::SalirMinijuego()
{
    ResetearMovimiento();
    this->close();
}

void MinijuegoCiencia::actualizarEstado(int indice)
{
    EstadoActual = indice;
    configurarEscena();
    configurarObstaculos();
    this->update();
}
