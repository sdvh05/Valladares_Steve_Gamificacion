#ifndef GRAFOMAPAVISUAL_H
#define GRAFOMAPAVISUAL_H

#include <QWidget>
#include <QPixmap>
#include <QPushButton>
#include <QLabel>
#include "grafo.h"

class MapaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapaWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QPixmap fondoMapa;
    GrafoMapa grafo;
    QString zonaActual;
    QString zonaInicio;
    QString zonaFin;



    QPushButton *btnRegresar;
    QLabel *lblZonaActual;
    QLabel *lblConexiones;

    void crearInterfaz();
    bool cargarFondo();
    void inicializarGrafo();
    void dibujarGrafo(QPainter &painter);
    QPoint obtenerPosicionZona(const QString& zona) const;
    QList<QString> rutaActual;
    QLabel* lblTodasRutas;

signals:
    void cerrarMapa();

};

#endif // GRAFOMAPAVISUAL_H
