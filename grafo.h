#ifndef GRAFOMAPA_H
#define GRAFOMAPA_H

#include <QMap>
#include <QString>
#include <QLabel>
#include <QSet>
#include <QList>

class GrafoMapa {
public:
    GrafoMapa();

    // Métodos para nodos
    void agregarZona(const QString& nombreZona);
    bool existeZona(const QString& nombreZona) const;
    QList<QString> obtenerZonas() const;

    // Métodos para aristas
    void agregarConexion(const QString& origen, const QString& destino, int peso, bool esPermanente = true);
    bool existeConexion(const QString& origen, const QString& destino) const;
    int pesoConexion(const QString& origen, const QString& destino) const;
    QList<QString> obtenerAdyacentes(const QString& zona) const;
    QList<QPair<QString, int>> obtenerConexiones(const QString& zona) const;
    QList<QString> rutaMasCorta(const QString& origen, const QString& destino) const;
    QList<QList<QString>> todasLasRutas(const QString& origen, const QString& destino) const;



    // Explorador
    void activarConexionCondicional(const QString& origen, const QString& destino);
    void desactivarConexionCondicional(const QString& origen, const QString& destino);

    bool esConexionPermanente(const QString& origen, const QString& destino) const;
    bool esConexionActiva(const QString& origen, const QString& destino) const;

private:
    struct Conexion {
        int peso;
        bool esPermanente;
        bool activa;
    };

    QMap<QString, QMap<QString, Conexion>> grafo;
    QSet<QString> zonas;




};

#endif // GRAFOMAPA_H
