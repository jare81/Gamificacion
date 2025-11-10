#include "colapreguntas.h"

ColaPreguntas::ColaPreguntas(){}

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


void ColaPreguntas::cargarPreguntasCiencia() {
    clear();

    enqueue(Pregunta(
        "Entre los siguientes renacentistas seleccione uno de los precursores filósofo-científico del heliocentrismo:",
        "Tomas Moro", "Galileo", "Platón", "Arquimedes",
        'B', "ciencia"
        ));

    enqueue(Pregunta(
        "El método científico se introduce por el interés de tres filósofos. Uno de los mencionados no es precursor del método científico:",
        "Francis Bacon", "Galileo Galilei", "Nicolas Maquiavelo", "René Descartes",
        'C', "ciencia"
        ));

    enqueue(Pregunta(
        "Es uno de los precursores del pensamiento Moderno:",
        "Isaac Newton", "René Descartes", "Erasmo de Roterdam", "Francis Bacon",
        'B', "ciencia"
        ));

    enqueue(Pregunta(
        "De los siguientes filósofos niega el geocentrismo :",
        "Aristóteles", "Nicolás Copérnico", "Tomás de Aquino", "Isaac Newton",
        'B', "ciencia"
        ));

    enqueue(Pregunta(
        "Uno de los inventos que suscitó un conocimiento ilimitado, fue el de Gutenberg:",
        "El astrolabio", "La imprenta", "La Nao y la Carabela", "El Telescopio",
        'B', "ciencia"
        ));
}


void ColaPreguntas::cargarPreguntasArte() {
    clear();

    enqueue(Pregunta(
        "Uno de los siguientes personajes fue el encargado de pintar la capilla Sixtina:",
        "Miguel Ángel", "Donatello", "Leonardo DaVinci", "Francis Bacon",
        'A', "arte"
        ));

    enqueue(Pregunta(
        "Genio del renacimiento que esculpió el Moisés, el David y la Pietá:",
        "Miguel Ángel", "Leonardo Da Vinci", "Rafael Sanzio", "Galileo Galilei",
        'A', "arte"
        ));

    enqueue(Pregunta(
        "Durante el renacimiento el estilo artístico que impregnó el arte, la filosofía, la pintura escritura fue el:",
        "El Gótico", "El barroco", "El clasicismo", "Romanticismo",
        'B', "arte"
        ));

    enqueue(Pregunta(
        "Durante el renacimiento surge una nueva visión del hombre, que se vio reflejada en el arte, en la política "
        "y en las ciencias sociales y humanas, a lo que se denomina:",
        "Antropocentrismo", "Humanismo", "Antropológico", "Teocentrismo",
        'B', "arte"
        ));

    enqueue(Pregunta(
        "Cuatro genios del renacimiento (Leonardo, Donatello, Rafael y Michelangelo) han sido llevados a la "
        "pantalla en los comics de:",
        "Tortugas ninjas", "Caballeros Zodiaco", "Cuatro fantásticos", "Antagonistas Attack Titan",
        'A', "arte"
        ));
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
        "Toma Constantinopla", "Tratado de paz", "Toma Bastilla", "La ruta seda",
        'B', "politica"
        ));

    enqueue(Pregunta(
        "Durante el siglo XV, la sociedad se estratifica en tres estamentos definidos:",
        "Media, baja, alta", "Nobleza, clero", "Artesanos, guardianes", "Reyes, plebeyos",
        'B', "politica"
        ));

    enqueue(Pregunta(
        "Aparece el realismo político, que se basaba en un orden establecido, explicación de un sistema y recomendaciones de como gobernar:",
        "Tomás Moro", "Jean Bodín", "Nicolás Maquiavelo", "Erasmo de Rotterdam",
        'C', "politica"
        ));

    enqueue(Pregunta(
        "Terminada la edad media, en el contexto de la política resulta que:",
        "Iglesia resalta su poder", "Iglesia pierde rectoria", "La Iglesia en la política", "La política desaparece",
        'B', "politica"
        ));
}






void ColaPreguntas::cargarPreguntasCombate() {
    clear();

    enqueue(Pregunta(
        "Para algunos de los siguientes filósofos, el criterio de verdad es la evidencia sensible:",
        "Empiristas", "Criticistas", "Racionalistas", "Dogmáticos",
        'A', "combate"
        ));

    enqueue(Pregunta(
        "De las siguientes, una de ellas es la corriente filosófica que en general tiende a negar la posibilidad de la metafísica y a sostener que hay conocimiento únicamente de los fenómenos.",
        "Racionalistas", "Empiristas", "Escolásticos", "Escépticos",
        'B', "combate"
        ));

    enqueue(Pregunta(
        "Para unos de los siguientes filósofos, la experiencia como única fuente del conocimiento.",
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
        "De los siguientes filósofos selecciones el que no se considera Racionalista:",
        "David Hume", "John Locke", "Nicolas Malebranch", "Francis Bacon",
        'C', "combate"
        ));

    enqueue(Pregunta(
        "Es la doctrina que establece que todos nuestros conocimientos provienen de la razón.",
        "Empirismo", "Criticismo", "Racionalismo", "Epistemología",
        'C', "combate"
        ));

    enqueue(Pregunta(
        "Uno de los siguientes filósofos, postula las ideas innatas en el sujeto:",
        "George Berkeley", "David Hume", "Leibniz", "Hipatía",
        'C', "combate"
        ));
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

    enqueue(Pregunta(
        "Kant decía que el lema de la ilustración era 'Sapere aude', que significa:",
        "Sopesa tus acciones", "Atrévete a saber por ti mismo", "Saber a la fuerza", "Someterse al conocimiento",
        'B', "kant"
        ));

    enqueue(Pregunta(
        "Kant (igual que Copérnico cambió el centro del universo de la tierra al sol), cambia el centro del conocimiento del objeto al sujeto, a esto se le llama:",
        "Subjetivismo", "Prejuicio", "Giro copernicano", "Suerte",
        'C', "kant"
        ));

    enqueue(Pregunta(
        "La postura conciliadora de Kant respecto a los empiristas y racionalistas define que los datos experimentales son la fuente del conocimiento racional del sujeto:",
        "Racionalismo", "Empirismo", "Criticismo", "Escepticismo",
        'C', "kant"
        ));

    enqueue(Pregunta(
        "De las siguientes obras de Emanuel Kant, seleccione aquella que define su epistemología:",
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
        "Una de las siguientes es considerada de las cuatro reglas del método en Descartes:",
        "Hipótesis", "Deducción", "Evidencia", "Inducción",
        'C', "descartes"
        ));

    enqueue(Pregunta(
        "En relación con los datos experimentales que otorgan los sentidos en el conocimiento, Descartes define como:",
        "Los sentidos son una fuente confiable para conocer", "Desconfianza de lo que los sentidos nos proporcionan",
        "Los sentidos son complementarios", "Los sentidos son determinantes",
        'B', "descartes"
        ));

    enqueue(Pregunta(
        "Solo debemos aceptar como verdadero aquel conocimiento que sea EVIDENTE, CLARO Y DISTINTO:",
        "René Descartes", "David Hume", "George Berkeley", "Aristóteles",
        'A', "descartes"
        ));

    enqueue(Pregunta(
        "En cuanto a la certeza del conocimiento, René Descartes afirma lo siguiente:",
        "Es preciso dudar", "Debemos confiar ciegamente", "Nada es importante", "Todo es posible",
        'A', "descartes"
        ));
}
