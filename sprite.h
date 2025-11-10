#ifndef SPRITE_H
#define SPRITE_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QTimer>
#include <QPixmap>

#include "isSprite.h"

class sprite : public QObject,  public isSprite
{
    Q_OBJECT
public:
    explicit sprite(QObject *parent = nullptr);
    QPixmap *pixmapFrente;
    QPixmap *pixmapAtras;
    QPixmap *pixmapActual;

    QPixmap *pixmapMuerte;  // sprite sheet de la muerte
    bool estaMuerto;        // estado de muerte
    int framesMuerte;
    int anchoMuerte;
    int altoMuerte;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void mover(int dx, int dy) override;
    void atacar() override;
    void dolor() override;         // <- añade esta firma
    void quieto() override;
    void morir() override;
    void revivir();
    bool estaEnZona(const QString& nombreZona) const override;



public slots:
    void actualizacion();


private:
    QTimer *timer;

    float filas;
    float columnas;
    float ancho;
    float alto;

    // atacar
    QPixmap *pixmapAtaque = nullptr;
    int anchoAtaque = 150;
    int altoAtaque  = 160;
    int framesAtaque = 0;
    bool estaAtacando = false;

    // defenderse
    QPixmap *pixmapDefensa = nullptr;
    int anchoDefensa = 150;
    int altoDefensa  = 160;
    int framesDefensa = 0;
    bool estaDefendiendo = false;



};

#endif // SPRITE_H
