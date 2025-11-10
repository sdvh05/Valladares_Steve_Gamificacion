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
    obstaculos.append(QRect(-30,1000, 1000, 4));//suelo

    switch (pasillo) {
    case 1:
        obstaculos.append(QRect(-30, 770, 1000, 4)); // suelo
        break;

    case 2:

        break;

    case 3:

        break;

    case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        obstaculos.append(QRect(140, 442, 1000, 4)); //pared puerta arriba
        obstaculos.append(QRect(490,650, 20, 4)); //ruleta
        break;






        break;
    case 9:
        obstaculos.append(QRect(460,612, 1000, 4));//arriba

        break;
    case 10:
        obstaculos.append(QRect(354,708, 1000, 4));//
        break;

    case 11:

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
        jugador->move(426,820);
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
    if(pasilloActual==9 && rectJugador.intersects(QRect(426,990, 200, 1))){ //volver a pasiilo izq
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


    //nuevo: descion conect ruleta
    if(pasilloActual==9 && rectJugador.intersects(QRect(934,808, 10, 600))){ //voler
        this->pasilloActual=4;
        configurarEscena();
        configurarObstaculos();
        jugador->move(90,782);
        this->update();
        qDebug() << "D->R";
        return;
    }

    if(pasilloActual==4 && rectJugador.intersects(QRect(-52,772, 80, 80))){ //
        this->pasilloActual=9;
        configurarEscena();
        configurarObstaculos();
        jugador->move(756,790);
        this->update();
        qDebug() << "R->D";
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
        if (pasilloActual == 4) {

            // Caso: todas las puertas ya jugadas
            if (!jugador->puertas.contains(false)) {
                qDebug() << "Ya jugaste todos los minijuegos";

                QLabel* aviso = new QLabel("🎯 Ya jugaste todos los minijuegos.\nNo puedes girar la ruleta.", this);
                aviso->setStyleSheet("background: rgba(0,0,0,200); color: white; font: bold 14px 'Segoe UI';"
                                     "border: 2px solid gray; border-radius: 10px; padding: 8px;");
                aviso->adjustSize();
                aviso->move(width()/2 - aviso->width()/2, height()/2 - aviso->height()/2);
                aviso->show();

                QTimer::singleShot(2500, aviso, [=]() { aviso->deleteLater(); });
                return;
            }

            int indice = -1;
            int intentos = 0;
            do {
                indice = QRandomGenerator::global()->bounded(0, 4);
                intentos++;
                if (intentos > 100) break;
            } while (jugador->puertas[indice]);

            qDebug() << "Ruleta seleccionó el índice:" << indice;

            this->ResetearMovimiento();

            // Abre ruleta solo si hay minijuegos disponibles
            if (jugador->puertas.contains(false)) {
                ruleta = new RuletaWidget(jugador->puertas, indice, this);
                connect(ruleta, &RuletaWidget::ruletaFinalizada, this, &Interior::actualizarPasilloRuleta);
                ruleta->setGeometry(340, 80, 520, 520);
                ruleta->setAttribute(Qt::WA_DeleteOnClose);
                ruleta->show();

                jugador->puertas[indice] = true;
            } else {
                QLabel* aviso = new QLabel("🎯 No hay minijuegos disponibles para girar.", this);
                aviso->setStyleSheet("background: rgba(0,0,0,200); color: white; font: bold 14px 'Segoe UI';"
                                     "border: 2px solid gray; border-radius: 10px; padding: 8px;");
                aviso->adjustSize();
                aviso->move(width()/2 - aviso->width()/2, height()/2 - aviso->height()/2);
                aviso->show();

                QTimer::singleShot(2500, aviso, [=]() { aviso->deleteLater(); });
            }
        }
    }


    if (event->key() == Qt::Key_E) {
        if (npcs.contains(pasilloActual)) {
            Npc* npc = npcs[pasilloActual];
            if (npc && npc->isVisible()) {

                // 🔹 NPC normal
                if (pasilloActual != 4 && pasilloActual != 5 && pasilloActual != 6 && pasilloActual != 7 && pasilloActual != 8) {
                    npc->Interactuar(jugador);

                    if (!jugador->tieneMapa && pasilloActual == 1) {
                        inventarioGlobal->agregarMapa();
                        jugador->tieneMapa = true;
                    }
                    return;
                }

                // 🔹 NPC de la ruleta
                bool todasPuertasAbiertas = !jugador->puertas.contains(false);
                bool todosGanados = !jugador->ganadas.contains(false);

                if (todasPuertasAbiertas) {
                    if (todosGanados) {
                        npc->setDialogos(QStringList()
                                         << "🏆 Ya ganaste todos los minijuegos."
                                         << "No hay más desafíos disponibles.");
                        npc->Interactuar(jugador);
                    } else {
                        mostrarPreguntaNPC();
                    }
                } else {
                    npc->setDialogos(QStringList()
                                         << "Bienvenido a la sala del destino."
                                         <<"Aqui Cargaras vida para \n el Combate del Conocimiento"
                                     << "¿Estas listo para ver \n que minijuego te espera?");
                    npc->Interactuar(jugador);
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
    qDebug() << "Pasillo Act:" << pasilloActual;
    qDebug() << "Vidas:" << jugador->getCorazones();

    // 🔹 Mostrar el estado de progreso
    qDebug() << "Puertas abiertas:";
    for (int i = 0; i < jugador->puertas.size(); ++i) {
        qDebug() << " Puerta" << i << ":" << (jugador->puertas[i] ? "✅ Abierta" : "❌ Cerrada");
    }

    qDebug() << "Minijuegos ganados:";
    for (int i = 0; i < jugador->ganadas.size(); ++i) {
        qDebug() << " Juego" << i << ":" << (jugador->ganadas[i] ? "🏆 Ganado" : "⏳ Pendiente");
    }

    if (pasilloActual > 4 && pasilloActual < 9) {
        pasilloActual = 4;
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


    // PASILLO 2 -
    Npc* npc2 = new Npc(this);
    npc2->setNombre("Pass");
    npc2->move(610,578);
    npc2->hide();
    npc2->miradoDerecha=false;
    QStringList d2;
    d2 << "Mas Adelante se encuentra \n la sala de la Ruleta"
       << "Estoy aqui porque perdi \n todos los minijuegos"
       << "Se que al termirar los 4, \n puedo reintentar en los que perdi"
       <<" Pero aun reintentandolos, \n sigo sin ganar vidas";
    npc2->setDialogos(d2);
    auto anim2 = npc2->obtenerAnimacion("idle");
    npc2->SetAnimacion(anim2.ruta, 6);
    npcs.insert(2, npc2);


    // PASILLOS 4-8 - NPC compartido (Ruleta)
    QStringList dRuleta;
    dRuleta << "Bienvenido a la Ruleta del Saber"
            <<"Aqui Cargaras vida para \n el Combate del Conocimiento"
            << "¿Estas listo para ver \n que minijuego te espera?";
    for (int p : {4, 5, 6, 7, 8}) {
        Npc* npcR = new Npc(this);
        npcR->setNombre("Rubio");
        npcR->move(158,734);
        npcR->hide();
        npcR->setDialogos(dRuleta);
        auto animR = npcR->obtenerAnimacion("idle");
        npcR->SetAnimacion(animR.ruta, 7);
        npcs.insert(p, npcR);
    }


    // PASILLO 9 - Eleccion
    Npc* npc9 = new Npc(this);
    npc9->setNombre("Mix");
    npc9->move(108,740);
    npc9->hide();
    QStringList d9;
    if(jugador->Bando==0){
        d9 << "Has llegado lejos, viajero."
           << "Elige con sabiduría \n tu destino en el combate final."
           << "Recuerda que siempre puedes volver \n a La ruleta del Saber \n para cargar mas vidas";
    } else if (jugador->Bando==1){
        d9 << "Veo que eres Todo un Racionalista"
           << "Te queda bien el Rojo"
            << "Recuerda que siempre puedes volver \n a jugar los minijuegos en los que fallaste";
    } else if (jugador->Bando==2){
        d9 << "Veo que eres Todo un Empirista"
           << "Te queda bien el Blanco"
            << "Recuerda que siempre puedes volver \n a jugar los minijuegos en los que fallaste";
    }

    npc9->setDialogos(d9);
    auto anim9 = npc9->obtenerAnimacion("idle");
    npc9->SetAnimacion(anim9.ruta, anim9.frames);
    npcs.insert(9, npc9);

    //DESCARTES - Kant
    Npc* npc10 = new Npc(this);
    npc10->setNombre("bluey");
    npc10->move(18,786);
    npc10->hide();
    QStringList d10;
    d10 << "Estas En los \n Salones Finales:"
        << "Aquí se guardan los saberes \n de Descartes y Kant."
       << "Buena Suerte con \n los acertijos";
    npc10->setDialogos(d10);
    auto anim10 = npc10->obtenerAnimacion("idle");
    npc10->SetAnimacion(anim10.ruta, 7);
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

void Interior::mostrarPreguntaNPC()
{
    ResetearMovimiento();

    // Si todos los minijuegos fueron ganados
    if (!jugador->ganadas.contains(false)) {
        QLabel* mensaje = new QLabel("🏆 ¡Has ganado en todos los minijuegos!\nNo hay más retos por ahora.", this);
        mensaje->setStyleSheet("background: rgba(0,0,0,200); color: white; font: bold 15px 'Segoe UI';"
                               "border: 2px solid #d4af37; border-radius: 10px; padding: 10px;");
        mensaje->adjustSize();
        mensaje->move(width()/2 - mensaje->width()/2, height()/2 - mensaje->height()/2);
        mensaje->show();
        QTimer::singleShot(3000, mensaje, [=]() { mensaje->deleteLater(); });
        return;
    }

    QLabel* pregunta = new QLabel("¿Deseas volver a intentar los minijuegos\nque aún no has ganado?", this);
    pregunta->setStyleSheet("background: rgba(0,0,0,200); color: white; font: bold 16px 'Segoe UI';"
                            "border: 2px solid #c9a44c; border-radius: 10px; padding: 10px;");
    pregunta->setAlignment(Qt::AlignCenter);
    pregunta->setFixedSize(440, 70);
    pregunta->move(width()/2 - pregunta->width()/2, height()/2 - 100);
    pregunta->show();

    QPushButton* btnSi = new QPushButton("Sí", this);
    QPushButton* btnNo = new QPushButton("No", this);
    btnSi->setGeometry(pregunta->x() + 90, pregunta->y() + 80, 100, 35);
    btnNo->setGeometry(pregunta->x() + 250, pregunta->y() + 80, 100, 35);

    btnSi->setStyleSheet("background:#3a9a3a; color:white; font:bold 14px; border-radius:8px;");
    btnNo->setStyleSheet("background:#a33; color:white; font:bold 14px; border-radius:8px;");

    btnSi->show();
    btnNo->show();

    connect(btnSi, &QPushButton::clicked, this, [=]() {
        pregunta->deleteLater();
        btnSi->deleteLater();
        btnNo->deleteLater();

        // 🔄 Rehabilitamos los juegos NO ganados
        for (int i = 0; i < jugador->puertas.size(); ++i) {
            jugador->puertas[i] = jugador->ganadas[i]; // si no ganaste, vuelve a estar disponible
        }

        QLabel* aviso = new QLabel("🔄 Los minijuegos que no ganaste\nvuelven a estar disponibles.", this);
        aviso->setStyleSheet("background: rgba(0,0,0,200); color: white; font: bold 14px 'Segoe UI';"
                             "border: 2px solid #c9a44c; border-radius: 10px; padding: 8px;");
        aviso->adjustSize();
        aviso->move(width()/2 - aviso->width()/2, height()/2 - aviso->height()/2);
        aviso->show();
        QTimer::singleShot(3000, aviso, [=]() { aviso->deleteLater(); });
    });

    connect(btnNo, &QPushButton::clicked, this, [=]() {
        pregunta->deleteLater();
        btnSi->deleteLater();
        btnNo->deleteLater();
    });
}

