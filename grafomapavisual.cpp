#include "grafomapavisual.h"
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QMouseEvent>

MapaWidget::MapaWidget(QWidget *parent) : QWidget(parent), zonaActual("")
{
    setFixedSize(900, 600);

    if(!cargarFondo()) {
        fondoMapa = QPixmap(size());
        fondoMapa.fill(Qt::darkGray);
    }

    inicializarGrafo();
    crearInterfaz();
}

void MapaWidget::inicializarGrafo()
{
    // Agregar zonas
    grafo.agregarZona("Lobby");
    grafo.agregarZona("Pasillo Izq");
    grafo.agregarZona("Pasillo Der");
    grafo.agregarZona("Ruleta");
    grafo.agregarZona("Combate");
    grafo.agregarZona("Desicion");
    grafo.agregarZona("Kant / Descartes");

    // Conexiones permanentes
    grafo.agregarConexion("Lobby", "Pasillo Izq", 40);
    grafo.agregarConexion("Pasillo Izq", "Lobby", 40);

    grafo.agregarConexion("Lobby", "Pasillo Der", 40);
    grafo.agregarConexion("Pasillo Der", "Lobby", 40);

    grafo.agregarConexion("Ruleta", "Pasillo Der", 55);
    grafo.agregarConexion("Pasillo Der", "Ruleta", 55);

    grafo.agregarConexion("Kant / Descartes", "Pasillo Izq", 25);
    grafo.agregarConexion("Pasillo Izq", "Kant / Descartes", 25);

    grafo.agregarConexion("Desicion", "Pasillo Izq", 35);
    grafo.agregarConexion("Pasillo Izq", "Desicion", 35);

    grafo.agregarConexion("Desicion", "Combate", 15);
    grafo.agregarConexion("Combate", "Desicion", 15);


    grafo.agregarConexion("Desicion", "Ruleta", 25);
    grafo.agregarConexion("Ruleta", "Desicion", 25);



}

bool MapaWidget::cargarFondo()
{
    bool cargado = fondoMapa.load("Sprites/Castle/Mapaa.png");
    if (cargado) {
        fondoMapa = fondoMapa.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    return cargado;
}

void MapaWidget::crearInterfaz()
{
    btnRegresar = new QPushButton("Regresar", this);
    btnRegresar->setGeometry(10, 10, 100, 30);
    connect(btnRegresar, &QPushButton::clicked, this, [=]() {
        emit cerrarMapa();
        this->close();
    });

    lblZonaActual = new QLabel("Selecciona una zona", this);
    lblZonaActual->setGeometry(120, 10, 300, 30);
    lblZonaActual->setStyleSheet("color: white; font-weight: bold;");

    lblConexiones = new QLabel(this);
    lblConexiones->setGeometry(10, 550, 880, 40);
    lblConexiones->setStyleSheet("color: white; background-color: rgba(0,0,0,150);");
    lblConexiones->setAlignment(Qt::AlignCenter);

    lblTodasRutas = new QLabel(this);
    lblTodasRutas->setGeometry(10, 650, 780, 40);
    lblTodasRutas->setStyleSheet("color: white; background-color: rgba(0,0,0,150); font-size: 12px;");
    lblTodasRutas->setAlignment(Qt::AlignCenter);
    lblTodasRutas->setWordWrap(true);

}

QPoint MapaWidget::obtenerPosicionZona(const QString& zona) const
{
    static const QMap<QString, QPoint> posiciones = {
        {"Lobby", QPoint(483 , 430)},
        {"Pasillo Izq", QPoint(240  ,  435)},
        {"Pasillo Der", QPoint(729  ,  358)},
        {"Ruleta", QPoint(505  ,  237)},
        {"Combate", QPoint(135  ,  204)},
        {"Desicion", QPoint(273  ,  254)},
        {"Kant / Descartes",QPoint(97  ,  469)}
    };
    return posiciones.value(zona, QPoint(50, 50));
}

void MapaWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    int x = (width() - fondoMapa.width()) / 2;
    int y = (height() - fondoMapa.height()) / 2;
    painter.drawPixmap(x, y, fondoMapa);

    dibujarGrafo(painter);


}

