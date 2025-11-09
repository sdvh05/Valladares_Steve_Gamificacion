#include "TableroPuntos.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QPixmap>
#include <QPalette>
#include <QDebug>

// Constructor
TableroPuntos::TableroPuntos(QWidget *parent) : QWidget(parent)
{
    rutaArchivo = QDir::currentPath() + "/Puntos/registro.txt";

    setFixedSize(600, 500);

    // Fondo
    QPixmap fondo("Sprites/Castle/Inventario.jpg");
    QPalette paleta;
    paleta.setBrush(QPalette::Window, fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setAutoFillBackground(true);
    setPalette(paleta);

    // Título
    lblTitulo = new QLabel("🏰 TABLERO DEL CONOCIMIENTO 🏰", this);
    lblTitulo->setAlignment(Qt::AlignCenter);
    lblTitulo->setStyleSheet(
        "color: #f9f3d2;"
        "font: bold 22px 'Papyrus';"
        "border: 2px solid #b08a57;"
        "background: rgba(40,20,0,120);"
        "border-radius: 10px;"
        "padding: 8px;"
        );

    // abla (texto con registros)
    lblTabla = new QLabel(this);
    lblTabla->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    lblTabla->setStyleSheet(
        "color: #fcebb6;"
        "font: 16px 'Courier New';"
        "background: rgba(80,50,20,150);"
        "border: 2px solid #b08a57;"
        "border-radius: 8px;"
        "padding: 10px;"
        );
    lblTabla->setFixedHeight(350);

    // Botones
    btnAlfabetico = new QPushButton("Alfabético", this);
    btnPuntos = new QPushButton("Puntos", this);

    QString estiloBoton =
        "QPushButton {"
        "background-color: #b08a57;"
        "color: white;"
        "font: bold 14px 'Papyrus';"
        "border: 2px solid #5a3d1e;"
        "border-radius: 8px;"
        "padding: 6px 20px;"
        "}"
        "QPushButton:hover { background-color: #d4a05a; }";

    btnAlfabetico->setStyleSheet(estiloBoton);
    btnPuntos->setStyleSheet(estiloBoton);

    QHBoxLayout *layoutBotones = new QHBoxLayout;
    layoutBotones->addWidget(btnAlfabetico);
    layoutBotones->addWidget(btnPuntos);

    layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->addWidget(lblTitulo);
    layoutPrincipal->addWidget(lblTabla);
    layoutPrincipal->addLayout(layoutBotones);
    layoutPrincipal->setAlignment(Qt::AlignCenter);
    layoutPrincipal->setContentsMargins(20, 20, 20, 20);

    setLayout(layoutPrincipal);

    // Conexiones
    connect(btnAlfabetico, &QPushButton::clicked, this, &TableroPuntos::onOrdenarPorNombre);
    connect(btnPuntos, &QPushButton::clicked, this, &TableroPuntos::onOrdenarPorPuntos);

    // Cargar datos
    cargarRegistros();
    ordenarPorPuntos();
    mostrarRegistros();
}

// Cargar registros
void TableroPuntos::cargarRegistros()
{
    registros.clear();
    QFile archivo(rutaArchivo);

    if (!archivo.exists()) {
        qDebug() << "Archivo no encontrado:" << rutaArchivo;
        return;
    }

    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&archivo);
        while (!in.atEnd()) {
            QString linea = in.readLine();
            QStringList partes = linea.split(",");
            if (partes.size() == 2) {
                Registro r;
                r.nombre = partes[0].trimmed();
                r.puntos = partes[1].trimmed().toInt();
                registros.append(r);
            }
        }
        archivo.close();
    }
}


void TableroPuntos::mostrarRegistros()
{
    QString texto = "<pre>RANK  NOMBRE         PUNTOS\n";
    texto += "--------------------------------\n";

    int rank = 1;
    for (const Registro &r : registros) {
        QString nombreAlineado = r.nombre.leftJustified(12, ' ');
        QString linea = QString("%1.   %2   %3\n")
                            .arg(rank++, 2)
                            .arg(nombreAlineado)
                            .arg(r.puntos, 5);
        texto += linea;
    }

    texto += "</pre>";
    lblTabla->setText(texto);
}

// Inserción
void TableroPuntos::ordenarPorNombre()
{
    ordenDescendentePuntos = false;
    for (int i = 1; i < registros.size(); ++i) {
        Registro key = registros[i];
        int j = i - 1;
        while (j >= 0 && registros[j].nombre.compare(key.nombre, Qt::CaseInsensitive) > 0) {
            registros[j + 1] = registros[j];
            j--;
        }
        registros[j + 1] = key;
    }
}


// QuickSort
int TableroPuntos::particion(int inicio, int fin)
{
    int pivote = registros[fin].puntos;
    int i = inicio - 1;
    for (int j = inicio; j < fin; ++j) {
        if (registros[j].puntos > pivote) {
            i++;
            std::swap(registros[i], registros[j]);
        }
    }
    std::swap(registros[i + 1], registros[fin]);
    return i + 1;
}

void TableroPuntos::quickSort(int inicio, int fin)
{
    if (inicio < fin) {
        int pi = particion(inicio, fin);
        quickSort(inicio, pi - 1);
        quickSort(pi + 1, fin);
    }
}

void TableroPuntos::ordenarPorPuntos()
{
    ordenAscendenteNombre = true;
    if (registros.size() > 1)
        quickSort(0, registros.size() - 1);
}


void TableroPuntos::onOrdenarPorPuntos()
{
    if (registros.isEmpty()) return;

    // Si ya estaba descendente, invertimos
    if (ordenDescendentePuntos) {
        ordenarPorPuntos();
        std::reverse(registros.begin(), registros.end());
        ordenDescendentePuntos = false;
    } else {
        ordenarPorPuntos();
        ordenDescendentePuntos = true;
    }

    mostrarRegistros();
}

void TableroPuntos::onOrdenarPorNombre(){
    if (registros.isEmpty()) return;

    ordenarPorNombre();

    // Si ya estaba ascendente invertimos
    if (!ordenAscendenteNombre) {
        std::reverse(registros.begin(), registros.end());
    }

    // Alternar el estado
    ordenAscendenteNombre = !ordenAscendenteNombre;

    mostrarRegistros();
}
