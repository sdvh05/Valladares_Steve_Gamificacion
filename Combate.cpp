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

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QFile>



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
    jugador->move(116, 702);
    jugador->MoverDerecha();

    jugador->show();
    jugador->raise();

    corazonesPrevios=jugador->getCorazones();


    //NPC
    npc = new Npc(this);
    if (Bando==1)
        npc->setNombre("Racionalista");
    else
         npc->setNombre("Empirista");

    npc->move(620, 702);
    npc->miradoDerecha=false;


    npc->show();
    npc->raise();

    auto datos = npc->obtenerAnimacion("idle");
    npc->SetAnimacion(datos.ruta, datos.frames);


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
    bannerEmpirista->setStyleSheet(
        "background: rgba(255, 255, 255, 200);"      // blanco semitransparente
        "color: black;"
        "font: bold 18px 'Courier New';"
        "padding: 6px;"
        "border-radius: 6px;"
        "border: 2px solid #333;"                    // pequeño borde oscuro
        );

    bannerRacionalista = new QLabel("RACIONALISTAS", this);
    bannerRacionalista->setStyleSheet(
        "background: rgba(180, 0, 0, 180);"          // rojo oscuro semitransparente
        "color: white;"
        "font: bold 18px 'Courier New';"
        "padding: 6px;"
        "border-radius: 6px;"
        "border: 2px solid gold;"                    // toque dorado elegante
        );


    if(Bando==2){
        bannerEmpirista->setGeometry(20, 250+25, 180, 30);
        bannerRacionalista->setGeometry(width() - 200, 250+25, 180, 30);
    }else{
        bannerEmpirista->setGeometry(width() - 200, 250+25, 180, 30);
        bannerRacionalista->setGeometry(20, 250+25, 180, 30);
    }

    // --- Botones de respuesta ---
    int btnY = 900;
    int btnW = 180;
    int btnH = 50;

    // 🎨 Colores según el bando del jugador
    QString baseColor;
    QString borderColor;

    if (jugador->Bando == 1) { //Racionalista
        baseColor = "background: rgba(180,30,30,220); color: white;";
        borderColor = "border: 2px solid #ffaaaa;";
    }
    else if (jugador->Bando == 2) { //Empirista
        // Empiristas → tonos azulados
        baseColor = "background: rgba(40,80,180,220); color: white;";
        borderColor = "border: 2px solid #aaccff;";
    }
    else { //Caso Base
        baseColor = "background: rgba(50,50,50,200); color: white;";
        borderColor = "border: 2px solid gray;";
    }

    QString styleBtn = QString("%1 %2 font: bold 16px 'Courier New'; border-radius: 8px; padding: 4px;").arg(baseColor, borderColor);
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

void Combate::npcMostrarRespuesta(char letra) {
    if (!npc) return;
    // convertir char -> QString
    QString texto = QChar(letra);
    npc->MostrarBubbleText(texto);
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
    timerRespuesta->start(7000); // IA responde en 7 segundos
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
        AnimarAtaque(true, [=]() {
            verificarVictoria();
        });
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
    npcMostrarRespuesta(respuesta);


    if (acierta) {
        jugador->PerderCorazones();
        ActualizarCorazones();
        mostrarResultadoTemporal(false, true, 1); // IA acierta

        AnimarAtaque(false, [=]() {
            verificarVictoria();
        });
    } else {
        mostrarResultadoTemporal(true, false, 1); // IA falla
    }

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
        ActualizarCorazones();
        timerRespuesta->stop();
        ReproducirCancion(true);
        MostrarVictoria(true);
        inventarioGlobal->agregarCorona();
        inventarioGlobal->agregarMedallaCombate();
    } else if (jugador->getCorazones() <= 0) {
        termino = true;
        labelPregunta->setText("Has sido derrotado...");
        timerRespuesta->stop();
        ReproducirCancion(false);
        MostrarVictoria(false);
        inventarioGlobal->agregarMedallaCombate();
    }
}

void Combate::ActualizarCorazonesEnemigos() {
    for (int i = 0; i < 4; ++i) {
        iconosCorazonesEnemigo[i]->setPixmap(QPixmap(
                                                 (i < CorazonesEnemigo) ? "Sprites/Recursos/FullHeartE.png" : "Sprites/Recursos/NullHeart.png"
                                                 ).scaled(32, 32));
    }
}


