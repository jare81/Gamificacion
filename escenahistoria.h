#ifndef ESCENAHISTORIA_H
#define ESCENAHISTORIA_H

#include <QDialog>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QKeyEvent>
#include "sprite.h"
#include "mapa.h"
#include "escenainterior.h"
#include "colapreguntas.h"
#include "spritevillanahistoria.h"


#include "gamestate.h"
#include <QTimer>

namespace Ui {
class EscenaHistoria;
}

class EscenaHistoria : public QDialog
{
    Q_OBJECT


public:
    explicit EscenaHistoria(QWidget *parent = nullptr);
    ~EscenaHistoria();

    void setSprite(isSprite *chica);
    void regresarEscenaInterior();
    void setEscenaAnterior(QWidget *escena);




protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void animarMovimiento();
    //void abrirEscenaHistoria();

    //PREGUNTAS
    void actualizarTiempo();
    void siguientePregunta();
    void verificarRespuesta();

    void onVillanaColisionaChica();

private:
    Ui::EscenaHistoria *ui;

    QGraphicsScene *sceneHistoria;
    isSprite *Chica;
    spriteVillanaHistoria *villana;
    QTimer *animTimer;
    int dx, dy;


    QGraphicsPixmapItem *fondoItemDentro;
    QWidget *escenaAnterior = nullptr;

    //PREGUNTAS
    void mostrarPregunta(const Pregunta& pregunta);
    void reiniciarTimer();
    ColaPreguntas preguntas;
    Pregunta preguntaActual;
    QTimer *timer;
    int tiempoRestante;

    int numeroPreguntaActual;
    int totalPreguntas;
    int respuestasCorrectas = 0;

    void mostrarDerrota();


};

#endif // ESCENAHISTORIA_H
