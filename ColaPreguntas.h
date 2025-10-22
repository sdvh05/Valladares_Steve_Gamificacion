#ifndef COLAPREGUNTAS_H
#define COLAPREGUNTAS_H

#include "Pregunta.h"
#include <queue>
#include <QString>

class ColaPreguntas
{
private:
    std::queue<Pregunta> cola;
    bool esCircular;

public:
    ColaPreguntas(bool circular = false);

    //Funciones
    void enqueue(const Pregunta& pregunta);
    Pregunta dequeue();
    bool isEmpty() const;
    int size() const;

    //Cargar (helper)
    void cargarPreguntasArte();
    void cargarPreguntasPolitica();
    void cargarPreguntasCiencia();
    void cargarPreguntasHistoria();

    void cargarPreguntasCombate();

    void cargarPreguntasEpistemologia();
    void cargarPreguntasKant();
    void cargarPreguntasDescartes();

    //Circular (Batalla)
    Pregunta dequeueCircular();

    //Cargar Por tipo
    void cargarPreguntasPorTipo(const QString& tipo);

    Pregunta front() const;
    void clear();
};

#endif // COLAPREGUNTAS_H