void Combate::AnimarAtaque(bool jugadorAtaca, std::function<void()> onFinalizado)
{
    QWidget* atacante = jugadorAtaca ? static_cast<QWidget*>(jugador) : static_cast<QWidget*>(npc);
    QWidget* objetivo = jugadorAtaca ? static_cast<QWidget*>(npc) : static_cast<QWidget*>(jugador);

    int origenX = jugadorAtaca ? 116 : 620;
    int destinoX = jugadorAtaca ? 520 : 220;
    int y = 702;

    // Cambiar animaciones
    if (jugadorAtaca) {
        auto datos = jugador->obtenerAnimacion("walk");
        jugador->SetAnimacion(datos.ruta, datos.frames);
    } else {
        auto datos = npc->obtenerAnimacion("walk");
        npc->SetAnimacion(datos.ruta, datos.frames);
    }

    // Animación de movimiento (ir hacia el objetivo)
    QPropertyAnimation* mover = new QPropertyAnimation(atacante, "pos");
    mover->setDuration(600);
    mover->setStartValue(QPoint(origenX, y));
    mover->setEndValue(QPoint(destinoX, y));
    mover->setEasingCurve(QEasingCurve::InOutQuad);

    connect(mover, &QPropertyAnimation::finished, this, [=]() {
        // Cambiar animación a ataque
        if (jugadorAtaca) {
            auto datosAtk = jugador->obtenerAnimacion("melee");
            jugador->SetAnimacion(datosAtk.ruta, datosAtk.frames);

            auto datosHurt = npc->obtenerAnimacion("hurt");
            npc->SetAnimacion(datosHurt.ruta, datosHurt.frames);
        } else {
            auto datosAtk = npc->obtenerAnimacion("attack");
            npc->SetAnimacion(datosAtk.ruta, datosAtk.frames);

            auto datosHurt = jugador->obtenerAnimacion("hurt");
            jugador->SetAnimacion(datosHurt.ruta, datosHurt.frames);
        }

        // Esperar breve momento y volver
        QTimer::singleShot(700, this, [=]() {
            // Volver al punto original
            QPropertyAnimation* volver = new QPropertyAnimation(atacante, "pos");
            volver->setDuration(600);
            volver->setStartValue(QPoint(destinoX, y));
            volver->setEndValue(QPoint(origenX, y));
            volver->setEasingCurve(QEasingCurve::InOutQuad);

            connect(volver, &QPropertyAnimation::finished, this, [=]() {
                // Volver a animación "idle"
                if (jugadorAtaca) {
                    auto datosIdle = jugador->obtenerAnimacion("idle");
                    jugador->SetAnimacion(datosIdle.ruta, datosIdle.frames);
                    auto datosIdle2 = npc->obtenerAnimacion("idle");
                    npc->SetAnimacion(datosIdle2.ruta, datosIdle2.frames);
                } else {
                    auto datosIdle = npc->obtenerAnimacion("idle");
                    npc->SetAnimacion(datosIdle.ruta, datosIdle.frames);
                    auto datosIdle2 = jugador->obtenerAnimacion("idle");
                    jugador->SetAnimacion(datosIdle2.ruta, datosIdle2.frames);
                }

                if (onFinalizado)
                    onFinalizado();
            });

            volver->start(QAbstractAnimation::DeleteWhenStopped);
        });
    });

    mover->start(QAbstractAnimation::DeleteWhenStopped);
}





void Combate::SalirMinijuego()
{
    qDebug() << "Salir del combate";


    FadeOutMusica(1200); // 1.2 segundos de fade
    jugador->setCorazones(corazonesPrevios); //recuperar los corazones que perdimos
    ActualizarCorazones();
    if (!jugador) return;

    ResetearMovimiento();

    Interior* interior = new Interior(jugador, nullptr, 9);
    if(Bando==1)
        jugador->move(276,734);
    else
        jugador->move(686,734);

    interior->show();

    this->close();
}



// --- Vacíos obligatorios ---
void Combate::onMovimientoUpdate() {}
void Combate::detectarZonaPuerta() {}
void Combate::mostrarHintPuerta(const QRect&) {}
void Combate::ocultarHintPuerta() {}

void Combate::keyPressEvent(QKeyEvent* event) { ControlPersonaje::keyPressEvent(event); }



