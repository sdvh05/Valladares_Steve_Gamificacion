#include "Interior.h"
#include "Frontview.h"
#include "Ruleta.h"

#include "Minijuegohistoria.h"
#include "Minijuegociencia.h"
#include "Minijuegopolitica.h"
#include "Minijuegoarte.h"

#include "Minijuegodescartes.h"
#include "Minijuegokant.h"

#include "Combate.h"

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

    crearNpcs();
    actualizarNpc();

    Movimientos();
}

void Interior::configurarEscena(){
    int pasillo=pasilloActual;
    QString rutaFondo;

    switch (pasillo) {
    case 1: //entrada
        rutaFondo = "Sprites/Castle/Interior.jpg";
        break;
    case 2: //der
        rutaFondo = "Sprites/Castle/Pasillo.jpg";
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


    case 3: //izq
        rutaFondo = "Sprites/Castle/pasillo.png";
        break;

    case 9: //zona seleccion
        rutaFondo = "Sprites/Castle/Seleccion.png";
        break;

    case 10: //D&K
        rutaFondo = "Sprites/Castle/Pasillo10.png";
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

    actualizarNpc();
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
        return;
    }
    else if (pasilloActual == 1 && rectJugador.intersects(QRect(-54, 784, 10, 100))) { //ir a derecha
        jugador->move(820,826);
        this->pasilloActual =3;
        configurarEscena();
        configurarObstaculos();
        this->update();
        qDebug()<<"transision derecha";
        return;
    }

    //PASILLOS
    //-------------------------------------------------------------------------------------------------------------------------
        //Pasillo izquierda
    if(pasilloActual==2 && rectJugador.intersects(QRect(-54, 784, 10, 100))){ // volver a main de der

        this->pasilloActual=1;
        configurarEscena();
        configurarObstaculos();
        jugador->move(820,826);
        this->update();
        qDebug()<<"Vuelta a Main ";
        return;
    }

    else if(pasilloActual==2 && rectJugador.intersects(QRect(200, 450, 10, 100))){ //entrar ruleta der
        this->pasilloActual=4;
        configurarEscena();
        configurarObstaculos();
        jugador->move(746,794);
        this->update();
        qDebug()<<"Entrar Ruleta I";
        return;
    }


        //Pasillo derecha
     else if(pasilloActual==3 && rectJugador.intersects(QRect(998, 784, 50, 100))){ // volver a main de izq
        this->pasilloActual=1;
        configurarEscena();
        configurarObstaculos();
        jugador->move(40,844);
        this->update();
        qDebug()<<"Vuelta a Main ";
        return;
    }

    else if(pasilloActual==3 && rectJugador.intersects(QRect(158,538, 250, 100))){ // volver a main de izq
        this->pasilloActual=9;
        configurarEscena();
        configurarObstaculos();
        jugador->move(426,850);
        this->update();
        qDebug()<<"Entrar Seleccion";
        return;

    }

     //Ruleta
    //-------------------------------------------------------------------------------------------------------------------------
    if (pasilloActual>=4 && pasilloActual<=8 && rectJugador.intersects(QRect(910,852, 10, 100))){ //salir a der
        this->pasilloActual=2;
        configurarEscena();
        configurarObstaculos();
        jugador->move(250,500);
        this->update();
        qDebug()<<"Salirs Ruleta I";
        return;
    }

    //SELECCION
    if(pasilloActual==9 && rectJugador.intersects(QRect(426,1000, 100, 4))){ //volver a pasiilo izq
        this->pasilloActual=3;
        configurarEscena();
        configurarObstaculos();
        jugador->move(240,642);
        this->update();
        return;
    }

   if(pasilloActual==3 && rectJugador.intersects(QRect(-15,784, 50, 200))){ //ir a pasiilo de kant / descartes

        this->pasilloActual=10;
        jugador->move(708,828);
        configurarEscena();
        configurarObstaculos();
        this->update();
        qDebug() << "KD";
        return;
    }

   if(pasilloActual==10 && rectJugador.intersects(QRect(920,828, 50, 400))){ //voler
        this->pasilloActual=3;
        configurarEscena();
        configurarObstaculos();
        jugador->move(84,780);
        this->update();
        qDebug() << "KD->3";
        return;
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
        zonaPuerta = QRect();
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
    QRect rectJugador = jugador->geometry();

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

        case 9:

        default:

            break;
        }
    }

    if(event->key() == Qt::Key_Q){
        if(pasilloActual==9 && rectJugador.intersects(QRect(286,666, 250, 100)) && jugador->puertas.contains(!false)){
            jugador->Bando=1;
            ResetearMovimiento();
            Combate* BF = new Combate(jugador,nullptr, jugador->Bando);
            BF->show();
            this->close();

        } else if(pasilloActual==9 && rectJugador.intersects(QRect(702,666, 250, 100))){
            jugador->Bando=2;
            ResetearMovimiento();
            Combate* BF = new Combate(jugador,nullptr, jugador->Bando);
            BF->show();
            this->close();
        }

        if(pasilloActual==10 && rectJugador.intersects(QRect(518,744, 200, 80))){
            ResetearMovimiento();
            MinijuegoDescartes *MD = new MinijuegoDescartes(jugador);
            MD->show();
            this->close();

        }else if(pasilloActual==10 && rectJugador.intersects(QRect(198,744, 200, 80))){
            ResetearMovimiento();
            MinijuegoKant *MK = new MinijuegoKant(jugador);
            MK->show();
            this->close();
            qDebug()<<"KANT";
        }

    }

    if (event->key() == Qt::Key_R && hayPuertaCerca) {
        if(pasilloActual==4){
            if (!jugador->puertas.contains(false)) {
                qDebug() << "Ya jugaste todos los minijuegos";
            }

            int indice; //guardamos el indice
            do {
                indice = QRandomGenerator::global()->bounded(0, 4); // genera 0,1,2 o 3
            } while (jugador->puertas[indice]);

            qDebug() << indice;

            this->ResetearMovimiento();

            if(jugador->puertas.contains(false)){
            ruleta = new RuletaWidget(jugador->puertas, indice, this);
            connect(ruleta, &RuletaWidget::ruletaFinalizada, this, &Interior::actualizarPasilloRuleta);
            ruleta->setGeometry(340, 80, 520, 520);
            //ruleta->setGeometry(340, 520, 520, 520);
            ruleta->setAttribute(Qt::WA_DeleteOnClose);
            ruleta->show();
            }else{
                //mostrar un label de que ya no se puede girar
            }



            jugador->puertas[indice] = true;


        }
    }

    if (event->key() == Qt::Key_E) {
        if (npcs.contains(pasilloActual)) {
            Npc* npc = npcs[pasilloActual];
            if (npc && npc->isVisible()) {
                npc->Interactuar(jugador);
                //jugador->guardarPuntos(jugador->nombre, jugador->puntos);


                if (!jugador->tieneMapa && pasilloActual == 1) {
                    inventarioGlobal->agregarMapa();
                    jugador->tieneMapa = true;
                }
            }
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
        try {
            MinijuegoArte* MA = new MinijuegoArte(jugador);
            MA->show();
        } catch (...) {
            qDebug() << "Ocurrió una excepción al crear MinijuegoArte";
        }

    } else if(pasilloActual==6){ //Politica
        MinijuegoPolitica* MP = new MinijuegoPolitica(jugador);
        MP->show();

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

    qDebug()<<"Pasillo Act: " <<pasilloActual;

    //this->ActualizarCorazones(true);
    qDebug() << "Vidas:" << jugador->getCorazones();


    if(pasilloActual>4 && pasilloActual<9){
        pasilloActual=4;
        configurarEscena();
        configurarObstaculos();
    }

    ResetearMovimiento();

}



void Interior::crearNpcs() {

    // PASILLO 1 - Guardia
    Npc* npc1 = new Npc(this);
    npc1->setNombre("Empirista");
    npc1->move(202,782);
    npc1->hide();
    QStringList d1;
    d1 << "¡Bienvenido al Castillo!"
       << "Me alegra ver a alguien Nuevo por aqui"
       << "El interior del Castillo puede ser algo Confuso"
       << "Ten este Mapa para Guiarte"
       << "Presiona la M para Usarlo";
    npc1->setDialogos(d1);
    auto anim1 = npc1->obtenerAnimacion("idle");
    npc1->SetAnimacion(anim1.ruta, anim1.frames);
    npcs.insert(1, npc1);


    // PASILLO 2 - Bibliotecaria
    Npc* npc2 = new Npc(this);
    npc2->setNombre("Empirista");
    npc2->move(610,578);
    npc2->hide();
    npc2->miradoDerecha=false;
    QStringList d2;
    d2 << "Aquí se guardan los saberes de Descartes y Kant."
       << "Usa la razón para avanzar, no solo el valor.";
    npc2->setDialogos(d2);
    auto anim2 = npc2->obtenerAnimacion("idle");
    npc2->SetAnimacion(anim2.ruta, anim2.frames);
    npcs.insert(2, npc2);


    // PASILLOS 4-8 - NPC compartido (Ruleta)
    QStringList dRuleta;
    dRuleta << "Bienvenido a la sala del destino."
            <<"Aqui Cargaras vida para \n el Combate del Conocimiento"
            << "¿Estas listo para ver \n que minijuego te espera?";
    for (int p : {4, 5, 6, 7, 8}) {
        Npc* npcR = new Npc(this);
        npcR->setNombre("Empirista");
        npcR->move(158,734);
        npcR->hide();
        npcR->setDialogos(dRuleta);
        auto animR = npcR->obtenerAnimacion("idle");
        npcR->SetAnimacion(animR.ruta, animR.frames);
        npcs.insert(p, npcR);
    }


    // PASILLO 9 - Sabio
    Npc* npc9 = new Npc(this);
    npc9->setNombre("Empirista");
    npc9->move(108,740);
    npc9->hide();
    QStringList d9;
    if(jugador->Bando==0){
        d9 << "Has llegado lejos, viajero."
           << "Elige con sabiduría \n tu destino en el combate final.";
    } else if (jugador->Bando==1){
        d9 << "Veo que eres Todo un Racionalista"
           << "Te queda bien el Rojo";
    } else if (jugador->Bando==2){
        d9 << "Veo que eres Todo un Empirista"
           << "Te queda bien el Blanco";
    }

    npc9->setDialogos(d9);
    auto anim9 = npc9->obtenerAnimacion("idle");
    npc9->SetAnimacion(anim9.ruta, anim9.frames);
    npcs.insert(9, npc9);

    //DESCARTES - Kant
    Npc* npc10 = new Npc(this);
    npc10->setNombre("Empirista");
    npc10->move(18,786);
    npc10->hide();
    QStringList d10;
    d10 << "Estas En los \n Salones Finales:"
       << "Buena Suerte con \n los acertijos";
    npc10->setDialogos(d10);
    auto anim10 = npc10->obtenerAnimacion("idle");
    npc10->SetAnimacion(anim9.ruta, anim9.frames);
    npcs.insert(10, npc10);
}

void Interior::actualizarNpc() {
    for (auto it = npcs.begin(); it != npcs.end(); ++it) {
        if (it.key() == pasilloActual && pasilloActual != 3) {
            it.value()->show();
            it.value()->raise();
        } else {
            it.value()->hide();
        }
    }
}
