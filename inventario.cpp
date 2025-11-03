#include "inventario.h"
#include <QDebug>

Inventario::Inventario() : raiz(nullptr) {}

// Destructor
Inventario::~Inventario() {
    destruir(raiz);
}

void Inventario::destruir(NodoInventario* nodo) {
    if (!nodo) return;
    destruir(nodo->izq);
    destruir(nodo->der);
    delete nodo;
}

// Inserción en ABB
NodoInventario* Inventario::insertar(NodoInventario* nodo, const Objeto& obj) {
    if (!nodo)
        return new NodoInventario(obj);

    if (obj.nombre < nodo->obj.nombre) {
        nodo->izq = insertar(nodo->izq, obj);
    } else if (obj.nombre > nodo->obj.nombre) {
        nodo->der = insertar(nodo->der, obj);
    } else {
        nodo->obj.cantidad += obj.cantidad;
    }
    return nodo;
}

void Inventario::agregarObjeto(const Objeto& obj) {
    raiz = insertar(raiz, obj);
}

// Búsqueda por nombre
Objeto* Inventario::buscar(NodoInventario* nodo, const QString& nombre) {
    if (!nodo) return nullptr;

    if (nombre < nodo->obj.nombre)
        return buscar(nodo->izq, nombre);
    else if (nombre > nodo->obj.nombre)
        return buscar(nodo->der, nombre);
    else
        return &nodo->obj;
}

Objeto* Inventario::obtenerObjeto(const QString& nombre) {
    return buscar(raiz, nombre);
}

// Uso de objeto (resta cantidad si es posible)
void Inventario::usarObjeto(const QString& nombre) {
    Objeto* obj = obtenerObjeto(nombre);
    if (obj && obj->cantidad > 0) {
        obj->cantidad--;
        qDebug() << "Usaste" << nombre << ". Quedan:" << obj->cantidad;
    } else {
        qDebug() << "No tienes" << nombre << "disponible.";
    }
}



// Recorrido en orden que guarda punteros a objetos
void Inventario::recogerObjetos(QVector<Objeto*>& lista) const {
    recogerObjetosRec(raiz, lista);
}

void Inventario::recogerObjetosRec(NodoInventario* nodo, QVector<Objeto*>& lista) const {
    if (!nodo) return;
    recogerObjetosRec(nodo->izq, lista);
    lista.append(&nodo->obj);
    recogerObjetosRec(nodo->der, lista);
}

void Inventario::recogerPorTipo(const QString& tipo, QVector<Objeto*>& lista) const {
    recogerPorTipoRec(raiz, tipo, lista);
}

void Inventario::recogerPorTipoRec(NodoInventario* nodo, const QString& tipo, QVector<Objeto*>& lista) const {
    if (!nodo) return;
    recogerPorTipoRec(nodo->izq, tipo, lista);
    if (nodo->obj.tipo == tipo)
        lista.append(&nodo->obj);
    recogerPorTipoRec(nodo->der, tipo, lista);
}

void Inventario::recogerPorRareza(const QString& rareza, QVector<Objeto*>& lista) const {
    recogerPorRarezaRec(raiz, rareza, lista);
}

void Inventario::recogerPorRarezaRec(NodoInventario* nodo, const QString& rareza, QVector<Objeto*>& lista) const {
    if (!nodo) return;
    recogerPorRarezaRec(nodo->izq, rareza, lista);
    if (nodo->obj.rareza == rareza)
        lista.append(&nodo->obj);
    recogerPorRarezaRec(nodo->der, rareza, lista);
}

// Permitir acceso al árbol
NodoInventario* Inventario::getRaiz() const {
    return raiz;
}

bool Inventario::gastarDinero(int cantidad) {
    Objeto* monedas = obtenerObjeto("Monedas");

    if (monedas && monedas->cantidad >= cantidad) {
        monedas->cantidad -= cantidad;
        qDebug() << "Gastaste" << cantidad << "monedas. Quedan:" << monedas->cantidad;
        return true;
    } else {
        qDebug() << "No tienes suficiente dinero. Tienes:"
                 << (monedas ? monedas->cantidad : 0)
                 << ", necesitas:" << cantidad;
        return false;
    }
}


//----------------------------------------

void Inventario::agregarLlave(int cantidad){
    agregarObjeto(Objeto("LLave", "Epico", "Badge", "LLave de una Peculiar Forma, \n puede ser usada para abrir algo",
                         "Sprites/Recursos/Golden Key.png", cantidad, 1));
}
void Inventario::agregarMedallaCiencia(int cantidad){
    agregarObjeto(Objeto("Medalla Ciencia", "Legendario", "Badge", "Medalla Otorgada por ganar el Minijuego Ciencia",
                         "Sprites/Recursos/Bagde Ciencia.png", cantidad, 1));
}
void Inventario::agregarMedallaPolitica(int cantidad){
    agregarObjeto(Objeto("Medalla Politica", "Comun", "Badge", "Medalla Otorgada por ganar el Minijuego Politica",
                         "Sprites/Recursos/Badge Politica.png", cantidad, 20));
}
void Inventario::agregarMedallaArte(int cantidad){
    agregarObjeto(Objeto("Medalla Arte", "Comun", "Badge","Medalla Otorgada por ganar el Minijuego Arte",
                         "Sprites/Recursos/Bagde Arte.png", cantidad, 20));
}
void Inventario::agregarMedallaHistoria(int cantidad){
    agregarObjeto(Objeto("Medalla Historia", "Raro", "Badge", "Medalla Otorgada por ganar el Minijuego Historia",
                         "Sprites/Recursos/Badge Historia.png", cantidad, 60));
}
void Inventario::agregarMedallaKant(int cantidad){
    agregarObjeto(Objeto("Medalla Kant", "Raro", "Badge", "Medalla Otorgada por resolver el Misterio de Kant",
                         "Sprites/Recursos/Badge Kant.png", cantidad, 60));
}

void Inventario::agregarMedallaDescartes(int cantidad){
    agregarObjeto(Objeto("Medalla Descartes", "Epico", "Badge", "Medalla Otorgada por resolver el Misterio de Kant",
                         "Sprites/Recursos/Badge Descartes.png", cantidad, 1));
}


