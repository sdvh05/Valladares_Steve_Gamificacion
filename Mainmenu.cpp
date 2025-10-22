#include "MainMenu.h"
#include "Frontview.h"

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QPixmap>
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QDir>
#include <QDataStream>
#include <QMessageBox>
#include <QDebug>

MainMenu::MainMenu(QWidget *parent)
    : QWidget(parent), jugador(nullptr)
{
    this->resize(1100, 700);
    this->setWindowTitle("Proyecto Gamificacion");

    // Centrar ventana en pantalla
    QScreen *pantalla = QApplication::primaryScreen();
    QRect centroPantalla = pantalla->availableGeometry();
    this->move(centroPantalla.center() - this->rect().center());

    // ===== Fondo =====
    fondo = new QLabel(this);
    QPixmap fondoImg("Sprites/Castle/MainMenu.jpg");
    if (fondoImg.isNull()) {
        qDebug() << "No se pudo cargar el fondo del menú principal.";
    } else {
        fondo->setPixmap(fondoImg.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
    fondo->setGeometry(0, 0, width(), height());
    fondo->lower();

    // ===== Fuentes =====
    QFont fuenteTitulo("Arial", 26, QFont::Bold);
    QFont fuenteSubTitulo("Arial", 14, QFont::Bold);

    // ===== Título =====
    lblTitulo = new QLabel(" GAMIFICACION");
    lblTitulo->setFont(fuenteTitulo);
    lblTitulo->setStyleSheet("color: purple;");
    lblTitulo->setAlignment(Qt::AlignCenter);

    // ===== Subtítulo =====
    miniTitulo = new QLabel("La Epistemología");
    miniTitulo->setFont(fuenteSubTitulo);
    miniTitulo->setStyleSheet("color: white;");
    miniTitulo->setAlignment(Qt::AlignCenter);

    // ===== Botones =====
    btnNuevaPartida = new QPushButton("Jugar");
    Salir = new QPushButton("Salir");

    QString estiloBoton =
        "QPushButton {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                      stop:0 #6A5ACD, stop:0.1 #483D8B,"
        "                                      stop:0.5 #4B0082, stop:0.9 #2F4F4F,"
        "                                      stop:1 #2C3E50);"
        "    color: #E6E6FA;"
        "    border: 3px solid #2C3E50;"
        "    border-radius: 12px;"
        "    font-family: 'Times New Roman';"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    padding: 16px 32px;"
        "    min-width: 300px;"
        "    text-transform: uppercase;"
        "    letter-spacing: 1px;"
        "}"
        "QPushButton:hover {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                      stop:0 #7B68EE, stop:0.1 #6A5ACD,"
        "                                      stop:0.5 #483D8B, stop:0.9 #4B0082,"
        "                                      stop:1 #36454F);"
        "    border: 3px solid #6A5ACD;"
        "    color: #FFFFFF;"
        "}"
        "QPushButton:pressed {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                      stop:0 #483D8B, stop:0.5 #4B0082,"
        "                                      stop:1 #2C3E50);"
        "}";

    btnNuevaPartida->setStyleSheet(estiloBoton);
    Salir->setStyleSheet(estiloBoton);

    // ===== Layout vertical =====
    QVBoxLayout *layoutVertical = new QVBoxLayout();
    layoutVertical->addWidget(lblTitulo);
    layoutVertical->addWidget(miniTitulo);
    layoutVertical->addSpacing(50);
    layoutVertical->addWidget(btnNuevaPartida, 0, Qt::AlignCenter);
    layoutVertical->addWidget(Salir, 0, Qt::AlignCenter);
    layoutVertical->addStretch();

    // ===== Contenedor central =====
    QWidget *contenedor = new QWidget(this);
    contenedor->setLayout(layoutVertical);
    contenedor->setFixedSize(600, 400);
    contenedor->move(width() / 2 - contenedor->width() / 2,
                     height() / 2 - contenedor->height() / 2);
    contenedor->raise();

    // ===== Conexiones =====
    connect(Salir, &QPushButton::clicked, this, &MainMenu::close);
    connect(btnNuevaPartida, &QPushButton::clicked, this, &MainMenu::AbrirJuego);
}

void MainMenu::AbrirJuego()
{


    if (!jugador) {
        jugador = new Personaje();
        auto anim = jugador->obtenerAnimacion("idle");
        jugador->SetAnimacion(anim.ruta, anim.frames);
    }

   FrontView *ventanaJuego = new FrontView(jugador);
    ventanaJuego->show();




    this->close();
}

