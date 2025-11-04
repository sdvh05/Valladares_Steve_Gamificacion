#include "Combate.h"
#include "interior.h"
#include "inventario.h"
#include <QPixmap>
#include <QDebug>
#include <QTimer>
#include <QPushButton>
#include <QRandomGenerator>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

extern Inventario* inventarioGlobal;

Combate::Combate(Personaje* jugadorExistente, QWidget* parent, int Bando)
    : ControlPersonaje(jugadorExistente, parent), Bando(Bando)
{
    resize(848, 1200);
    setWindowTitle("Epistemefighter: Combate por la Verdad");

    fondoLabel = new QLabel(this);
    fondoLabel->setGeometry(0, 0, width(), height());
    fondoLabel->lower();

    configurarEscena();
    configurarObstaculos();

    jugador = jugadorExistente;
    jugador->setParent(this);
    jugador->move(Bando == 1 ? 116 : 666, 702);
    jugador->show();
    jugador->raise();

    ActualizarCorazones();

    // --- Labels de interfaz ---
    labelPregunta = new QLabel(this);
    labelPregunta->setStyleSheet("background: rgba(0,0,0,180); color: white; padding: 10px; border-radius: 8px; font: bold 16px 'Courier New';");
    labelPregunta->setGeometry(24, 20+25, 800, 80);
    labelPregunta->setAlignment(Qt::AlignCenter);

    labelRespuestas = new QLabel(this);
    labelRespuestas->setStyleSheet("background: rgba(0,0,0,160); color: white; padding: 8px; border-radius: 6px; font: bold 14px 'Courier New';");
    labelRespuestas->setGeometry(24, 110+25, 800, 120);
    labelRespuestas->setAlignment(Qt::AlignLeft);
    labelRespuestas->show();

    // --- Banners ---
    bannerEmpirista = new QLabel("EMPIRISTAS", this);
    bannerEmpirista->setStyleSheet("background: rgba(0,50,200,150); color: white; font: bold 18px 'Courier New'; padding: 6px; border-radius: 6px;");
    bannerEmpirista->setGeometry(20, 250+25, 180, 30);

    bannerRacionalista = new QLabel("RACIONALISTAS", this);
    bannerRacionalista->setStyleSheet("background: rgba(200,50,0,150); color: white; font: bold 18px 'Courier New'; padding: 6px; border-radius: 6px;");
    bannerRacionalista->setGeometry(width() - 200, 250+25, 180, 30);

    // --- Botones de respuesta ---
    int btnY = 900;
    int btnW = 180;
    int btnH = 50;
    QString styleBtn = "background: rgba(0,0,0,200); color: white; border: 2px solid gray; font: bold 16px 'Courier New'; border-radius: 8px;";
    A = new QPushButton("A", this);
    B = new QPushButton("B", this);
    C = new QPushButton("C", this);
    D = new QPushButton("D", this);
    QList<QPushButton*> botones = {A, B, C, D};
    for (int i = 0; i < botones.size(); ++i) {
        botones[i]->setGeometry(20 + i * (btnW + 30), btnY, btnW, btnH);
        botones[i]->setStyleSheet(styleBtn);
        connect(botones[i], &QPushButton::clicked, [=]() { responder('A' + i); });
    }

    // --- Corazones enemigos ---
    for (int i = 0; i < 4; ++i) {
        QLabel* corazon = new QLabel(this);
        corazon->setPixmap(QPixmap("Sprites/Recursos/FullHeartE.png").scaled(32, 32));
        corazon->setGeometry(width() - 180 + i * 36, 10, 32, 32);
        corazon->show();
        iconosCorazonesEnemigo.append(corazon);
    }

    preguntas.cargarPreguntasPorTipo("Combate");
    cargarPreguntaActual();

    // --- Timer de IA ---
    timerRespuesta = new QTimer(this);
    connect(timerRespuesta, &QTimer::timeout, this, &Combate::EnemigoResponder);

    iniciarTurno();
}