void MapaWidget::dibujarGrafo(QPainter &painter)
{
    painter.setRenderHint(QPainter::Antialiasing);

    for(const QString& origen : grafo.obtenerZonas()) {
        for(const auto& conexion : grafo.obtenerConexiones(origen)) {
            QString destino = conexion.first;
            int peso = conexion.second;

            QPoint p1 = obtenerPosicionZona(origen);
            QPoint p2 = obtenerPosicionZona(destino);

            bool esPermanente = grafo.esConexionPermanente(origen, destino);
            bool estaActiva = grafo.esConexionActiva(origen, destino);

            QPen pen;
            if(esPermanente) {
                pen.setColor(Qt::cyan);  // Conexión permanente
                pen.setWidth(2);
            } else if(estaActiva) {
                pen.setColor(Qt::green);  // Conexión condicional activa verde
                pen.setWidth(2);
                pen.setStyle(Qt::DashLine);
            } else {
                continue;  // No dibujar conexiones condicionales inactivas
            }

            painter.setPen(pen);
            painter.drawLine(p1, p2);

            // Dibujar peso
            QPoint centro = (p1 + p2) / 2;
            painter.setPen(Qt::white);
            painter.drawText(centro, QString::number(peso) + " m");
        }
    }

    if (rutaActual.size() >= 2) {
        QPen rutaPen(Qt::yellow, 4, Qt::SolidLine);
        painter.setPen(rutaPen);

        for (int i = 0; i < rutaActual.size() - 1; ++i) {
            if (rutaActual[i].startsWith("(") || rutaActual[i + 1].startsWith("("))
                continue;

            QPoint p1 = obtenerPosicionZona(rutaActual[i]);
            QPoint p2 = obtenerPosicionZona(rutaActual[i + 1]);
            painter.drawLine(p1, p2);
        }
    }


    // Dibujar zonas
    for(const QString& zona : grafo.obtenerZonas()) {
        QPoint centro = obtenerPosicionZona(zona);

        // Dibujar círculo de zona
        painter.setBrush(zona == zonaActual ? Qt::yellow : Qt::blue);
        painter.setPen(QPen(Qt::white, 2));
        painter.drawEllipse(centro, 20, 20);

        // Dibujar nombre
        painter.setPen(Qt::white);
        painter.drawText(centro.x() - 30, centro.y() - 25, zona);
    }
}

void MapaWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint clickPos = event->pos();
    qDebug() << clickPos.x() << " , " << clickPos.y();


    for(const QString& zona : grafo.obtenerZonas()) {
        QPoint centro = obtenerPosicionZona(zona);
        if(QVector2D(clickPos - centro).length() <= 20) {

            if (zonaInicio.isEmpty()) {
                zonaInicio = zona;
                lblZonaActual->setText("Inicio: " + zonaInicio);
            } else {
                zonaFin = zona;
                lblZonaActual->setText("Destino: " + zonaFin);

                rutaActual = grafo.rutaMasCorta(zonaInicio, zonaFin);


                if (rutaActual.isEmpty())
                    lblConexiones->setText("");

                if (!rutaActual.isEmpty()) {
                    lblConexiones->setText("Ruta más corta: " + rutaActual.join(" -> "));

                    QList<QList<QString>> rutas = grafo.todasLasRutas(zonaInicio, zonaFin);
                    QStringList rutasTexto;
                    for (const QList<QString>& r : rutas) {
                        rutasTexto << r.join(" -> ");
                    }
                    lblTodasRutas->setText("El Resto de Rutas:\n" + rutasTexto.join("\n"));
                } else {
                    lblConexiones->setText("No hay ruta disponible.");
                    lblTodasRutas->setText("");
                }


                zonaInicio.clear();
                zonaFin.clear();
            }

            update();
            break;
        }
    }


    QWidget::mousePressEvent(event);
}

