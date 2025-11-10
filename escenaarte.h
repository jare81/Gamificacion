#ifndef ESCENAARTE_H
#define ESCENAARTE_H

#include <QDialog>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QVariantAnimation>
#include <QFrame>
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include<QPropertyAnimation>


#include "sprite.h"
#include "colapreguntas.h"
#include "escenainterior.h"

#include "gamestate.h"


namespace Ui {
class EscenaArte;
}

class EscenaArte : public QDialog
{
    Q_OBJECT

public:
    explicit EscenaArte(QWidget *parent = nullptr);
    ~EscenaArte();


    void setSprite(isSprite *chica);
    void regresarEscenaInterior();

    void setEscenaAnterior(QWidget *escena);


    //niebla
    void ajustarATamanoDeView();
    void initNiebla();
    void setFogProgress(double p);
    void animateFogTo(double target, int ms = 350);
    void layoutFog();  // Reposiciona/escala niebla según fogProgress y tamaño del view


    // --- Utilidades ---
    void initOverlayArte();
    void showOverlayArte(const QString& txt, bool estiloDerrota);
    void checkFogCollision();          // << NUEVA
    void victoria();                   // << NUEVA
    void derrota();                    // << NUEVA
    bool isCollidingWithFog() const;   // << NUEVA



protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *e) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void animarMovimiento();

    //void abrirEscenaHistoria();

    //PREGUNTAS
    void siguientePregunta();
    void verificarRespuesta();

private:
    Ui::EscenaArte *ui;

    QGraphicsScene *sceneArte;
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

    // ---- Niebla ----
    QGraphicsPixmapItem *fogLeft = nullptr;
    QGraphicsPixmapItem *fogRight = nullptr;
    QVariantAnimation *fogAnim = nullptr;
    QPixmap fogPixmap;
    double fogProgress = 0.0; // 0.0 = abierta, 1.0 = cerrado total

    // Opcional: área jugable que cubre la niebla (puedes recalcularla si cambia el size del view)
    QRectF playArea;


    // --- Overlay (arte) ---
    QFrame* overlay = nullptr;
    QLabel* overlayLabel = nullptr;
    QGraphicsOpacityEffect* overlayFx = nullptr;
    QPropertyAnimation *fadeIn = nullptr, *fadeOut = nullptr;
    QSequentialAnimationGroup* shakeGroup = nullptr;
    QTimer* autoHide = nullptr;


    // --- Estados / sonidos ---
    bool gameOver = false;     // evita doble derrota/victoria



};

#endif // ESCENAARTE_H
