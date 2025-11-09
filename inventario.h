#ifndef INVENTARIO_H
#define INVENTARIO_H


#include <QString>
#include <QDebug>

struct Objeto {
    QString nombre; //nombre del objeto
    QString rareza;//eliminar rareza ->
    QString tipo;
    QString Descripcion;
    QString RutaImagen;
    int cantidad;
    int valor;

    Objeto(QString n, QString r, QString t, QString D, QString  ri, int c, int v)
        : nombre(n),rareza(r), tipo(t), Descripcion(D),RutaImagen(ri),cantidad(c), valor(v) {}
};

class NodoInventario {
public:
    Objeto obj;
    NodoInventario* izq;
    NodoInventario* der;

    NodoInventario(const Objeto& o) : obj(o), izq(nullptr), der(nullptr) {}
};



class Inventario {
private:
    NodoInventario* raiz;
    void destruir(NodoInventario* nodo);
    NodoInventario* insertar(NodoInventario* nodo, const Objeto& obj);
    Objeto* buscar(NodoInventario* nodo, const QString& nombre);

    void recogerObjetosRec(NodoInventario* nodo, QVector<Objeto*>& lista) const;
    void recogerPorTipoRec(NodoInventario* nodo, const QString& tipo, QVector<Objeto*>& lista) const;
    void recogerPorRarezaRec(NodoInventario* nodo, const QString& rareza, QVector<Objeto*>& lista) const;

public:
    Inventario();
    ~Inventario();

    void agregarObjeto(const Objeto& obj);
    Objeto* obtenerObjeto(const QString& nombre);
    void usarObjeto(const QString& nombre);
    bool gastarDinero(int cantidad);

    void recogerObjetos(QVector<Objeto*>& lista) const;
    void recogerPorTipo(const QString& tipo, QVector<Objeto*>& lista) const;
    void recogerPorRareza(const QString& rareza, QVector<Objeto*>& lista) const;

    NodoInventario* getRaiz() const;

public: //Agregar items especificos

    void agregarLlave(int valor=1);
    void agregarDinero(int valor=1);

    void agregarMedallaCiencia(int valor=1);
    void agregarMedallaPolitica(int valor=1);
    void agregarMedallaArte(int valor=1);
    void agregarMedallaHistoria(int valor=1);
    void agregarMedallaCombate(int valor=1);

    void agregarMedallaKant(int valor=1);
    void agregarMedallaDescartes(int valor=1);

    void agregarVida(int valor=1);
    void agregarCorona(int valor=1);

    void agregarMapa(int valor=1);
    void agregarBanner(int tipo, int valor=1);




};


#endif // INVENTARIO_H
