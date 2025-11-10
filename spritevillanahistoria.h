#ifndef SPRITEVILLANAHISTORIA_H
#define SPRITEVILLANAHISTORIA_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QTimer>
#include <QPixmap>
#include "sprite.h"

class spriteVillanaHistoria : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit spriteVillanaHistoria(QObject *parent = nullptr);
    QPixmap *pixmapFrente;
    QPixmap *pixmapAtras;
    QPixmap *pixmapActual;

    void setChica(isSprite *chica);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void mover(int dx, int dy);
    bool estaEnZona(const QString &nombreZona);

     void detenerMovimiento();

signals:
    void colisionConChica();


public slots:
    void actualizacion();
    void quieto();
    void retroceder();
    void seguirAvanzando();




private:
    QTimer *timer;

    float filas;
    float columnas;
    float ancho;
    float alto;

    int direccion;      // 1 = avanza, -1 = retrocede
    int velocidad;      // píxeles por tick

    isSprite *chicaObj = nullptr;
};



#endif // SPRITEVILLANAHISTORIA_H
