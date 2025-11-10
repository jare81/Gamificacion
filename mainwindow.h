#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QGraphicsItem>
#include <QRectF>
#include <QDesktopServices>
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTimer>
#include "sprite.h"
#include "ruleta.h"



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void on_pushButton_clicked();

    void on_bnNuevaPartida_clicked();

    void animarMovimiento();

    void on_giro_2_clicked();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event);

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;
    QGraphicsScene *sceneDentro;
    QGraphicsScene *sceneMapa;

    Ruleta *ruleta;


    isSprite *Chica;
     QTimer *animTimer;
      int dx, dy;

     bool enZonaSalida;

    float x,y, ancho, alto;



};
#endif // MAINWINDOW_H
