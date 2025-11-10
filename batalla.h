#ifndef BATALLA_H
#define BATALLA_H

#include <QDialog>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QKeyEvent>
#include "sprite.h"
#include "spritev.h"
#include "spriter.h"
#include "colapreguntas.h"

#include <QTimer>

#include <QLabel>
#include <QRandomGenerator>
#include <QSoundEffect>

#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>

#include "chooseapectdialog.h"



class isSprite;      // fwd
class spriteV;       // fwd
class spriteR;       // fwd
class ChooseApectDialog; // fwd

namespace Ui {
class Batalla;
}

class Batalla : public QDialog
{
    Q_OBJECT

public:
    explicit Batalla(QWidget *parent = nullptr);
    ~Batalla();

    //SPRITE CHICA
    void setSprite(isSprite *chica);

    void derrota();
    void victoria();


protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;


private slots:
    void animarMovimiento();

    //PREGUNTAS
    void siguientePregunta();
    void verificarRespuesta();

    //void colisionSpriteV();

     void onAIRespondePrimero();

private:
    Ui::Batalla *ui;

    QGraphicsScene *sceneBatalla;
    isSprite *Chica;
    spriteV *villana;
    QTimer *animTimer;
    int dx, dy;

    QGraphicsPixmapItem *fondoItemDentro;
    QString fondoActual;


    //PREGUNTAS
    void mostrarPregunta(const Pregunta& pregunta);

    ColaPreguntas preguntas;
    Pregunta preguntaActual;



    int numeroPreguntaActual;
    int totalPreguntas;
    int respuestasCorrectas = 0;

    void mostrarDerrota();

    QFrame *overlay = nullptr;
    QLabel *overlayLabel = nullptr;
    QGraphicsOpacityEffect *overlayFx = nullptr;
    QPropertyAnimation *fadeIn = nullptr;
    QPropertyAnimation *fadeOut = nullptr;
    QSequentialAnimationGroup *shakeGroup = nullptr;
    QTimer *autoHide = nullptr;

    void initOverlay();                           // crea el overlay
    void showTetricoOverlay(const QString& txt,   // muestra mensaje
                            bool gano);

    QSoundEffect *sonidoDerrota = nullptr;
    QSoundEffect *sonidoVictoria = nullptr;


    // IA
     QTimer* aiTimer = nullptr;
    bool    preguntaViva = false;


    // === NUEVO: referencias claras a jugador/enemigo ===
    isSprite* jugador = nullptr;
    isSprite* enemigo = nullptr;

    // Helpers
    void pedirAspectoJugador();
    isSprite* crearSpritePorEleccion(ChooseApectDialog::Eleccion e);
    isSprite* crearEnemigoOpuesto(ChooseApectDialog::Eleccion e);
    void colocarFrenteAFrente();
    void lookAtEachOther();

    void iniciarCombate();
    void preRoundCountdown(int desde = 3);


    // --- Coreografía de golpe ---
    QTimer* strikeTimer = nullptr;
    bool strikeInProgress = false;

    enum class StrikePhase { Advance, AttackPause, Retreat };
    StrikePhase strikePhase;

    isSprite* atkPtr = nullptr;
    isSprite* defPtr = nullptr;
    QPointF   atkHome;

    // Parámetros de la coreografía
    int advancePixels = 140;   // cuánto se acerca a golpear
    int stepPixels    = 10;    // tamaño de paso por tick
    int tickMs        = 30;    // cada cuánto se mueve
    int pauseAttackMs = 420;   // pausa para que se vea el golpe

    int stepsRemaining = 0;
    int stepSign = +1;         // +1 hacia la derecha, -1 hacia la izquierda

    void strike(isSprite* attacker, isSprite* defender);
    void tickStrike();

    void resetRoundState();



};

#endif // BATALLA_H
