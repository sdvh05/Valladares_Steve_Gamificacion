#ifndef TABLEROPUNTOS_H
#define TABLEROPUNTOS_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMap>
#include <QVector>

struct Registro {
    QString nombre;
    int puntos;
};

class TableroPuntos : public QWidget
{
    Q_OBJECT

public:
    explicit TableroPuntos(QWidget *parent = nullptr);
    void cargarRegistros();
    void mostrarRegistros();
    void ordenarPorNombre();   // Inserción
    void ordenarPorPuntos();   // QuickSort

private slots:
    void onOrdenarPorNombre();
    void onOrdenarPorPuntos();

private:
    QVector<Registro> registros;

    QLabel *lblTitulo;
    QLabel *lblTabla;
    QPushButton *btnAlfabetico;
    QPushButton *btnPuntos;

    QVBoxLayout *layoutPrincipal;
    QString rutaArchivo;

    // Métodos auxiliares
    void quickSort(int inicio, int fin);
    int particion(int inicio, int fin);

    bool ordenDescendentePuntos = false;
    bool ordenAscendenteNombre = true;

};

#endif // TABLEROPUNTOS_H
