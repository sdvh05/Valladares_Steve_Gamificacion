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

    //if (esCircular) {
        cola.push(pregunta); // Reencolar batalla
    //}

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
    if (tipo == "Arte") {
        cargarPreguntasArte();
    } else if (tipo == "Politica") {
        cargarPreguntasPolitica();
    } else if (tipo == "Ciencia") {
        cargarPreguntasCiencia();
    } else if (tipo == "Historia") {
        cargarPreguntasHistoria();
    } else if (tipo == "Combate") {
        cargarPreguntasCombate();
    } else if (tipo == "Kant") {
        cargarPreguntasKant();
    } else if (tipo == "Descartes") {
        cargarPreguntasDescartes();
    }
}

// CARGAR PREGUNTAS
//-------------------------------------------------------------------------------------------------------------------------
void ColaPreguntas::cargarPreguntasArte() {
    clear();

    enqueue(Pregunta(
        "Uno de los siguientes personajes fue el encargado de \n pintar la capilla Sixtina:",
        "Miguel Ángel", "Donatello", "Leonardo Da Vinci", "Francis Bacon",
        'A', "arte"
        ));

    enqueue(Pregunta(
        "Genio del renacimiento que esculpió \n el Moisés, el David y la Pietá:",
        "Miguel Ángel Buonarroti", "Leonardo Da Vinci", "Rafael Sanzio", "Galileo Galilei",
        'A', "arte"
        ));

    enqueue(Pregunta(
        "Durante el renacimiento el estilo artístico que impregnó \n el arte, la filosofía, la pintura escritura fue el:",
        "El Gótico", "El barroco", "El clasicismo", "Romanticismo",
        'B', "arte"
        ));

    enqueue(Pregunta(
        "Durante el renacimiento surge una nueva visión del hombre,\n que se vio reflejada en el arte, en la política "
        "y en las ciencias sociales y humanas, a lo que se denomina:",
        "Antropocentrismo", "Humanismo", "Paradigma antropológico", "Teocentrismo",
        'B', "arte"
        ));

    enqueue(Pregunta(
        "Cuatro genios del renacimiento (Leonardo, Donatello, Rafael y Michelangelo) \n han sido llevados a la "
        "pantalla en los comics de:",
        "Las tortugas ninjas", "Los caballeros del Zodiaco", "Los cuatro fantásticos", "Los antagonistas de Attack Titan",
        'A', "arte"
        ));
}

void ColaPreguntas::cargarPreguntasPolitica() {
    clear();

    enqueue(Pregunta(
        "Durante el renacimiento, el modelo de  \n gobierno es uno de los siguientes:",
        "Monarquía absoluta", "Tiranía republicana", "Democracia participativa", "Liberalismo político",
        'A', "politica"
        ));

    enqueue(Pregunta(
        "De los siguientes acontecimientos, seleccione  \n el que inicia el período moderno:",
        "Toma de Constantinopla", "Tratado de paz de westfalia", "Toma de la Bastilla", "La ruta de la seda",
        'A', "politica"
        ));

    enqueue(Pregunta(
        "Durante el siglo XV, la sociedad se estratifica \n en tres estamentos definidos:",
        "Clase media, baja y alta", "Nobleza, clero y estado llano", "Artesanos, guardianes y gobernantes", "Reyes, nobles y plebeyos",
        'B', "politica"
        ));

    enqueue(Pregunta(
        "Aparece el realismo político, que se basaba en un orden establecido,\n explicación de un sistema y recomendaciones de como gobernar:",
        "Tomás Moro", "Jean Bodín", "Nicolás Maquiavelo", "Erasmo de Rotterdam",
        'C', "politica"
        ));

    enqueue(Pregunta(
        "Terminada la edad media, en el contexto de la política resulta que:",
        "La Iglesia resalta su poder", "La Iglesia pierde el papel rector en la política", "La Iglesia evangélica se posiciona en la política", "La política desaparece",
        'B', "politica"
        ));
}


