#include "mainwindow.h"
#include "Personaje.h"
#include "Mainmenu.h"
#include "Ruleta.h"


#include <QApplication>

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






    return a.exec();
}
