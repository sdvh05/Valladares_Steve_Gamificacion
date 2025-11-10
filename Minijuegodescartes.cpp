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
    this->setWindowTitle("Rene Descartes");

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
    npcDescartes->move(640,670);
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
                <<"Usa las teclas 1,2,3,4 \n para Responder"
                << "Mucha Suerte";
        break;
    case 1:
        dialogo << "Respondiste Correctamente!"
                << "Con cada respuesta correcta"
                << "Aparecera una pista Nueva"
                << "Ve a leer el caso \n y la EVIDENCIA";
        break;
    case 2:
        dialogo << "Ahora tienes una \n Segunda Pista"
                <<"Te doy brindo un ANÁLISIS"
                <<"Sobre esta compleja \n situacion";

        break;
    case 3:
        dialogo << "Muy bien!"
                <<"Te has Ganado mas Informacion"
                <<"Una SÍNTESIS sobre el caso";

         break;
    case 4:
        dialogo <<"Te Prestare Mi cuarderno"
                <<"Enumerando diversas \n conclusiones"
                <<"A ver si te es de Uso\n para responder el Dilema";

         break;
    case 5:
        dialogo <<"Ya tienes todas las \n Pistas Disponibles"
                <<"Es tu turno de contestarme..."
                <<"¿Cual es la decision \n que deben tomar?";

         break;

    default:
         dialogo <<"No hay pistas disponibles en esta etapa.";
         break;
    }

    npcDescartes->setDialogos(dialogo);
}

void MinijuegoDescartes::responderAlDilema() {
    labelPregunta->setText("🧪 DILEMA FINAL:\n¿Debo revelar una falta grave que conozco, aunque eso perjudique a alguien cercano?");
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
        "<font color='white'>A: Ocultar la falta para proteger el vínculo personal</font><br>"
        "<font color='white'>B: Revelar la falta solo si hay evidencia irrefutable</font><br>"
        "<font color='white'>C: Consultar primero con la persona involucrada</font><br>"
        "<font color='white'>D: Revelar la verdad, priorizando el deber moral</font>"
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
        texto =
            "🧠 <b>Mesa 1: EVIDENCIA</b><br><br>"
                "🧪 DILEMA: ¿Debo revelar una falta grave que conozco, aunque eso perjudique a alguien cercano?</b><br><br>"
            "Elementos disponibles:<br>"
            "&nbsp;&nbsp;- Una nota anónima con la confesión de una falta<br>"
            "&nbsp;&nbsp;- El vínculo afectivo con la persona involucrada<br>"
            "&nbsp;&nbsp;- El impacto potencial en otros si se revela<br>"
            "&nbsp;&nbsp;- El principio de sinceridad<br><br>"
            "📍<i>Pista 1 (Regla de la evidencia):</i><br>"
            "<i>“No aceptar nada como verdadero sin evidencia clara. "
            "¿La falta es real y verificable, o solo una sospecha?”</i>";
        break;

    case 2:
        texto =
            "🔍 <b>Mesa 2: ANÁLISIS</b><br><br>"
            "Ejercicios de inferencia:<br>"
            "&nbsp;&nbsp;- ¿Qué consecuencias tendría revelar la falta?<br>"
            "&nbsp;&nbsp;- ¿Qué consecuencias tendría ocultarla?<br>"
            "&nbsp;&nbsp;- ¿La nota es confiable?<br>"
            "&nbsp;&nbsp;- ¿El vínculo personal afecta tu juicio?<br><br>"
            "📍<i>Pista 2 (Regla del análisis):</i><br>"
            "<i>“Dividir cada dificultad en partes. "
            "¿Qué está en juego: la verdad, el daño, el deber, la confianza?”</i>";
        break;

    case 3:
        texto =
            "🧩 <b>Mesa 3: SÍNTESIS</b><br><br>"
            "Relaciones establecidas:<br>"
            "&nbsp;&nbsp;- Veracidad vs. lealtad<br>"
            "&nbsp;&nbsp;- Daño emocional vs. justicia<br>"
            "&nbsp;&nbsp;- Intención vs. resultado<br>"
            "&nbsp;&nbsp;- Universalidad de la acción<br><br>"
            "📍<i>Pista 3 (Regla de la síntesis):</i><br>"
            "<i>“Conducir los pensamientos por orden. "
            "¿Qué principio puede aplicarse a todos sin contradicción?”</i>";
        break;

    case 4:
        texto =
            "📋 <b>Mesa 4: ENUMERACIÓN</b><br><br>"
            "Orden de conceptos para concluir:<br>"
            "&nbsp;&nbsp;- La falta parece real y comprobable.<br>"
            "&nbsp;&nbsp;- Ocultarla protegería a alguien, pero violaría el principio de sinceridad.<br>"
            "&nbsp;&nbsp;- Revelarla podría causar daño, pero también restaurar justicia.<br>"
            "&nbsp;&nbsp;- El deber moral exige actuar según principios universales, no intereses personales.<br><br>"
            "📍<i>Pista 4 (Regla de la enumeración)</i>";
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
    QRect zonaFinal(-28,408,50,50);


    if (rectJugador.intersects(zonaFinal) && termino) {
        SalirMinijuego();
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

        QTimer::singleShot(1000, this, [=]() {
            labelPregunta->setText(preguntaActual.texto);
        });
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
                 termino=true;
                //QTimer::singleShot(2500, this, [=]() { SalirMinijuego(); });

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

    Interior* interior = new Interior(jugador,nullptr,10);
    jugador->move(524,786);
    interior->show();
    inventarioGlobal->agregarMedallaDescartes();



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
    //EstadoActual++; //esto es solo para probar pq aun no implemento que cambie dentro del juego



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

