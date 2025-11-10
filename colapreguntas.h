#ifndef COLAPREGUNTAS_H
#define COLAPREGUNTAS_H

#include <queue>
#include <QString>
#include "pregunta.h"


class ColaPreguntas
{
private:
    std::queue<Pregunta> cola;


public:
    ColaPreguntas();

    //Funciones
    void enqueue(const Pregunta& pregunta);
    Pregunta dequeue();
    bool isEmpty() const;
    int size() const;

    //Cargar
    void cargarPreguntasArte();
    void cargarPreguntasPolitica();
    void cargarPreguntasCiencia();
    void cargarPreguntasHistoria();

    void cargarPreguntasCombate();

    void cargarPreguntasEpistemologia();
    void cargarPreguntasKant();
    void cargarPreguntasDescartes();

    //Circular
    Pregunta dequeueCircular();

    //Cargar Por tipo
    void cargarPreguntasPorTipo(const QString& tipo);

    Pregunta front() const;
    void clear();
};

#endif // COLAPREGUNTAS_H
