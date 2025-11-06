#include "InventarioWidget.h"
#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QEvent>
#include <QFrame>
#include <QPainter>
#include <QStyleOption>

InventarioWidget::InventarioWidget(Inventario* inv, QWidget* parent)
    : QWidget(parent), inventario(inv)
{
    setFixedSize(600, 500);
    this->setWindowTitle("Inventario del Jugador");

    // Fondo
    setStyleSheet("QWidget {"
                  "background-image: url('Sprites/Castle/Inventario.jpg');"
                  "background-repeat: no-repeat;"
                  "background-position: center;"
                  "}");

    layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(15, 15, 15, 15);
    layoutPrincipal->setSpacing(10);

    // Scroll
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical { width: 10px; background: rgba(0,0,0,50); border-radius: 5px; }"
        "QScrollBar::handle:vertical { background: #b8860b; border-radius: 5px; }"
        );
    layoutPrincipal->addWidget(scrollArea);

    contenedor = new QWidget(this);
    layoutContenedor = new QVBoxLayout(contenedor);
    layoutContenedor->setSpacing(8);
    contenedor->setLayout(layoutContenedor);
    contenedor->setStyleSheet("background: rgba(30, 20, 10, 100); border-radius: 10px;");
    scrollArea->setWidget(contenedor);

    // Botón usar
    botonUsar = new QPushButton("⚔️ Usar Objeto", this);
    botonUsar->setFixedHeight(45);
    botonUsar->setStyleSheet(
        "QPushButton {"
        "background-color: #8b4513;"
        "color: #f0e68c;"
        "font-family: 'Papyrus';"
        "font-size: 18px;"
        "border: 3px solid #b8860b;"
        "border-radius: 10px;"
        "padding: 5px;"
        "}"
        "QPushButton:hover { background-color: #a0522d; }"
        );
    layoutPrincipal->addWidget(botonUsar);

    connect(botonUsar, &QPushButton::clicked, this, &InventarioWidget::usarObjetoSeleccionado);

    actualizarVista();
}

//--- Dibujar fondo correctamente si tiene stylesheet ---
void InventarioWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    QPixmap bg("Sprites/Castle/Inventario.jpg");
    p.drawPixmap(rect(), bg);

    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void InventarioWidget::actualizarVista() {
    QLayoutItem* child;
    while ((child = layoutContenedor->takeAt(0)) != nullptr) {
        if (child->widget()) {
            mapaItems.remove(child->widget());
            child->widget()->removeEventFilter(this);
            delete child->widget();
        }
        delete child;
    }

    seleccionadoActual.clear();
    mostrarObjetos();
}

void InventarioWidget::mostrarObjetos() {
    QStringList ordenRarezas = { "Legendario", "Epico", "Raro", "Comun" };

    for (const QString& rareza : ordenRarezas) {
        QVector<Objeto*> lista;
        inventario->recogerPorRareza(rareza, lista);
        for (Objeto* obj : lista) {
            QWidget* w = crearWidgetObjeto(*obj);
            mapaItems[w] = obj->nombre;
            layoutContenedor->addWidget(w);
        }
    }
}

QWidget* InventarioWidget::crearWidgetObjeto(const Objeto& obj) {
    QWidget* card = new QWidget(this);
    QHBoxLayout* layoutCard = new QHBoxLayout(card);
    layoutCard->setContentsMargins(12, 12, 12, 12);
    layoutCard->setSpacing(15);

    // Imagen centrada
    QLabel* imagen = new QLabel(card);
    QPixmap pix(obj.RutaImagen);
    pix = pix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imagen->setPixmap(pix);
    imagen->setAlignment(Qt::AlignCenter);
    imagen->setFixedSize(80, 80);

    QString color;
    if (obj.rareza == "Legendario") color = "#FFD700";
    else if (obj.rareza == "Epico") color = "#800080";
    else if (obj.rareza == "Raro") color = "#1E90FF";
    else color = "#228B22";

    imagen->setStyleSheet(QString(
                              "border: 3px solid %1;"
                              "border-radius: 10px;"
                              "background-color: rgba(20, 20, 20, 120);").arg(color));

    QLabel* info = new QLabel(card);
    info->setText(QString("<b>%1</b><br>"
                          "<i>Tipo:</i> %2<br>"
                          "<i>Rareza:</i> %3<br>"
                          "<i>Descripcion:</i> %4<br>"
                          "<i>Cantidad:</i> x%5")
                      .arg(obj.nombre)
                      .arg(obj.tipo)
                      .arg(obj.rareza)
                      .arg(obj.Descripcion)
                      .arg(obj.cantidad));
    info->setStyleSheet(
        "color: #f5deb3;"
        "font-family: 'Papyrus';"
        "font-size: 14px;"
        "background: transparent;"
        );

    layoutCard->addWidget(imagen, 0, Qt::AlignCenter);
    layoutCard->addWidget(info);

    card->installEventFilter(this);
    card->setStyleSheet(
        "background-color: rgba(50, 30, 10, 180);"
        "border: 2px solid #8b4513;"
        "border-radius: 10px;"
        );

    return card;
}

bool InventarioWidget::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget* widget = qobject_cast<QWidget*>(watched);
        if (widget && mapaItems.contains(widget)) {
            seleccionadoActual = mapaItems[widget];
            qDebug() << "Seleccionaste:" << seleccionadoActual;

            for (auto it = mapaItems.begin(); it != mapaItems.end(); ++it) {
                if (it.key() == widget) {
                    it.key()->setStyleSheet(
                        "background-color: rgba(100, 60, 20, 200);"
                        "border: 3px solid #FFD700;"
                        "border-radius: 10px;"
                        );
                } else {
                    it.key()->setStyleSheet(
                        "background-color: rgba(50, 30, 10, 180);"
                        "border: 2px solid #8b4513;"
                        "border-radius: 10px;"
                        );
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void InventarioWidget::usarObjetoSeleccionado() {
    if (seleccionadoActual.isEmpty()) {
        qDebug() << "No has seleccionado ningún objeto.";
        return;
    }

    Objeto* obj = inventario->obtenerObjeto(seleccionadoActual);
    if (!obj) {
        qDebug() << "El objeto no existe en el inventario.";
        return;
    }

    if (obj->tipo == "Curacion") {
        inventario->usarObjeto(obj->nombre);
        actualizarVista();
        this->close();
    } else {
        qDebug() << obj->nombre << "no se puede usar.";
    }
}
