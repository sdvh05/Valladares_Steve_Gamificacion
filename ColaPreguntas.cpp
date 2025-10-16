#include "ColaPreguntas.h"

ColaPreguntas::ColaPreguntas(bool circular) : esCircular(circular) {}

void ColaPreguntas::enqueue(const Pregunta& pregunta) {
    cola.push(pregunta);
}

Pregunta ColaPreguntas::dequeue() {
    if (cola.empty()) {
        return Pregunta();
    }

    Pregunta pregunta = cola.front();
    cola.pop();
    return pregunta;
}

Pregunta ColaPreguntas::dequeueCircular() {
    if (cola.empty()) {
        return Pregunta();
    }

    Pregunta pregunta = cola.front();
    cola.pop();

    if (esCircular) {
        cola.push(pregunta); // Reencolar para modo circular
    }

    return pregunta;
}

bool ColaPreguntas::isEmpty() const {
    return cola.empty();
}

int ColaPreguntas::size() const {
    return cola.size();
}

Pregunta ColaPreguntas::front() const {
    if (cola.empty()) {
        return Pregunta();
    }
    return cola.front();
}

void ColaPreguntas::clear() {
    while (!cola.empty()) {
        cola.pop();
    }
}

void ColaPreguntas::cargarPreguntasPorTipo(const QString& tipo) {
    if (tipo == "arte") {
        cargarPreguntasArte();
    } else if (tipo == "politica") {
        cargarPreguntasPolitica();
    } else if (tipo == "ciencia") {
        cargarPreguntasCiencia();
    } else if (tipo == "historia") {
        cargarPreguntasHistoria();
    } else if (tipo == "epistemologia") {
        cargarPreguntasEpistemologia();
    } else if (tipo == "kant") {
        cargarPreguntasKant();
    } else if (tipo == "descartes") {
        cargarPreguntasDescartes();
    }
}

// Implementación de las funciones de carga de preguntas
void ColaPreguntas::cargarPreguntasArte() {
    clear();

    enqueue(Pregunta(
        "Uno de los siguientes personajes fue el encargado de pintar la capilla Sixtina:",
        "Miguel Ángel", "Donatello", "Leonardo Da Vinci", "Francis Bacon",
        'A', "arte"
        ));

    enqueue(Pregunta(
        "Genio del renacimiento que esculpió el Moisés, el David y la Pietá:",
        "Miguel Ángel Buonarroti", "Leonardo Da Vinci", "Rafael Sanzio", "Galileo Galilei",
        'A', "arte"
        ));

    // ...
}

void ColaPreguntas::cargarPreguntasPolitica() {
    clear();

    enqueue(Pregunta(
        "Durante el renacimiento, el modelo de gobierno es uno de los siguientes:",
        "Monarquía absoluta", "Tiranía republicana", "Democracia participativa", "Liberalismo político",
        'A', "politica"
        ));

    enqueue(Pregunta(
        "De los siguientes acontecimientos, seleccione el que inicia el período moderno:",
        "Toma de Constantinopla", "Tratado de paz de westfalia", "Toma de la Bastilla", "La ruta de la seda",
        'A', "politica"
        ));

    // ...
}

void ColaPreguntas::cargarPreguntasCiencia() {
    clear();

    enqueue(Pregunta(
        "Entre los siguientes renacentistas seleccione uno de los precursores filósofo-científico del heliocentrismo:",
        "Tomas Moro", "Galileo", "Platón", "Arquimedes",
        'B', "ciencia"
        ));

    // ...
}

void ColaPreguntas::cargarPreguntasHistoria() {
    clear();

    enqueue(Pregunta(
        "Después del feudalismo medieval acudimos al surgimiento de una nueva clase social conocida como la:",
        "La monarquía", "El mercantilismo", "La burguesía", "El proletariado",
        'C', "historia"
        ));

    // ...
}

void ColaPreguntas::cargarPreguntasEpistemologia() {
    clear();

    enqueue(Pregunta(
        "Para algunos de los siguientes filósofos, el criterio de verdad es la evidencia sensible:",
        "Empiristas", "Criticistas", "Racionalistas", "Dogmáticos",
        'A', "epistemologia"
        ));

    enqueue(Pregunta(
        "Filósofos para quienes la única fuente del conocimiento es la razón:",
        "Epistemólogos", "Racionalistas", "Empiristas", "Escépticos",
        'B', "epistemologia"
        ));

    // ...
}

void ColaPreguntas::cargarPreguntasKant() {
    clear();

    enqueue(Pregunta(
        "Seleccione el mandato cuya obligación viene del miedo al castigo o la búsqueda de un premio:",
        "Imperativo Hipotético", "Imperativo categórico", "Ambos", "Ninguno",
        'A', "kant"
        ));

    enqueue(Pregunta(
        "Para Emanuel Kant, es posible conocer lo que las cosas nos permiten a través de nuestros sentidos:",
        "Conocimiento Noumenico", "Conocimiento fenoménico", "Conocimiento Empírico", "Conocimiento Racional",
        'B', "kant"
        ));

    // ...
}

void ColaPreguntas::cargarPreguntasDescartes() {
    clear();

    enqueue(Pregunta(
        "Son los pasos o razones de la duda metódica:",
        "Los sentidos nos engañan", "La existencia del genio maligno",
        "Imposibilidad para diferenciar la vigilia del sueño", "Todas son correctas",
        'D', "descartes"
        ));

    enqueue(Pregunta(
        "Una de las siguientes es considerada de las cuatro reglas del método en Descartes:",
        "Hipótesis", "Deducción", "Evidencia", "Inducción",
        'C', "descartes"
        ));

    // ...
}