void Combate::ReproducirCancion(bool gano)
{
    QString rutaCancion;

    if (gano) {
        if (jugador->Bando == 1)
            rutaCancion = "Canciones/O Fortuna.mp3";
        else
            rutaCancion = "Canciones/Hallowed Be Thy Name.mp3";
    } else {
        if (jugador->Bando == 1)
            rutaCancion = "Canciones/Hallowed Be Thy Name.mp3";
        else
            rutaCancion = "Canciones/O Fortuna.mp3";
    }

    qDebug() << "Existe archivo:" << QFile::exists(rutaCancion) << rutaCancion;

    if (playerMusica) {
        playerMusica->stop();
        playerMusica->deleteLater();
        playerMusica = nullptr;
    }
    if (audioOutput) {
        audioOutput->deleteLater();
        audioOutput = nullptr;
    }

    audioOutput = new QAudioOutput(this);
    playerMusica = new QMediaPlayer(this);
    playerMusica->setAudioOutput(audioOutput);

    audioOutput->setVolume(0.5);
    playerMusica->setSource(QUrl::fromLocalFile(rutaCancion));
    playerMusica->play();

    qDebug() << "Reproduciendo canción:" << rutaCancion;
}


void Combate::FadeOutMusica(int duracionMs)
{
    if (!audioOutput || !playerMusica) return;

    QPropertyAnimation* fade = new QPropertyAnimation(audioOutput, "volume", this);
    fade->setDuration(duracionMs);
    fade->setStartValue(audioOutput->volume());
    fade->setEndValue(0.0);

    connect(fade, &QPropertyAnimation::finished, this, [this]() {
        if (playerMusica) {
            playerMusica->stop();
            playerMusica->deleteLater();
            playerMusica = nullptr;
        }
        if (audioOutput) {
            audioOutput->deleteLater();
            audioOutput = nullptr;
        }
    });

    fade->start(QAbstractAnimation::DeleteWhenStopped);
}


void Combate::MostrarVictoria(bool gano)
{
    // --- Ocultar botones de respuesta ---
    QList<QPushButton*> botones = {A, B, C, D};
    for (auto btn : botones)
        btn->hide();

    // --- Crear botón de salir ---
    QPushButton* salirBtn = new QPushButton("Salir del Combate", this);
    salirBtn->setGeometry(width()/2 - 100, 950, 200, 50);
    salirBtn->setStyleSheet(
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
        "stop:0 rgba(50,40,30,230), stop:1 rgba(100,80,50,230));"
        "color: white; border: 2px solid gold; font: bold 18px 'Papyrus'; border-radius: 10px;"
        );
    salirBtn->show();
    connect(salirBtn, &QPushButton::clicked, this, &Combate::SalirMinijuego);

    // --- Determinar ganador y mensaje ---
    QString mensaje;
    QString colorFondo;
    QString bandoTexto = (jugador->Bando == 1) ? "Racionalistas" : "Empiristas";

    if (gano) {
        mensaje = QString("🏆 ¡Victoria de los %1! 🏆").arg(bandoTexto);
        colorFondo = (jugador->Bando == 1)
                         ? "background: rgba(180, 0, 0, 180);"   // Racionalistas (rojo)
                         : "background: rgba(240, 240, 255, 200);"; // Empiristas (blanco azulado)

        // Mover al jugador ganador al centro
        jugador->move(365, 372);

        // Mostrar corona sobre el jugador
        QLabel* corona = new QLabel(this);
        corona->setPixmap(QPixmap("Sprites/Recursos/corona.png").scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        corona->move(jugador->x() + 34, jugador->y() - 30);
        corona->show();
        corona->raise();

    } else {
        // Perdió el jugador: gana el oponente
        QString bandoOponente = (jugador->Bando == 1) ? "Empiristas" : "Racionalistas";
        mensaje = QString("💀 Los %1 han ganado... 💀").arg(bandoOponente);
        colorFondo = (jugador->Bando == 1)
                         ? "background: rgba(240, 240, 255, 200);"  // empiristas
                         : "background: rgba(180, 0, 0, 180);";      // racionalistas


        // Mover NPC al centro
        npc->move(376, 372);

        QLabel* corona = new QLabel(this);
        corona->setPixmap(QPixmap("Sprites/Recursos/corona.png").scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        corona->move(npc->x() + 20, npc->y() - 40);
        corona->show();
        corona->raise();
    }

    // --- Reutilizar labelPregunta para mostrar mensaje ---
    labelPregunta->setStyleSheet(QString(
                                     "%1 color: #fdf5e6; border: 3px solid gold; border-radius: 12px; "
                                     "padding: 10px; font: bold 22px 'Papyrus'; text-align: center;"
                                     ).arg(colorFondo));
    labelPregunta->setText(mensaje);
}

