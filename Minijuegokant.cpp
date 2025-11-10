#include "MinijuegoKant.h"
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

MinijuegoKant::MinijuegoKant(Personaje* jugadorExistente, QWidget* parent, int EstadoActual)
    : ControlPersonaje(jugadorExistente, parent), EstadoActual(EstadoActual)
{
    this->resize(848, 1200);
    this->setWindowTitle("Immanuel Kant");

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
    preguntas.cargarPreguntasPorTipo("Kant");
    cargarPreguntaActual();


    Movimientos();

    npcKant = new Npc(this);
    npcKant->setNombre("Kant");
    npcKant->move(678,416);
    npcKant->miradoDerecha=false;
    npcKant->show();
    npcKant->raise();

    auto datos = npcKant->obtenerAnimacion("idle");
    npcKant->SetAnimacion(datos.ruta, 6);

    // Diálogo del NPC
    actualizarDialogoKant();


}

void MinijuegoKant::actualizarDialogoKant(){
    QStringList dialogo;

    switch (EstadoActual) {
    case 0:
        dialogo << "Bienvenido al Aula"
                << "Alrededor del Aula \n hay Pistas escondidas"
                <<"Estas te ayudaran a \n contestar las preguntas"
                <<"Busca en los diferentes \n objetos que hay \n en el Aula"
                <<"Estos 2 libros no son pistas: \n pero igual contienen conocimiento sobre  \n la Crítica de la razón ";

        break;
    case 1:
        dialogo << "Recuerda que las pistas"
                << "Van Acorde al numero de \n la pregunta";
        break;
    case 2:
        dialogo << "Aprendes Rapido"
                <<"Continua asi y saldras \n lleno de conocimiento";
        break;
    case 3:
        dialogo << "Encuentra todas las pistas"
                <<"Te hara la vida mas facil";
        break;
    case 4:
        dialogo << "¿Has leigo los libros?"
                <<"¿Que te parecieron?";
        break;
    case 5:
        dialogo <<"Ya casi llegas ahi"
                <<"Sigue Adelante casi completas \n todas las preguntas.";
        break;

    case 6:
        dialogo <<"EXCELENTE!!"
                <<"Han comprendido. El deber moral no es ciego ni frío"
                <<"es racional, universal y humano.”";
        break;
    default:
        dialogo << "Excelente"
            << "Si buscas el conocimiento \n las pistas te guiaran \n a la verdad";
        break;
    }

    npcKant->setDialogos(dialogo);
}

/*
void MinijuegoKant::responderAlDilema() {
    labelPregunta->setText("🧩 DILEMA FINAL:\n Ahora que han recorrido el camino de la razón, deben decidir \n ¿Qué deben hacer con el secreto que conocen?");
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
        "<font color='white'>A: Revelarlo, porque la verdad siempre debe decirse.</font><br>"
        "<font color='white'>B: Ocultarlo, porque el daño sería mayor que el beneficio.</font><br>"
        "<font color='white'>C: Revelarlo solo si la acción respeta la dignidad de todos los involucrados. </font><br>"
        "<font color='white'>D: Técnica de recuperación activa</font>"
        );

    respuestasActivas = true;

    EstadoActual = 5;
    actualizarDialogoKant();
}*/



void MinijuegoKant::MostrarPistas(int Etapa) {
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
    case 7:
        texto = "📗 <b> Crítica de la razón pura (Conocimiento)</b><br><br>"
                "&nbsp;&nbsp;- El conocimiento comienza con la experiencia, pero no todo proviene de ella. La razón impone formas a lo que los sentidos captan.<br>"
                "&nbsp;&nbsp;- Antes de juzgar, pregúntate: ¿lo que sabes es realmente conocimiento, o solo percepción sin fundamento?<br>"
                "&nbsp;&nbsp;- La verdad no se encuentra en lo que se ve, sino en lo que puede ser pensado con claridad.<br>";

        break;
    case 8:
        texto = "📘 <b>Crítica de la razón práctica (Ética)</b><br><br>"
                "&nbsp;&nbsp;- Obra de tal modo que tu acción pueda convertirse en ley universal. El deber no depende del resultado, sino de la intención.<br>"
                "&nbsp;&nbsp;- La moral no es una emoción ni una conveniencia: es respeto por la dignidad del otro.<br>"
                "&nbsp;&nbsp;- Pregúntate: ¿actuarías igual si todos lo hicieran? ¿Tu decisión respeta la humanidad en cada persona involucrada?<br>";
        break;

    case 1:
        texto = "📘 <b>Pista 1:</b><br><br>"
                "La respuesta debe ser un solo mandato; busca el que depende de un fin. "
                "Piensa en la forma práctica: “Si quieres X, entonces haz Y”.";
        break;

    case 2:
        texto = "📘 <b>Pista 2:</b><br><br>"
                "Nouménico — realidad inaccesible.<br>"
                "Fenoménico — apariencia sensible (acceso por sentidos).<br>"
                "Empírico — datos observables.<br>"
                "Racional — principios razonados.";
        break;

    case 3:
        texto = "📘 <b>Pista 3:</b><br><br>"
                "“No esperes que otros piensen por ti; usa tu propia razón y atrévete a cuestionar.”";
        break;

    case 4:
        texto = "📘 <b>Pista 4:</b><br><br>"
                "“Como Copérnico movió el centro en el cielo, Kant mueve el centro del saber: "
                "ahora el que conoce (el sujeto) importa primero.”";
        break;

    case 5:
        texto = "📘 <b>Pista 5:</b><br><br>"
                "Criticar aquí es ‘examinar con cuidado’. Kant no elige solo experiencia ni solo razón, "
                "sino que los examina juntos.";
        break;

    case 6:
        texto = "📘 <b>Pista 6:</b><br><br>"
                "Busca la obra que es teórica, que investiga cómo funciona la razón en sí misma.";
        break;

    default:
        texto = "No hay pistas disponibles en esta etapa.";
        break;
    }

    pagina->setText(texto);
}




