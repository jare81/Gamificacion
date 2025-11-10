#ifndef PREGUNTA_H
#define PREGUNTA_H

#include <QString>

struct  Pregunta
{
public:
    QString texto;
    QString respuestaA;
    QString respuestaB;
    QString respuestaC;
    QString respuestaD;
    char respuestaCorrecta; // 'A', 'B', 'C' o 'D'
    QString tipo; // Ruleta: "arte", "politica", "ciencia", "historia", "epistemologia",  Batalla: "kant", "descartes"

    Pregunta() : respuestaCorrecta('A') {}

    Pregunta(QString t, QString a, QString b, QString c, QString d, char correcta, QString tip)
        : texto(t), respuestaA(a), respuestaB(b), respuestaC(c), respuestaD(d),
        respuestaCorrecta(correcta), tipo(tip) {}

};

#endif // PREGUNTA_H
