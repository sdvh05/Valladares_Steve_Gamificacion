#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QFile>
#include <QTextStream>
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

    QList<bool> puertas = {false, false, false, false};
    // arte,politica.ciencia, historia...

    QList<bool> ganadas = {false, false, false, false};

    void MoverSiNoColisiona(int dx, int dy, const QVector<QRect>& obstaculos);
    void SetAnimacionMovimiento(int velocidad);
    int getVelocidadMovimiento() const { return velocidadMovimiento; }

    DatosAnimacion obtenerAnimacion(const QString& tipo);
    QString animacionActual;

    int getCorazones();
    void AumentarCorazones();
    void PerderCorazones();

    void setCorazones(int corazon) {
        corazones = corazon;
    }
    int getVidas() { return vida; }

    int Bando=0;
    bool tieneMapa=false;
    //bool tieneLLave=false;
    bool tieneLLave=true;

    //nuevos
    QString nombre;
    int puntos=0;

    void guardarPuntos(const QString& nombre, int puntos);

private:

    QVector<QPixmap> frames;
    int frameActual;
    QTimer* timer;

    bool ultimaDireccionDerecha;


    bool enPausa=false;

    QVector<QRect> obstaculos;
    int vida;
    int corazones = 0;
    int velocidadMovimiento;

    void PuertaSeleccionada(int indice);
};



#endif // PERSONAJE_H
