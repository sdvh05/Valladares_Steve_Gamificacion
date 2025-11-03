#ifndef INVENTARIOWIDGET_H
#define INVENTARIOWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QMap>
#include <QPaintEvent>
#include "Inventario.h"

class InventarioWidget : public QWidget {
    Q_OBJECT

private:
    Inventario* inventario;
    QVBoxLayout* layoutPrincipal;
    QScrollArea* scrollArea;
    QWidget* contenedor;
    QVBoxLayout* layoutContenedor;
    QPushButton* botonUsar;
    QString seleccionadoActual;
    QMap<QWidget*, QString> mapaItems;

    void mostrarObjetos();
    QWidget* crearWidgetObjeto(const Objeto& obj);
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void usarObjetoSeleccionado();

protected:
    void paintEvent(QPaintEvent* event) override;

public:
    explicit InventarioWidget(Inventario* inv, QWidget* parent = nullptr);
    void actualizarVista();

signals:
    void objetoUsado(const Objeto& objeto);
};

#endif // INVENTARIOWIDGET_H
