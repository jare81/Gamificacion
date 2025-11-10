#include "spritevillanahistoria.h"

spriteVillanaHistoria::spriteVillanaHistoria(QObject *parent)
    : QObject{parent}, direccion(1), velocidad(2) // velozidad
{

    timer = new QTimer(this);

    filas =0;
    columnas=0;

    pixmapFrente = new QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/spriteVH.png");
    pixmapAtras  = new QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/spriteVHatras.png");

    // mirando hacia adelante
    pixmapActual = pixmapFrente;

    //dimensiones de imagenes
    ancho =185;
    alto = 240;

    //timer->start(350);

    connect(timer, &QTimer::timeout, this, &spriteVillanaHistoria::actualizacion);

      timer->start(150);


}


void spriteVillanaHistoria::actualizacion()
{
    columnas += ancho;
    if (columnas >= pixmapActual->width())
        columnas = 0;

    // Movimiento automático
    qreal oldX = x();
    qreal oldY = y();

    setPos(x() + velocidad * direccion, y());

    // Evitar pasar paredes
    QList<QGraphicsItem*> colisiones = collidingItems();
    for (auto* item : colisiones) {
        if (item->data(0) == "pared") {
            setPos(oldX, oldY); // volver atrás si toca pared
            direccion *= -1;    // cambiar dirección
            break;
        }
    }

    // --- Detección de colisión con la chica ---
    if (chicaObj && collidesWithItem(chicaObj)) {
        // ya no llamamos directamente a chicaObj->morir() desde aquí
        chicaObj->morir();
        emit colisionConChica();
        timer->stop();       // opcional: detener movimiento de villana
        return;
    }

    update();
}

QRectF spriteVillanaHistoria::boundingRect() const
{
    return QRectF(-ancho/2, -alto/2, ancho, alto);
}

void spriteVillanaHistoria::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(-ancho/2, -alto/2, *pixmapActual, columnas, 0, ancho, alto);

}


void spriteVillanaHistoria::mover(int dx, int dy) {
    if (dx == 0 && dy == 0)
        return;

    qreal oldX = x();
    qreal oldY = y();

    setPos(x() + dx, y() + dy);

    QList<QGraphicsItem*> colisiones = collidingItems();
    for (auto* item : colisiones) {
        if (item->data(0) == "pared") {
            setPos(oldX, oldY);
            return;
        }
    }

    // Cambiar sprite según dirección
    if (dy < 0)
        pixmapActual = pixmapActual;
    else if (dy > 0)
        pixmapActual = pixmapActual;
    else if (dx < 0)
        pixmapActual = pixmapAtras;
    else if (dx > 0)
        pixmapActual = pixmapFrente;


    // Reproducir solo 7 frames (0–6)
    columnas += ancho;
    if (columnas >= ancho * 7)
        columnas = 0;

    update();
}

void spriteVillanaHistoria::retroceder()
{
    timer->stop();
    pixmapActual = pixmapAtras; // ⬅️ Mostrar sprite hacia atrás
    update();

    qreal desplazamiento = 70; // puedes ajustar la distancia del retroceso
    setPos(x() - desplazamiento, y());

    // Restaurar dirección normal más tarde
    QTimer::singleShot(400, [this]() {
        pixmapActual = pixmapFrente; // ⬅️ Volver sprite hacia adelante
        update();
    });

    timer->start(150);
}

void spriteVillanaHistoria::seguirAvanzando()
{
    pixmapActual = pixmapFrente; // mirar hacia adelante
    update();

    qreal avanceExtra = 70; // ⬅ Ajusta este valor para avanzar más
    setPos(x() + avanceExtra, y());
}

void spriteVillanaHistoria::detenerMovimiento()
{
    if (timer->isActive()) timer->stop();
    // dejar el frame en quieto
    columnas = 1;
    update();
}


bool spriteVillanaHistoria::estaEnZona(const QString &nombreZona) {
    QList<QGraphicsItem*> colisiones = collidingItems();
    for (auto *item : colisiones) {
        if (item->data(0).toString() == nombreZona) {
            return true;
        }
    }
    return false;
}

void spriteVillanaHistoria::setChica(isSprite *chica) {
    chicaObj = chica;
}







void spriteVillanaHistoria::quieto() {
    columnas = 1;  // primer frame
    update();
}
