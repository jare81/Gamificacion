#ifndef SPRITER_H
#define SPRITER_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QTimer>
#include <QPixmap>

#include "isSprite.h"

class spriteR : public QObject,  public isSprite
{
    Q_OBJECT
public:
     explicit spriteR(QObject *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;


    // ISprite (override)
    void mover(int dx, int dy) override;
    void atacar() override;
    void dolor() override;         // <- añade esta firma
    void quieto() override;
    void morir() override;
    bool estaEnZona(const QString &nombreZona) const override;

public slots:
    void actualizacion();

private:
    enum class Estado { Idle, WalkLeft, WalkRight, Attack, Hurt, Dead };
    void setEstado(Estado e);
    void seleccionarPixmapPorEstado();
    int frameWidthActual() const;
    int frameHeightActual() const;
    int framesTotalesActual() const;

    QTimer *timer = nullptr;

    // Estado/frames
    Estado estado = Estado::Idle;
    int columnas = 0;

    // Sprites
    QPixmap *pixmapIdle   = nullptr;  // “normal”
    QPixmap *pixmapLeft   = nullptr;  // caminar izquierda (frente en tu nota)
    QPixmap *pixmapRight  = nullptr;  // caminar derecha (atrás en tu nota)
    QPixmap *pixmapAtk    = nullptr;
    QPixmap *pixmapHurt   = nullptr;

    QPixmap *pixmapActual = nullptr;

    // Dimensiones por hoja (ajústalas a tus sprites reales)
    int wIdle=100,  hIdle=160,  framesIdle=0;
    int wWalk=100,  hWalk=160,  framesLeft=0, framesRight=0;
    int wAtk =132,  hAtk =160,  framesAtk=0;
    int wHurt=135,  hHurt=160,  framesHurt=0;


    QSizeF sizeFor(Estado e) const;
    void ajustarTickPorEstado();

    void quietoForce();

};

#endif // SPRITER_H