void MinijuegoKant::configurarEscena(){
    cambiarEscena();
    fondoLabel->lower();
    fondoLabel->show();
}

void MinijuegoKant::cambiarEscena() {



    QString ruta = "Sprites/Castle/Minijuegos/Kant/Kant.png";

    QPixmap fondo(ruta);
    if (fondo.isNull()) {
        qDebug() << "Error al cargar:" << ruta ;
        return;
    }
    qDebug() << ruta ;
    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}


void MinijuegoKant::configurarObstaculos(){
    obstaculos.clear();

    obstaculos.append(QRect(264,250, 100, 5));


    \
}





void MinijuegoKant::onMovimientoUpdate() {
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
    QRect zonaFinal(-68,554,10,500);

    if (rectJugador.intersects(zonaFinal) && termino) {
        SalirMinijuego();
    }

}


void MinijuegoKant::detectarZonaPuerta() {
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


void MinijuegoKant::mostrarHintPuerta(const QRect& zonaPuerta) {
}


void MinijuegoKant::ocultarHintPuerta() {
    labelPuerta->hide();
}

void MinijuegoKant::VerificarRespuesta(char letra)
{
    if (preguntaActual.texto.isEmpty()) return;

    bool correcta = (letra == preguntaActual.respuestaCorrecta);

    if (correcta) {
        qDebug() << "Respuesta correcta:" << letra;
        labelPregunta->setText("✅ ¡Correcto!");
        ActualizarCorazones();

        QTimer::singleShot(1000, this, [=]() {
            ActualizarPregunta();
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




void MinijuegoKant::keyPressEvent(QKeyEvent* event) {
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
        if (rectJugador.intersects(QRect(26,328, 15, 50)) ) {
            qDebug() << "pista 1";
            this->MostrarPistas(1);
        } else if (rectJugador.intersects(QRect(26,570, 15, 50)) ) {
            qDebug() << "pista 2";
            this->MostrarPistas(2);
        } else if (rectJugador.intersects(QRect(598,314, 15, 50)) ) {
            qDebug() << "pista 3";
            this->MostrarPistas(3);
        } else if (rectJugador.intersects(QRect(666,600, 15, 50)) ) {
            qDebug() << "pista 4";
            this->MostrarPistas(4);
        }
        else if (rectJugador.intersects(QRect(294,280, 15, 50)) ) {
            qDebug() << "pista 5";
            this->MostrarPistas(5);
        } else if (rectJugador.intersects(QRect(344,466, 15, 50)) ) {
            qDebug() << "pista 6";
            this->MostrarPistas(6);

        } else if (rectJugador.intersects(QRect(486,772, 15, 50)) ) { //libro critica razon pura
            qDebug() << "pista 7";
            this->MostrarPistas(7);
        } else if (rectJugador.intersects(QRect(158,808, 15, 50)) ) { //Critica Razon Practca
            qDebug() << "pista 8";
            this->MostrarPistas(8);
        }

    }

    if (event->key() == Qt::Key_E && npcKant) {
        npcKant->Interactuar(jugador);
    }



    //event eltras A,B,C,D para responder -> mandar las respuestas.
    if (respuestasActivas) {
        if (event->key() == Qt::Key_1) { VerificarRespuesta('A'); }
        if (event->key() == Qt::Key_2) { VerificarRespuesta('B'); }
        if (event->key() == Qt::Key_3) { VerificarRespuesta('C'); }
        if (event->key() == Qt::Key_4) { VerificarRespuesta('D'); }
    }
}


void MinijuegoKant::actualizarRespuestas() {
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

void MinijuegoKant::ActualizarPregunta(){
    cargarPreguntaActual();
    EstadoActual++;
    qDebug() <<EstadoActual;
    //cambiarEscena();
    actualizarDialogoKant();
}


void MinijuegoKant::SalirMinijuego(){
    if (!jugador) return;

    ResetearMovimiento();

    Interior* interior = new Interior(jugador,nullptr,10);
    jugador->move(524,786);
    interior->show();
    inventarioGlobal->agregarMedallaKant();


    this->close();
}




void MinijuegoKant::actualizarEstado(int indice) {

    configurarEscena();
    configurarObstaculos();
    this->update();
}


void MinijuegoKant::mousePressEvent(QMouseEvent* event)
{
    //qDebug() << "Coordenadas del click: " << event->pos();
    qDebug() << "Jugador en:" << jugador->pos();
    qDebug() << "Respuesta: " <<preguntaActual.respuestaCorrecta;
    ResetearMovimiento();

    //this->ActualizarCorazones(true);
    qDebug() << "Vidas:" << jugador->getCorazones();
    qDebug() <<npcKant;
    EstadoActual++; //esto es solo para probar pq aun no implemento que cambie dentro del juego



}

void MinijuegoKant::cargarPreguntaActual() {
    if (!preguntas.isEmpty()) {
        preguntaActual = preguntas.dequeue();
        labelPregunta->setText(preguntaActual.texto);
        labelRespuestas->hide();

        ultimaPregunta = preguntas.isEmpty();
    } else {
        qDebug() << "No hay más preguntas en la cola.";
        labelPregunta->setText("¡Has Completado Misterio de Kant!");
        labelRespuestas->hide();
        termino=true;
        ultimaPregunta =true;
    }
}
