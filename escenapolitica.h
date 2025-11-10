#ifndef ESCENAPOLITICA_H
#define ESCENAPOLITICA_H

#include <QDialog>
#include <qgraphicsscene.h>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QKeyEvent>
#include "sprite.h"
#include "colapreguntas.h"
#include "escenainterior.h"

#include <QLabel>
#include <QRandomGenerator>
#include <QSoundEffect>

#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>



namespace Ui {
class EscenaPolitica;
}

class EscenaPolitica : public QDialog
{
    Q_OBJECT

public:
    explicit EscenaPolitica(QWidget *parent = nullptr);
    ~EscenaPolitica();

    void regresarEscenaInterior();
    void setEscenaAnterior(QWidget *escena);

    void setSprite(isSprite *chica);

    void setVidrioEstado(int idx, bool roto);
    void romperVidrioAleatorio();
    void repararVidrioUno();
    void derrota();
    void victoria();



 protected:
     void keyPressEvent(QKeyEvent *event) override;
     void keyReleaseEvent(QKeyEvent *event) override;
     void resizeEvent(QResizeEvent *event) override;


 private slots:
     void animarMovimiento();
     //void abrirEscenaHistoria();

     //PREGUNTAS
     void siguientePregunta();
     void verificarRespuesta();


private:
    Ui::EscenaPolitica *ui;

    QGraphicsScene *scenePolitica;
    QGraphicsPixmapItem *fondoItemDentro;
      QWidget *escenaAnterior = nullptr;

    isSprite *Chica;
    QTimer *animTimer;
    int dx, dy;



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

      //VIDRIOS
      QList<QLabel*> vidrios;      // lbVidrio1..lbVidrio5
      QPixmap iconoVidrioRoto;     // imagen del vidrio roto
      QVector<bool> estaRoto;      // estado de cada label


      QSoundEffect *sonidoVidrio;


      QFrame *overlay = nullptr;
      QLabel *overlayLabel = nullptr;
      QGraphicsOpacityEffect *overlayFx = nullptr;
      QPropertyAnimation *fadeIn = nullptr;
      QPropertyAnimation *fadeOut = nullptr;
      QSequentialAnimationGroup *shakeGroup = nullptr;
      QTimer *autoHide = nullptr;

      void initOverlay();                           // crea el overlay
      void showTetricoOverlay(const QString& txt,   // muestra mensaje
                              bool esRoto);         // true: “vidrio roto”, false: “reparado”


      QSoundEffect *sonidoDerrota = nullptr;
      QSoundEffect *sonidoVictoria = nullptr;
      QPixmap fondoDerrota;
      QPixmap iconoMonstruo;


};

#endif // ESCENAPOLITICA_H
