#include "mainwindow.h"
#include "Personaje.h"
#include "Mainmenu.h"
#include "Ruleta.h"
#include "InventarioWidget.h"
#include"inventario.h"


#include <QApplication>

Inventario* inventarioGlobal = nullptr;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainMenu w;
    w.show();



   /* QList<bool> puertas = {false, false, false, false};
    //                  0=Arte,1=Politica,2=Ciencia,3=Historia
    int resultado = 2;
    RuletaWidget r(puertas, resultado);
    r.show();*/

    inventarioGlobal = new Inventario();
    inventarioGlobal ->agregarMedallaCiencia();
    inventarioGlobal->agregarMedallaArte();
    inventarioGlobal->agregarMedallaHistoria();
    inventarioGlobal->agregarMedallaPolitica();


    inventarioGlobal->agregarMedallaDescartes();
    inventarioGlobal->agregarMedallaKant();
    /*InventarioWidget* inventarioUI = new InventarioWidget(inventarioGlobal);
    inventarioUI->show();*/





    return a.exec();
}