void Combate::configurarEscena() { cambiarEscena(); }
void Combate::cambiarEscena() {
    QPixmap fondo("Sprites/Castle/Batalla.png");
    if (fondo.isNull()) {
        qDebug() << "Error cargando fondo.";
        return;
    }
    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    fondoLabel->show();
}
void Combate::configurarObstaculos() { obstaculos.clear(); }

void Combate::iniciarTurno() {
    jugadorRespondio = false;
    enemigoRespondio = false;
    timerRespuesta->start(8000); // IA responde en 8 segundos
}

void Combate::cargarPreguntaActual() {
    if (!preguntas.isEmpty()) {
        preguntaActual = preguntas.dequeueCircular();

        labelPregunta->setStyleSheet(
            "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
            "stop:0 rgba(120,100,70,220), stop:1 rgba(80,60,40,220));"
            "color: #fdf5e6;"
            "border: 3px solid #d4af37;"        // borde dorado
            "border-radius: 12px;"
            "padding: 10px;"
            "font: bold 18px 'Papyrus';"
            );
        labelPregunta->setText(preguntaActual.texto);

        AnsError = {true, true, true, true};
        actualizarRespuestas();
        labelRespuestas->show(); // ✅ Mostrar respuestas
    }
    else {
        labelPregunta->setText("🏆 ¡El combate ha terminado!");
        labelRespuestas->hide();
        termino = true;
        verificarVictoria();
    }
}

void Combate::actualizarRespuestas() {
    QString colorA = AnsError[0] ? "#fff" : "#e63946";
    QString colorB = AnsError[1] ? "#fff" : "#e63946";
    QString colorC = AnsError[2] ? "#fff" : "#e63946";
    QString colorD = AnsError[3] ? "#fff" : "#e63946";

    QString texto;
    texto += QString("<font color='%1'>A: %2</font><br>").arg(colorA, preguntaActual.respuestaA);
    texto += QString("<font color='%1'>B: %2</font><br>").arg(colorB, preguntaActual.respuestaB);
    texto += QString("<font color='%1'>C: %2</font><br>").arg(colorC, preguntaActual.respuestaC);
    texto += QString("<font color='%1'>D: %2</font>").arg(colorD, preguntaActual.respuestaD);

    labelRespuestas->setStyleSheet(
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
        "stop:0 rgba(100,80,50,220), stop:1 rgba(60,40,20,220));"
        "color: #fdf5e6;"
        "border: 2px solid #b8860b;"     // dorado más tenue
        "border-radius: 10px;"
        "padding: 10px;"
        "font: bold 15px 'Papyrus';"
        );
    labelRespuestas->setTextFormat(Qt::RichText);
    labelRespuestas->setText(texto);
    labelRespuestas->show();
}


void Combate::responder(char letra) {
    if (jugadorRespondio || termino) return;
    jugadorRespondio = true;
    timerRespuesta->stop();

    if (letra == preguntaActual.respuestaCorrecta) {
        mostrarResultadoTemporal(false, true, 0); // Jugador acierta
        CorazonesEnemigo--;
        ActualizarCorazonesEnemigos();
        verificarVictoria();
    } else {
        AnsError[letra - 'A'] = false;
        actualizarRespuestas();
        mostrarResultadoTemporal(true, false, 0); // Jugador falla (sin daño)
    }
}

void Combate::EnemigoResponder() {
    if (enemigoRespondio || termino) return;
    enemigoRespondio = true;

    int chance = QRandomGenerator::global()->bounded(1, 101); // 1..100
    bool acierta = (chance <= 80); // 80% acierto
    char respuesta;

    if (acierta)
        respuesta = preguntaActual.respuestaCorrecta;
    else {
        QString opciones = "ABCD";
        do {
            respuesta = opciones[QRandomGenerator::global()->bounded(0, 4)].toLatin1();
        } while (respuesta == preguntaActual.respuestaCorrecta);
    }

    qDebug() << "IA responde:" << respuesta << (acierta ? "(Correcto)" : "(Incorrecto)");

    if (acierta) {
        jugador->PerderCorazones();
        ActualizarCorazones();
        mostrarResultadoTemporal(false, true, 1); // IA acierta
    } else {
        mostrarResultadoTemporal(true, false, 1); // IA falla
    }

    verificarVictoria();
}


