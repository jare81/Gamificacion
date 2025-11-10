#ifndef ISSPRITE_H
#define ISSPRITE_H

#pragma once
#include <QGraphicsItem>
#include <QString>

// Interfaz POLIMÓRFICA
class isSprite : public QGraphicsItem {
public:
    virtual ~isSprite() = default;

    // Acciones comunes
    virtual void mover(int dx, int dy) = 0;
    virtual void atacar() = 0;
    virtual void dolor() = 0;    // en el normal puede ser “no-op”
    virtual void quieto() = 0;
    virtual void morir() = 0;

    virtual bool estaEnZona(const QString& nombreZona) const = 0;
};
#endif // ISSPRITE_H
