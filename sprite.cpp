#include "sprite.h"

sprite::sprite(QObject *parent)
    : QObject{parent}
{
    timer = new QTimer(this);

    filas =0;
    columnas=0;

    pixmapFrente = new QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/per2.png");
    pixmapAtras  = new QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/per3.png");

    // mirando hacia adelante
    pixmapActual = pixmapFrente;

    //dimensiones de imagenes
    ancho =90;
    alto = 160;

    //timer->start(350);

    connect(timer, &QTimer::timeout, this, &sprite::actualizacion);


    //muerte
    pixmapMuerte = new QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/spriteDead1.png");
    framesMuerte = pixmapMuerte->width() / ancho;
    estaMuerto = false;
    anchoMuerte = 155;
    altoMuerte  = 160;
    framesMuerte = pixmapMuerte->width() / anchoMuerte;

    //atacar
    pixmapAtaque = new QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/ataque.png");
    if (!pixmapAtaque->isNull()) {
        framesAtaque = pixmapAtaque->width() / anchoAtaque;
    } else {
        framesAtaque = 0; // por si falla la carga
    }




}

void sprite::actualizacion()
{
    if (estaMuerto) {
        columnas += anchoMuerte;
        if (columnas >= framesMuerte * anchoMuerte) {
            columnas = (framesMuerte-1) * anchoMuerte;
            timer->stop();
        }
        update();
        return;
    }

    if (estaAtacando) {
        columnas += anchoAtaque;
        if (columnas >= framesAtaque * anchoAtaque) {
            // Termina ataque
            estaAtacando = false;
            columnas = 0;
            timer->stop();     // ← clave para que no se dispare “caminar” solo
        }
        update();
        return;
    }

}



QRectF sprite::boundingRect() const
{
    if (estaMuerto)
        return QRectF(-anchoMuerte/2, -altoMuerte/2, anchoMuerte, altoMuerte);

    if (estaAtacando)
        return QRectF(-anchoAtaque/2, -altoAtaque/2, anchoAtaque, altoAtaque);

    return QRectF(-ancho/2, -alto/2, ancho, alto);
}


void sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (estaMuerto) {
        painter->drawPixmap(-anchoMuerte/2, -altoMuerte/2,
                            *pixmapMuerte, columnas, 0,
                            anchoMuerte, altoMuerte);
    } else if (estaAtacando) {
        painter->drawPixmap(-anchoAtaque/2, -altoAtaque/2,
                            *pixmapAtaque, columnas, 0,
                            anchoAtaque, altoAtaque);
    } else {
        painter->drawPixmap(-ancho/2, -alto/2,
                            *pixmapActual, columnas, 0,
                            ancho, alto);
    }
}



void sprite::mover(int dx, int dy) {
   if (estaMuerto || estaAtacando) return;

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

void sprite::morir() {
    /*//if (estaMuerto) return; // evitar reinicio
    //estaMuerto = true;
    columnas = 0;  // empieza desde el primer frame
    timer->start(120); //*/ /*velocidad más rápida para la animación*/

    // if (estaMuerto) return;
     estaAtacando = false;  // ← cancelar ataque si estaba en curso
    // estaMuerto = true;
    // columnas = 0;
    // timer->start(120);
}

void sprite::atacar() {
    if (estaMuerto || estaAtacando) return;  // no encadenar ataques ni atacar muerto
    if (framesAtaque <= 0) return;           // no hay sprite de ataque cargado

    estaAtacando = true;
    columnas = 0;          // empieza desde el primer frame
    if (!timer->isActive())
        timer->start(90);  // velocidad del ataque (ajusta a gusto)
    update();
}



bool sprite::estaEnZona(const QString &nombreZona) const
{
    QList<QGraphicsItem*> colisiones = collidingItems();
    for (auto *item : colisiones) {
        if (item->data(0).toString() == nombreZona) {
            return true;
        }
    }
    return false;
}


void sprite::dolor() {
    // No-op
}


void sprite::quieto() {
    if (estaMuerto || estaAtacando) return;
    columnas = 0;  // primer frame
    update();
}




