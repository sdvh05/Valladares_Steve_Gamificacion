#include "Minijuegodescartes.h"
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

MinijuegoDescartes::MinijuegoDescartes(Personaje* jugadorExistente, QWidget* parent, int EstadoActual)
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


    jugador->move(72,498);

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
    preguntas.cargarPreguntasPorTipo("Descartes");
    cargarPreguntaActual();


    Movimientos();

    npcDescartes = new Npc(this);
    npcDescartes->setNombre("Descartes");
    npcDescartes->move(670,670);
    npcDescartes->show();
    npcDescartes->raise();

    auto datos = npcDescartes->obtenerAnimacion("idle");
    npcDescartes->SetAnimacion(datos.ruta, 6);

    // Diálogo del NPC
    actualizarDialogoDescartes();


}

void MinijuegoDescartes::actualizarDialogoDescartes(){
    QStringList dialogo;

    switch (EstadoActual) {
    case 0:
        dialogo << "Bienvenido al Laboratorio"
                << "Aqui Seras \n presentado con:"
                << "Preguntas tal como en \n los Minijuegos"
                << "Y un dilema a resolver"
                <<"Usa las teclas 1,2,3,4";
        break;
    case 1:
        dialogo << "Respondiste Correctamente!"
                << "Con cada respuesta correcta"
                << "Aparecera una pista Nueva";
        break;
    case 2:

        break;
    case 3:

         break;
    case 4:

         break;
    case 5:

         break;

    default:
        dialogo << "Hola";
         break;
    }

    npcDescartes->setDialogos(dialogo);
}

void MinijuegoDescartes::responderAlDilema() {
    labelPregunta->setText("🧩 DILEMA FINAL:\n¿Cuál método realmente mejora el aprendizaje a largo plazo?");
    labelPregunta->setStyleSheet(
        "background: rgba(40,40,40,220); color: white; font: bold 16px 'Courier New'; "
        "border: 2px solid white; border-radius: 8px; padding: 10px;");
    labelPregunta->show();

    labelRespuestas->show();
    labelRespuestas->setStyleSheet(
        "background: rgba(30,30,30,220); color: white; font: 14px 'Courier New'; "
        "border: 2px solid gray; border-radius: 8px; padding: 10px;");
    labelRespuestas->setTextFormat(Qt::RichText);
    labelRespuestas->setText(
        "<font color='white'>A: Leer y subrayar</font><br>"
        "<font color='white'>B: Hacer mapas mentales</font><br>"
        "<font color='white'>C: Explicar en voz alta</font><br>"
        "<font color='white'>D: Técnica de recuperación activa</font>"
        );

    respuestasActivas = true;

    EstadoActual = 5;
    actualizarDialogoDescartes();
}



void MinijuegoDescartes::MostrarPistas(int Etapa) {
    QLabel* pagina = new QLabel(this);
    pagina->setStyleSheet(
        "background: rgba(250,250,250,230); color: black; border: 3px solid gray;"
        "border-radius: 10px; font: 14px 'Courier New'; padding: 18px;");
    pagina->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    pagina->setWordWrap(true);
    pagina->setGeometry(60, 180, 760, 300);
    pagina->setTextFormat(Qt::RichText);
    pagina->raise();
    pagina->show();

    // Botón "X"
    QPushButton* cerrar = new QPushButton("✕", this);
    cerrar->setGeometry(pagina->x() + pagina->width() - 35, pagina->y() + 10, 30, 30);
    cerrar->setStyleSheet(
        "background: rgb(200,0,0); color: white; font: bold 16px 'Arial'; "
        "border: none; border-radius: 8px;"
        "hover { background: rgb(255,50,50); }");
    cerrar->raise();
    cerrar->show();

    connect(cerrar, &QPushButton::clicked, [=]() {
        pagina->deleteLater();
        cerrar->deleteLater();
    });

    QString texto;
    switch (Etapa) {
    case 1:
        texto = "📚 <b>Mesa 1: EVIDENCIA</b><br><br>"
                "&nbsp;&nbsp;- Leer y subrayar<br>"
                "&nbsp;&nbsp;- Hacer mapas mentales<br>"
                "&nbsp;&nbsp;- Explicar en voz alta<br>"
                "&nbsp;&nbsp;- Técnica de recuperación activa (hacerte preguntas sin ver el material)<br><br>"
                "Dilema: El estudiante debe decidir cuál método realmente mejora el aprendizaje a largo plazo.";
        break;
    case 2:
        texto = "🔍 <b>Mesa 2: ANÁLISIS</b><br><br>"
                "Ejercicios de inferencia:<br>"
                "&nbsp;&nbsp;- Leer y subrayar = fácil, pero pasivo.<br>"
                "&nbsp;&nbsp;- Mapas mentales = organizan ideas, pero no garantizan retención.<br>"
                "&nbsp;&nbsp;- Explicar en voz alta = mejora comprensión, pero depende del nivel previo.<br>"
                "&nbsp;&nbsp;- Recuperación activa = obliga al cerebro a recordar, fortalece la memoria y detecta lagunas.";
        break;
    case 3:
        texto = "🧠 <b>Mesa 3: SÍNTESIS</b><br><br>"
                "Relaciones:<br>"
                "&nbsp;&nbsp;- Pasivo vs. activo.<br>"
                "&nbsp;&nbsp;- Organización vs. retención.<br>"
                "&nbsp;&nbsp;- Comprensión vs. memoria duradera.";
        break;
    case 4:
        texto = "📋 <b>Mesa 4: ENUMERACIÓN</b><br><br>"
                "Conclusión ordenada:<br>"
                "&nbsp;&nbsp;- Leer y subrayar ayuda a identificar ideas, pero no garantiza que se recuerden.<br>"
                "&nbsp;&nbsp;- Mapas mentales son útiles para visualizar, pero no prueban el conocimiento.<br>"
                "&nbsp;&nbsp;- Explicar en voz alta mejora la comprensión, pero no siempre detecta errores.<br>"
                "&nbsp;&nbsp;- Recuperación activa exige recordar sin ayuda, lo que fortalece el aprendizaje.";
        break;
    default:
        texto = "No hay pistas disponibles en esta etapa.";
        break;
    }

    pagina->setText(texto);
}