void Combate::mostrarResultadoTemporal(bool reiniciar, bool acierto, int tipoRespuesta) {
    QString texto;
    QString colorBorde;
    QString colorFondo;
    QString colorTexto;

    // --- Determinar mensaje y colores ---
    if (acierto) {
        texto = (tipoRespuesta == 0) ? "¡Correcto!" : "¡El enemigo acertó!";
        colorTexto = "#fdf5e6"; // blanco antiguo
        colorBorde = (tipoRespuesta == 0) ? "#8fce00" : "#ffb347"; // verde o dorado
        colorFondo = (tipoRespuesta == 0)
                         ? "qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(90,140,60,220), stop:1 rgba(50,90,40,220))"
                         : "qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(160,110,40,220), stop:1 rgba(100,70,30,220))";
    } else {
        texto = (tipoRespuesta == 0) ? "¡Error!" : "¡El enemigo falló!";
        colorTexto = "#fff0f0";
        colorBorde = "#b22222"; // rojo oscuro
        colorFondo = "qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(100,20,20,220), stop:1 rgba(60,10,10,220))";
    }

    // --- Mostrar mensaje temporal ---
    labelPregunta->setStyleSheet(QString(
                                     "background: %1;"
                                     "color: %2;"
                                     "border: 3px solid %3;"
                                     "border-radius: 10px;"
                                     "padding: 10px;"
                                     "font: bold 20px 'Papyrus';"
                                     "text-align: center;"
                                     ).arg(colorFondo, colorTexto, colorBorde));
    labelPregunta->setText(texto);

    // --- Si acierta: avanzar pregunta ---
    if (acierto) {
        QTimer::singleShot(1500, this, [this]() {
            if (!termino) {
                labelPregunta->setStyleSheet(
                    "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
                    "stop:0 rgba(120,100,70,220), stop:1 rgba(80,60,40,220));"
                    "color: #fdf5e6;"
                    "border: 3px solid #d4af37;"
                    "border-radius: 12px;"
                    "padding: 10px;"
                    "font: bold 18px 'Papyrus';"
                    );
                cargarPreguntaActual();
                iniciarTurno();
            }
        });
    }
    // --- Si falla: volver a mostrar la misma pregunta ---
    else {
        QTimer::singleShot(1500, this, [this]() {
            if (!termino) {
                labelPregunta->setStyleSheet(
                    "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
                    "stop:0 rgba(120,100,70,220), stop:1 rgba(80,60,40,220));"
                    "color: #fdf5e6;"
                    "border: 3px solid #d4af37;"
                    "border-radius: 12px;"
                    "padding: 10px;"
                    "font: bold 18px 'Papyrus';"
                    );
                // Volvemos a mostrar la misma pregunta actual
                labelPregunta->setText(preguntaActual.texto);
                labelRespuestas->show();
                iniciarTurno();
            }
        });
    }
}


void Combate::verificarVictoria() {
    if (CorazonesEnemigo <= 0) {
        termino = true;
        labelPregunta->setText("¡Has ganado el combate!");
        ActualizarCorazones(true);
        timerRespuesta->stop();
    } else if (jugador->getCorazones() <= 0) {
        termino = true;
        labelPregunta->setText("Has sido derrotado...");
        timerRespuesta->stop();
    }
}

void Combate::ActualizarCorazonesEnemigos() {
    for (int i = 0; i < 4; ++i) {
        iconosCorazonesEnemigo[i]->setPixmap(QPixmap(
                                                 (i < CorazonesEnemigo) ? "Sprites/Recursos/FullHeartE.png" : "Sprites/Recursos/NullHeart.png"
                                                 ).scaled(32, 32));
    }
}

void Combate::SalirMinijuego() {

}

// --- Vacíos obligatorios ---
void Combate::onMovimientoUpdate() {}
void Combate::detectarZonaPuerta() {}
void Combate::mostrarHintPuerta(const QRect&) {}
void Combate::ocultarHintPuerta() {}

void Combate::keyPressEvent(QKeyEvent* event) { ControlPersonaje::keyPressEvent(event); }
