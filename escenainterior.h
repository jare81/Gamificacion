#ifndef ESCENAINTERIOR_H
#define ESCENAINTERIOR_H

#include <QDialog>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QKeyEvent>
#include "sprite.h"
#include "mapa.h"

#include <QLabel>
#include "gamestate.h"
#include <QPointer>
#include "ruleta.h"




namespace Ui {
class EscenaInterior;
}

class EscenaInterior : public QDialog
{
    Q_OBJECT

public:
    explicit EscenaInterior(QWidget *parent = nullptr);
    ~EscenaInterior();

    //SPRITE CHICA
    void setSprite(isSprite *chica);
    void cambiarFondo(int seccion);
    void actualizarFondo(const QString &ruta);
    void animarMovimiento();

    //SCENAS DE MINIJUEGOS
    void abrirEscenaHistoria();
    void abrirEscenaCiencia();
    void abrirEscenaPolitica();
    void abrirEscenaArte();
    void abrirEscenaBatalla();

    void agregarSpriteDesdeHistoria(isSprite *chica);
    QGraphicsView* getGraphicsView();

    bool eventFilter(QObject* obj, QEvent* ev) override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;


private slots:





    void on_pushButton_clicked();

    void on_giro_clicked();

private:
    Ui::EscenaInterior *ui;
    QGraphicsScene *sceneDentro;
    isSprite *Chica;
    QTimer *animTimer;
    int dx, dy;

    QGraphicsPixmapItem *fondoItemDentro;
    QString fondoActual;


    QPointer<Ruleta> ruleta;                 // <-- miembro
    Gamestate& GS = Gamestate::instance();   // <-- singleton

    // Portal/pared final:
    QGraphicsRectItem* paredBatalla = nullptr;

    QLabel* heartsHUD = nullptr;
    void crearHUDCorazones();
    void actualizarHUDCorazones(int vidas);



    void actualizarParedBatalla();

    // Utilidad para saber qué puerta es
    static Gamestate::Puerta clavePuerta(const QString& clave) {
        if (clave == "historia") return Gamestate::Historia;
        if (clave == "ciencia") return Gamestate::Ciencia;
        if (clave == "politica")  return Gamestate::Politica;
        return Gamestate::Arte;
    }


     bool mostrandoAvisoPuerta = false;
     bool abriendoBatalla = false;

       // cambia al widget final


};

#endif // ESCENAINTERIOR_H
