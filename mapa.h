#ifndef MAPA_H
#define MAPA_H

#include <QDialog>
#include <QGraphicsPixmapItem>
#include "sprite.h"

#include <QWidget>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QKeyEvent>

namespace Ui {
class Mapa;
}

class Mapa : public QDialog
{
    Q_OBJECT

public:
    explicit Mapa(QWidget *parent = nullptr);
    ~Mapa();

    void setSprite(isSprite *chica);


protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void animarMovimiento();

private:
    Ui::Mapa *ui;
    QGraphicsScene *sceneMapa;

    isSprite *Chica;
    QTimer *animTimer;
    int dx, dy;

};

#endif // MAPA_H