void ColaPreguntas::cargarPreguntasCiencia() {
    clear();

    enqueue(Pregunta(
        "Entre los siguientes renacentistas seleccione uno de \n los precursores filósofo-científico del heliocentrismo:",
        "Tomas Moro", "Galileo", "Platón", "Arquimedes",
        'B', "ciencia"
        ));

    enqueue(Pregunta(
        "El método científico se introduce por el interés de tres filósofos. \n Entre los siguientes uno de los mencionados no es precursor del método científico:",
        "Francis Bacon", "Galileo Galilei", "Nicolas Maquiavelo", "René Descartes",
        'C', "ciencia"
        ));

    enqueue(Pregunta(
        "Es uno de los precursores del pensamiento Moderno:",
        "Isaac Newton", "René Descartes", "Erasmo de Roterdam", "Francis Bacon",
        'B', "ciencia"
        ));

    enqueue(Pregunta(
        "De los siguientes filósofos niega el geocentrismo \n(teoría que afirma que el centro de nuestro sistema solar es la tierra):",
        "Aristóteles", "Nicolás Copérnico", "Tomás de Aquino", "Isaac Newton",
        'B', "ciencia"
        ));

    enqueue(Pregunta(
        "Uno de los inventos que suscitó un conocimiento \n ilimitado, fue el de Gutenberg:",
        "El astrolabio", "La imprenta", "La Nao y la Carabela", "El Telescopio",
        'B', "ciencia"
        ));
}


void ColaPreguntas::cargarPreguntasHistoria() {
    clear();

    enqueue(Pregunta(
        "Después del feudalismo medieval acudimos al surgimiento \n de una nueva clase social conocida como la:",
        "La monarquía", "El mercantilismo", "La burguesía", "El proletariado",
        'C', "historia"
        ));

    enqueue(Pregunta(
        "El renacimiento supone una época de absolutismo y nacionalismos, \n como el nacimiento de fuertes monarquías europeas centralizadas como:",
        "Grecia", "Inglaterra", "Yugoslavia", "Egipto",
        'B', "historia"
        ));

    enqueue(Pregunta(
        "Antes de la consolidación del estado moderno, Italia estuvo divida en pequeñas \n ciudades-estado normalmente enfrentadas entre si, como es el caso de:",
        "Florencia-Napoli", "Ámsterdam-Cracovia", "Reims-Colonia", "Milán-Lourdes",
        'A', "historia"
        ));

    enqueue(Pregunta(
        "La toma de Constantinopla supone un bloqueo comercial entre Europa y Asia \n (la ruta de la seda) y ocurrió en lo que hoy es actualmente:",
        "Eslovaquia", "Estambul en Turquía", "Mesopotamia", "Jerusalén",
        'B', "historia"
        ));

    enqueue(Pregunta(
        "Resurge el interés por Grecia y Roma, junto al declive del sistema feudal, \n el crecimiento del comercio e innovaciones entre las que mencionamos:",
        "La imprenta y la brújula", "La rueda y la escritura", "Las máquinas de vapor y la producción en masa", "La pólvora y La rueda",
        'A', "historia"
        ));
}
//-------------------------------------------------------------------------------------------------------------------------

