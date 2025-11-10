#ifndef RULETA_H
#define RULETA_H

#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QString>

class Ruleta : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit Ruleta(QObject *parent = nullptr);

    void iniciarGiro();  // Inicia la animación de la ruleta
    void reset();

signals:
    void resultadoListo(const QString &seccion);   // Notifica qué sección salió
    void fondoCambiado(const QString &rutaFondo);  // Notifica para cambiar fondo
    void sinGiros();

private slots:
    void animarRuleta();   // Controla el cambio de imágenes durante el giro

private:
    void actualizarImagen();  // Cambia el pixmap actual
    void mostrarResultado();  // Muestra el resultado final

    QString rutasImagenes[4];   // Rutas de las 4 imágenes de la ruleta
    bool yaSalio[4];            // Para evitar repetir secciones
    int resultado=0;              // Índice del resultado final
    int girosRealizados =0;        // Cuántas veces ha girado
    int indiceActual =0;           // Imagen actual mostrada en la secuencia

     QTimer* timer = nullptr;             // Controla el cambio de frames
    int velocidad=0;              // Velocidad actual de cambio (ms)
    int tiempoGiro=0;             // Duración total del giro (ms)
    int tiempoTranscurrido=0;     // Tiempo transcurrido del giro
};

#endif // RULETA_H

