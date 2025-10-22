#ifndef RULETAWIDGET_H
#define RULETAWIDGET_H

#include "interior.h"

#include <QWidget>
#include <QList>
#include <QTimer>
#include <QLabel>
#include <QPushButton>

class RuletaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RuletaWidget(const QList<bool>& puertas, int indice, Interior* padre = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void girarRuleta();
    void detenerRuleta();
    void cerrarRuleta();

private:
    QList<bool> opcionesDisponibles;
    Interior* escenaPadre;
    int resultadoFinal;               // 0=Arte,1=Política,2=Ciencia,3=Historia
    QTimer* timerGiro;
    double anguloActual;
    int velocidad;
    bool girando;
    QLabel* lblResultado;
    QPushButton* btnCerrar;

    double anguloObjetivo=0.0;
    double anguloInicial = 0.0;
    bool objetivoCalculado = false;
    bool alcanzandoObjetivo;

    QString obtenerTextoCategoria(int indice) const;
    QColor obtenerColorCategoria(int indice) const;
};
#endif // RULETAWIDGET_H