void ColaPreguntas::cargarPreguntasCombate() {
    clear();

    enqueue(Pregunta(
        "Para algunos de los siguientes filósofos, el criterio de verdad es la evidencia sensible:",
        "Empiristas", "Criticistas", "Racionalistas", "Dogmáticos",
        'A', "combate"
        ));

    enqueue(Pregunta(
        "De las siguientes, una de ellas es la corriente filosófica que en general tiende a negar \n la posibilidad de la metafísica y a sostener que hay conocimiento únicamente de los fenómenos.",
        "Racionalistas", "Empiristas", "Escolásticos", "Escépticos",
        'B', "combate"
        ));

    enqueue(Pregunta(
        "Para unos de los siguientes filósofos, \n la experiencia como única fuente del conocimiento.",
        "Epistemólogos", "Racionalistas", "Empiristas", "Escépticos",
        'C', "combate"
        ));

    enqueue(Pregunta(
        "Filósofos para quienes la única fuente del conocimiento es la razón.",
        "Epistemólogos", "Racionalistas", "Empiristas", "Escépticos",
        'B', "combate"
        ));

    enqueue(Pregunta(
        "Filósofos que postulan las ideas innatas en el sujeto.",
        "Empiristas", "Idealistas", "Racionalistas", "Innatistas",
        'C', "combate"
        ));

    enqueue(Pregunta(
        "De los siguientes filósofos selecciones el \n que no se considera Racionalista:",
        "David Hume", "John Locke", "Nicolas Malebranch", "Francis Bacon",
        'C', "combate"
        ));

    enqueue(Pregunta(
        "Es la doctrina que establece que todos \n nuestros conocimientos provienen de la razón.",
        "Empirismo", "Criticismo", "Racionalismo", "Epistemología",
        'C', "combate"
        ));

    enqueue(Pregunta(
        "Uno de los siguientes filósofos, \n postula las ideas innatas en el sujeto:",
        "George Berkeley", "David Hume", "Leibniz", "Hipatía",
        'C', "combate"
        ));
}
//-------------------------------------------------------------------------------------------------------------------------



void ColaPreguntas::cargarPreguntasKant() {
    clear();

    enqueue(Pregunta(
        "1) Seleccione el mandato cuya obligación viene \n del miedo al castigo o la búsqueda de un premio:",
        "Imperativo Hipotético", "Imperativo categórico", "Ambos", "Ninguno",
        'A', "kant"
        ));

    enqueue(Pregunta(
        "2) Para Emanuel Kant, es posible conocer lo que las cosas \n nos permiten a través de nuestros sentidos:",
        "Conocimiento Noumenico", "Conocimiento fenoménico", "Conocimiento Empírico", "Conocimiento Racional",
        'B', "kant"
        ));

    enqueue(Pregunta(
        "3) Kant decía que el lema de la ilustración era \n 'Sapere aude', que significa:",
        "Sopesa tus acciones", "Atrévete a saber por ti mismo", "Saber a la fuerza", "Someterse al conocimiento",
        'B', "kant"
        ));

    enqueue(Pregunta(
        "4) Kant (igual que Copérnico cambió el centro del universo de la tierra al sol),\n cambia el centro del conocimiento del objeto al sujeto, a esto se le llama:",
        "Subjetivismo", "Prejuicio", "Giro copernicano", "Suerte",
        'C', "kant"
        ));

    enqueue(Pregunta(
        "5) La postura conciliadora de Kant respecto a los empiristas y racionalistas \n define que los datos experimentales son la fuente del conocimiento racional del sujeto:",
        "Racionalismo", "Empirismo", "Criticismo", "Escepticismo",
        'C', "kant"
        ));

    enqueue(Pregunta(
        "6) De las siguientes obras de Emanuel Kant, seleccione \n aquella que define su epistemología:",
        "Critica de la razón práctica", "Critica de la razón pura", "Critica del juicio", "Critica fenomenológica",
        'B', "kant"
        ));
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
        "Una de las siguientes es considerada \n de las cuatro reglas del método en Descartes:",
        "Hipótesis", "Deducción", "Evidencia", "Inducción",
        'C', "descartes"
        ));

    enqueue(Pregunta(
        "En relación con los datos experimentales que otorgan \n los sentidos en el conocimiento, Descartes define como:",
        "Los sentidos son una fuente confiable para conocer", "Desconfianza de lo que los sentidos nos proporcionan",
        "Los sentidos son complementarios", "Los sentidos son determinantes",
        'B', "descartes"
        ));

    enqueue(Pregunta(
        "Solo debemos aceptar como verdadero aquel conocimiento \n que sea EVIDENTE, CLARO Y DISTINTO:",
        "René Descartes", "David Hume", "George Berkeley", "Aristóteles",
        'A', "descartes"
        ));

    enqueue(Pregunta(
        "En cuanto a la certeza del conocimiento, \n René Descartes afirma lo siguiente:",
        "Es preciso dudar", "Debemos confiar ciegamente", "Nada es importante", "Todo es posible",
        'A', "descartes"
        ));
}