void MinijuegoDescartes::configurarEscena(){
    cambiarEscena();
    fondoLabel->lower();
    fondoLabel->show();
}

void MinijuegoDescartes::cambiarEscena() {
    QString nombreArchivoBase = "Sprites/Castle/Minijuegos/Descartes/Descartes ";

    QString Estado = "";

    switch (this->EstadoActual) {
    case 0:
        Estado ="0";
        break;
    case 1:
        Estado ="1";
        break;
    case 2:
        Estado ="2";
        break;
    case 3:
        Estado ="3";
        break;
    case 4:
        Estado= "4";
        break;
    case 5:
        Estado= "5";
        break;
    default: Estado ="0";
    }


    // Crear nombre del archivo tipo "AB Wrong.png"
    QString ruta = nombreArchivoBase + Estado + ".png";

    QPixmap fondo(ruta);
    if (fondo.isNull()) {
        qDebug() << "Error al cargar:" << ruta ;
        return;
    }
    qDebug() << ruta ;
    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}


void MinijuegoDescartes::configurarObstaculos(){
    obstaculos.clear();

    /*obstaculos.append(QRect(50,734, 2, 320));
    obstaculos.append(QRect(206,734, 2, 320));
    obstaculos.append(QRect(532,734, 2, 320));
    obstaculos.append(QRect(696,734, 2, 320));
    obstaculos.append(QRect(50,734, 2, 320)); */

    \
}





void MinijuegoDescartes::onMovimientoUpdate() {
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



    // --- Final ---
    QRect zonaFinal(222,200,200,100);

    if (rectJugador.intersects(zonaFinal) && termino) {
        zonaTermino = true;
    }

    if (rectJugador.intersects(zonaFinal) && !termino) {
        jugador->move(400,818);
        ResetearMovimiento();
        puentes = {true, true, true, true};
        cambiarEscena();
        configurarObstaculos();
        actualizarRespuestas();



        respuestasActivas = false;
        labelRespuestas->hide();

        // Si no quedan preguntas, terminar el minijuego
        if (preguntas.isEmpty()) {
            qDebug() << "Se terminó el minijuego";
            ActualizarCorazones(ganaste);
            if(ganaste){
                inventarioGlobal->agregarMedallaDescartes();
            }
            termino = true;
        }

        cargarPreguntaActual();
        this->update();
    }

}


