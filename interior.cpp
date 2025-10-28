#include "Interior.h"
#include "Frontview.h"
#include "Ruleta.h"

#include "Minijuegohistoria.h"
#include "Minijuegociencia.h"

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

Interior::Interior(Personaje* jugadorExistente, QWidget* parent, int PasilloActual)
    : ControlPersonaje(jugadorExistente, parent), pasilloActual(PasilloActual)
{
    this->resize(1000, 1200);
    this->setWindowTitle("Interior del Castillo");

    fondoLabel = new QLabel(this);
    fondoLabel->setGeometry(0, 0, width(), height());
    fondoLabel->lower();

    configurarEscena();
    configurarObstaculos();
    ActualizarCorazones();

    jugador = jugadorExistente;

    jugador->setParent(this);

    if(pasilloActual == 1)
        jugador->move(400,818);

    jugador->show();
    jugador->raise();


    // --- LABEL DE PUERTA ---
    labelPuerta = new QLabel("Presiona Q para salir", this);
    labelPuerta->setStyleSheet(
        "background: rgba(0,0,0,180); color: white; padding: 6px; "
        "border-radius: 6px; font: bold 14px 'Courier New';"
        );
    labelPuerta->setAlignment(Qt::AlignCenter);
    labelPuerta->setFixedSize(220, 30);
    labelPuerta->hide();

    Movimientos();
}

