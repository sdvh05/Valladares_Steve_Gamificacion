#include "MainMenu.h"
#include "Frontview.h"
#include "TableroPuntos.h"


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
#include <QLineEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>

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
    Salir = new QPushButton("Clasificacion");

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
    connect(Salir, &QPushButton::clicked, [=]() {
        TableroPuntos *ventanaPuntos = new TableroPuntos();
        ventanaPuntos->show();
    });

    connect(btnNuevaPartida, &QPushButton::clicked, this, &MainMenu::PreguntarNombre);
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

void MainMenu::PreguntarNombre()
{
    QDialog *popup = new QDialog(this);
    popup->setModal(true);
    popup->setFixedSize(420, 260);
    popup->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    popup->setAttribute(Qt::WA_TranslucentBackground);


    QWidget *contenedor = new QWidget(popup);
    contenedor->setObjectName("fondoMorado");
    contenedor->setStyleSheet(
        "#fondoMorado {"
        "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,"
        "stop:0 rgba(40, 0, 60, 230), stop:1 rgba(90, 0, 130, 230));"
        "border: 3px solid #cfa75c;"
        "border-radius: 15px;"
        "}"
        );
    contenedor->setGeometry(0, 0, 420, 260);

    // ✨ Sombra suave dorada
    QGraphicsDropShadowEffect *sombra = new QGraphicsDropShadowEffect();
    sombra->setBlurRadius(25);
    sombra->setOffset(3, 5);
    sombra->setColor(QColor(120, 80, 40, 200));
    contenedor->setGraphicsEffect(sombra);

    // 👑 Título
    QLabel *lblTitulo = new QLabel("¿Cuál es tu nombre, Jugador?");
    lblTitulo->setStyleSheet(
        "color: #f5e6c8;"
        "font: bold 20px 'Papyrus';"
        "letter-spacing: 1px;"
        "background: transparent;"
        );
    lblTitulo->setAlignment(Qt::AlignCenter);

    // ✍️ Campo de texto
    QLineEdit *txtNombre = new QLineEdit();
    txtNombre->setPlaceholderText("Escribe tu nombre aquí...");
    txtNombre->setStyleSheet(
        "QLineEdit {"
        "background-color: rgba(255, 255, 255, 180);"
        "border: 2px solid #cfa75c;"
        "border-radius: 8px;"
        "padding: 6px;"
        "font: 16px 'Courier New';"
        "color: #3a2250;"
        "}"
        "QLineEdit:focus {"
        "border: 2px solid #e6cf80;"
        "background-color: rgba(255,255,255,220);"
        "}"
        );

    // ⚔️ Botón Aceptar (tema morado-dorado)
    QPushButton *btnAceptar = new QPushButton("Aceptar");
    QString estiloBoton =
        "QPushButton {"
        "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,"
        "stop:0 #4a007d, stop:1 #7c3cb3);"
        "color: #f9eac3;"
        "font: bold 16px 'Papyrus';"
        "border: 2px solid #cfa75c;"
        "border-radius: 8px;"
        "padding: 8px 20px;"
        "}"
        "QPushButton:hover { background-color: #9d5de0; }";

    btnAceptar->setStyleSheet(estiloBoton);

    // 🧭 Layout vertical
    QVBoxLayout *layout = new QVBoxLayout(contenedor);
    layout->addSpacing(20);
    layout->addWidget(lblTitulo);
    layout->addSpacing(15);
    layout->addWidget(txtNombre);
    layout->addSpacing(25);
    layout->addWidget(btnAceptar, 0, Qt::AlignCenter);
    layout->addStretch();
    contenedor->setLayout(layout);

    // 🪄 Conexión del botón
    connect(btnAceptar, &QPushButton::clicked, [=]() {
        QString nombreIngresado = txtNombre->text().trimmed();
        if (nombreIngresado.isEmpty()) {
            QMessageBox::warning(popup, "Atención", "Por favor, escribe tu nombre antes de continuar.");
            return;
        }

        // Crear jugador y configurar animación
        if (!jugador) {
            jugador = new Personaje();
            jugador->nombre = nombreIngresado;
            jugador->guardarPuntos(jugador->nombre,jugador->puntos);

            auto anim = jugador->obtenerAnimacion("idle");
            jugador->SetAnimacion(anim.ruta, anim.frames);
        }

        // Cerrar popup y abrir el juego
        popup->accept();
        FrontView *ventanaJuego = new FrontView(jugador);
        ventanaJuego->show();
        this->close();
    });

    popup->exec();
}

