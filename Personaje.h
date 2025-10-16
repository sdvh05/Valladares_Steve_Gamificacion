#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QFile>
#include <QMap>
#include <QStringList>
#include <QRect>

class Personaje : public QLabel
{
    Q_OBJECT

public:
    explicit Personaje(QWidget* parent = nullptr);

    struct DatosAnimacion {
        QString ruta;
        int frames;
    };

    bool miradoDerecha;
    void SetAnimacion(const QString& ruta, int cantidadFrames);
    void AvanzarFrame();
    void MoverIzquierda();
    void MoverDerecha();
    void MoverArriba();
    void MoverAbajo();
    void DetenerAnimacion();

    void MoverSiNoColisiona(int dx, int dy, const QVector<QRect>& obstaculos);
    void SetAnimacionMovimiento(int velocidad);
    int getVelocidadMovimiento() const { return velocidadMovimiento; }

    DatosAnimacion obtenerAnimacion(const QString& tipo);
    QString animacionActual;

    void setVidas(int vidas) {
        vida = vidas;
    }
    int getVidas() { return vida; }


private:

    QVector<QPixmap> frames;
    int frameActual;
    QTimer* timer;

    bool ultimaDireccionDerecha;


    bool enPausa=false;

    QVector<QRect> obstaculos;
    int vida;
    int velocidadMovimiento;

};



#endif // PERSONAJE_H