void MinijuegoDescartes::detectarZonaPuerta() {
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


void MinijuegoDescartes::mostrarHintPuerta(const QRect& zonaPuerta) {
}


void MinijuegoDescartes::ocultarHintPuerta() {
    labelPuerta->hide();
}

void MinijuegoDescartes::VerificarRespuesta(char letra)
{
    if (preguntaActual.texto.isEmpty()) return;


    bool correcta = (letra == preguntaActual.respuestaCorrecta);

    if (correcta) {
        qDebug() << "Respuesta correcta:" << letra;
        labelPregunta->setText("✅ ¡Correcto!");
        ActualizarCorazones();

        QTimer::singleShot(1000, this, [=]() {
            if (ultimaPregunta) {
                responderAlDilema();
            } else {
                ActualizarPregunta();
            }
        });
    } else {
        qDebug() << "Respuesta incorrecta:" << letra;
        labelPregunta->setText("❌ Incorrecto...");
        ActualizarCorazones(false);
    }
}



void MinijuegoDescartes::keyPressEvent(QKeyEvent* event) {
    ControlPersonaje::keyPressEvent(event);
    QRect rectJugador = jugador->geometry();

    if (event->key() == Qt::Key_Z && !preguntaActual.texto.isEmpty()) {
        respuestasActivas = !respuestasActivas;

        if (respuestasActivas) {
            actualizarRespuestas();
            labelRespuestas->show();
        } else {
            labelRespuestas->hide();
        }
    }

    if(event->key() == Qt::Key_Q){
        if (rectJugador.intersects(QRect(104,704, 15, 100)) && EstadoActual >=1) {
            qDebug() << "pista 1";
            this->MostrarPistas(1);
        } else if (rectJugador.intersects(QRect(438,656, 15, 100)) && EstadoActual >=2) {
            qDebug() << "pista 2";
            this->MostrarPistas(2);
        } else if (rectJugador.intersects(QRect(126,852, 15, 100)) && EstadoActual >=3) {
            qDebug() << "pista 3";
            this->MostrarPistas(3);
        } else if (rectJugador.intersects(QRect(480,802, 15, 100)) && EstadoActual >=4) {
            qDebug() << "pista 4";
            this->MostrarPistas(4);
        }

    }

    if (event->key() == Qt::Key_E && npcDescartes) {
        npcDescartes->Interactuar(jugador);
    }
    if(event->key() == Qt::Key_E && npcDescartes && EstadoActual>=5 && rectJugador.intersects(QRect(670,670, 150, 150))){
        //
    }


    //event eltras A,B,C,D para responder -> mandar las respuestas.
    if (respuestasActivas && EstadoActual < 5) {
        if (event->key() == Qt::Key_1) { VerificarRespuesta('A'); }
        if (event->key() == Qt::Key_2) { VerificarRespuesta('B'); }
        if (event->key() == Qt::Key_3) { VerificarRespuesta('C'); }
        if (event->key() == Qt::Key_4) { VerificarRespuesta('D'); }
    }
    else if (EstadoActual == 5) {
        if (event->key() == Qt::Key_1 || event->key() == Qt::Key_3 ||
            event->key() == Qt::Key_2 || event->key() == Qt::Key_4) {

            // Correcta = D
            if (event->key() == Qt::Key_4) {
                labelPregunta->setText("¡Correcto! Has resuelto el dilema de Descartes.");
                QTimer::singleShot(2500, this, [=]() { SalirMinijuego(); });
            } else {
                labelPregunta->setText("Descartes dice: 'Piénsalo bien... el conocimiento no se memoriza, se construye.'");
            }
        }
    }


}


void MinijuegoDescartes::actualizarRespuestas() {
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

void MinijuegoDescartes::ActualizarPregunta(){
    if (preguntas.isEmpty()) {
        responderAlDilema();
        return;
    }

    cargarPreguntaActual();
    EstadoActual++;
    qDebug() <<EstadoActual;
    cambiarEscena();
    actualizarDialogoDescartes();
}


void MinijuegoDescartes::SalirMinijuego(){
    if (!jugador) return;

    ResetearMovimiento();

    Interior* interior = new Interior(jugador,nullptr,4);
    jugador->move(734,568); //cambair este mocer y el parametro int
    interior->show();


    this->close();
}




void MinijuegoDescartes::actualizarEstado(int indice) {

    configurarEscena();
    configurarObstaculos();
    this->update();
}


void MinijuegoDescartes::mousePressEvent(QMouseEvent* event)
{
    //qDebug() << "Coordenadas del click: " << event->pos();
    qDebug() << "Jugador en:" << jugador->pos();
    qDebug() << "Respuesta: " <<preguntaActual.respuestaCorrecta;
    ResetearMovimiento();

    this->ActualizarCorazones(true);
    qDebug() << "Vidas:" << jugador->getCorazones();
    qDebug() <<npcDescartes;
    EstadoActual++; //esto es solo para probar pq aun no implemento que cambie dentro del juego



}

void MinijuegoDescartes::cargarPreguntaActual() {
    if (!preguntas.isEmpty()) {
        preguntaActual = preguntas.dequeue();
        labelPregunta->setText(preguntaActual.texto);
        labelRespuestas->hide();

        ultimaPregunta = preguntas.isEmpty();
    } else {
        qDebug() << "No hay más preguntas en la cola.";
        labelPregunta->setText("¡Has Completado el Minijuegos!");
        labelRespuestas->hide();

        ultimaPregunta =true;
    }
}

