#ifndef ESCENACIENCIA_H
#define ESCENACIENCIA_H

#include <QDialog>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QKeyEvent>

#include "sprite.h"
#include "colapreguntas.h"
#include "escenainterior.h"

#include "gamestate.h"



namespace Ui {
class EscenaCiencia;
}

class EscenaCiencia : public QDialog
{
    Q_OBJECT

public:
    explicit EscenaCiencia(QWidget *parent = nullptr);
    ~EscenaCiencia();

    void setSprite(isSprite *chica);
    void regresarEscenaInterior();

     void setEscenaAnterior(QWidget *escena);



 protected:
     void keyPressEvent(QKeyEvent *event) override;
     void keyReleaseEvent(QKeyEvent *event) override;


 private slots:
     void animarMovimiento();
     void colisionPared(QGraphicsItem* pared);

     //void abrirEscenaHistoria();

     //PREGUNTAS
     void siguientePregunta();
     void verificarRespuesta();


private:
    Ui::EscenaCiencia *ui;


    QGraphicsScene *sceneCiencia;
    isSprite *Chica;
    QTimer *animTimer;
    int dx, dy;

    QGraphicsPixmapItem *fondoItemDentro;
    QWidget *escenaAnterior = nullptr;


    //PREGUNTAS
    void mostrarPregunta(const Pregunta& pregunta);
    ColaPreguntas preguntas;
    Pregunta preguntaActual;
    int totalPreguntas;
    int numeroPreguntaActual;
    int respuestasCorrectas;


    QList<QGraphicsProxyWidget*> tumbas;



};

#endif // ESCENACIENCIA_H
