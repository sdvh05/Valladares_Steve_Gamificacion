#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include "Personaje.h"

class QPushButton;
class QLabel;

class MainMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = nullptr);

private:
    QLabel *lblTitulo;
    QLabel *miniTitulo;
    QPushButton *btnNuevaPartida;
    QPushButton *btnCargarPartida;
    QPushButton *Salir;

    QLabel *fondo;

    Personaje *jugador;

    void AbrirJuego();
    void AbrirMenuCargarPartida();
};

#endif // MAINMENU_H