void Interior::configurarEscena(){
    int pasillo=pasilloActual;
    QString rutaFondo;

    switch (pasillo) {
    case 1: //entrada
        rutaFondo = "Sprites/Castle/Interior.jpg";
        break;
    case 2: //izquierda
        rutaFondo = "Sprites/Castle/Pasillo.jpg";
        break;
    case 3: //derecha
        rutaFondo = "Sprites/Castle/.jpg";
        break;

    case 4: //Ruleta
        rutaFondo = "Sprites/Castle/Ruleta/Ruleta.png";
        break;
    case 5: //Ruleta Arte
        rutaFondo = "Sprites/Castle/Ruleta/ArteOpen.png";
        break;
    case 6: //Ruleta Politica
         rutaFondo = "Sprites/Castle/Ruleta/PoliticaOpen.png";
        break;
    case 7: //Ruleta Ciencia
         rutaFondo = "Sprites/Castle/Ruleta/CienciaOpen.png";
        break;
    case 8: //Ruleta Historia
         rutaFondo = "Sprites/Castle/Ruleta/HistoriaOpen.png";
        break;
    case 9:

        break;

    default:
        rutaFondo = "Sprites/Castle/Interior.jpg";
        break;
    }

    QPixmap fondo(rutaFondo);
    if (fondo.isNull()) {
        qDebug() << "Error al cargar el fondo:" << rutaFondo;
        return;
    }

    fondoLabel->setPixmap(fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    fondoLabel->lower();
    fondoLabel->show();

}

void Interior::configurarObstaculos(){
    obstaculos.clear();
    int pasillo=pasilloActual;

    switch (pasillo) {
    case 1:
        obstaculos.append(QRect(-30, 770, 1000, 4)); // suelo
        break;

    case 2:

        break;

    case 3:

        break;

    case 4:
        obstaculos.append(QRect(140, 442, 1000, 4)); //pared puerta arriba
        obstaculos.append(QRect(490,650, 20, 4)); //ruleta
        break;
    }
}





void Interior::onMovimientoUpdate()
{

    QRect rectJugador = jugador->geometry();
    detectarZonaPuerta();

    if (rectJugador.intersects(zonaPuerta)) {
        mostrarHintPuerta(zonaPuerta);
        hayPuertaCerca = true;
    } else {
        ocultarHintPuerta();
        hayPuertaCerca = false;
    }

    // --- Transiciones automáticas ---

    //MAIN ENTRADA
    //-------------------------------------------------------------------------------------------------------------------------
    if (pasilloActual == 1 && rectJugador.intersects(QRect(998, 784, 10, 100))) { //ir a izquierda

        this->pasilloActual=2;
        configurarEscena();
        configurarObstaculos();
        jugador->move(190,770);
        this->update();
        qDebug()<<"transision izquierda";
    }
    else if (pasilloActual == 1 && rectJugador.intersects(QRect(-54, 784, 10, 100))) { //ir a derecha

        qDebug()<<"transision derecha";
    }

    //PASILLOS
    //-------------------------------------------------------------------------------------------------------------------------
        //Pasillo izquierda
    if(pasilloActual==2 && rectJugador.intersects(QRect(-54, 784, 10, 100))){ // volver a main de izq

        this->pasilloActual=1;
        configurarEscena();
        configurarObstaculos();
        jugador->move(820,826);
        this->update();
        qDebug()<<"Vuelta a Main ";
    }

    else if(pasilloActual==2 && rectJugador.intersects(QRect(200, 450, 10, 100))){ //entrar ruleta izq
        this->pasilloActual=4;
        configurarEscena();
        configurarObstaculos();
        jugador->move(746,794);
        this->update();
        qDebug()<<"Entrar Ruleta I";
    }


        //Pasillo derecha
     else if(pasilloActual==3 && rectJugador.intersects(QRect(998, 784, 10, 100))){ // volver a main de der

    }

     //Ruleta
    //-------------------------------------------------------------------------------------------------------------------------
    if (pasilloActual>=4 && rectJugador.intersects(QRect(910,852, 10, 100))){ //salir a izq
        this->pasilloActual=2;
        configurarEscena();
        configurarObstaculos();
        jugador->move(250,500);
        this->update();
        qDebug()<<"Salirs Ruleta I";
    }


}

void Interior::detectarZonaPuerta() {
    if (!jugador) return;

    QPoint pos = jugador->pos();


    // --- Detectar la puerta según el pasillo actual ---
    switch (pasilloActual) {
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


void Interior::mostrarHintPuerta(const QRect& zonaPuerta) {
    if (!labelPuerta->isVisible()) {

        QString textoHint;

        // --- Mensaje personalizado según el pasillo ---
        switch (pasilloActual) {
        case 1:
            textoHint = "Presiona Q para salir";
            break;
        case 4:
            textoHint = "Presiona R para girar";
            break;
        case 5:
            textoHint = "Minijuego Arte";
            break;
        case 6:
            textoHint = "Minijuego Politica";
            break;
        case 7:
            textoHint = "Minijuego Ciencia";
            break;
        case 8:
            textoHint = "Minijuego Historia";
            break;
        default:
            textoHint = "Puerta cerrada";
            break;
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


void Interior::ocultarHintPuerta() {
    labelPuerta->hide();
}


void Interior::keyPressEvent(QKeyEvent* event) {
    ControlPersonaje::keyPressEvent(event);

    if (event->key() == Qt::Key_Q && hayPuertaCerca) {

        switch (pasilloActual) {
        case 1:
            SalirCastillo();
            break;
        case 2:

            break;
        case 3:

            break;

        case 5:
        case 6:
        case 7:
        case 8:
            EntrarMinijuego();
            break;

        default:

            break;
        }
    }

    if (event->key() == Qt::Key_R && hayPuertaCerca) {
        if(pasilloActual==4){
            if (!puertas.contains(false)) {
                qDebug() << "Ya jugaste todos los minijuegos";
            }

            int indice; //guardamos el indice
            do {
                indice = QRandomGenerator::global()->bounded(0, 4); // genera 0,1,2 o 3
            } while (puertas[indice]);

            qDebug() << indice;

            this->ResetearMovimiento();
            RuletaWidget* r = new RuletaWidget(puertas, indice, this);
            r->setAttribute(Qt::WA_DeleteOnClose);
            puertas[indice]=true;
            r->show();
        }

    }
}

void Interior::SalirCastillo(){
    if (!jugador) return;

    ResetearMovimiento();

    FrontView* FV = new FrontView(jugador);
    FV->show();

    this->close();
}

void Interior::EntrarMinijuego(){
    if (!jugador) return;

    ResetearMovimiento();


    if(pasilloActual==5){ //Arte
        qDebug()<<"minijuego Arte";

    } else if(pasilloActual==6){ //Politica
        qDebug()<<"minijuego Politica";

    }else if(pasilloActual==7){ //Ciencia
        MinijuegoCiencia *MC = new MinijuegoCiencia(jugador);
        MC->show();

    }else if(pasilloActual==8){ //Historia
       MinijuegoHistoria* MH = new MinijuegoHistoria(jugador);
        MH->show();
    }

    this->close();
}



void Interior::actualizarPasilloRuleta(int indice) {
    this->pasilloActual = 5 + indice;
    //this->pasilloActual=8;
    configurarEscena();
    configurarObstaculos();
    this->update();
}



void Interior::mousePressEvent(QMouseEvent* event)
{
    //qDebug() << "Coordenadas del click: " << event->pos();
    qDebug() << "Jugador en:" << jugador->pos();

    this->ActualizarCorazones(true);
    qDebug() << "Vidas:" << jugador->getCorazones();

    if(pasilloActual>4){
       // pasilloActual=4;
        pasilloActual=7;
        configurarEscena();
        configurarObstaculos();
    }

}
