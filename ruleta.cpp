#include "ruleta.h"
#include <QMessageBox>
#include <QRandomGenerator>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>

Ruleta::Ruleta(QObject *parent)
    : QObject(parent), QGraphicsPixmapItem(nullptr)
{
    // --- Cargar las 4 imágenes ---
    rutasImagenes[0] = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/politicar.png";
    rutasImagenes[1] = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/arter.png";
    rutasImagenes[2] = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/cienciar.png";
    rutasImagenes[3] = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/historiar.png";

    for (int i = 0; i < 4; ++i)
        yaSalio[i] = false;

    girosRealizados = 0;
    indiceActual = 0;
    velocidad = 100; // ms por frame

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Ruleta::animarRuleta);

    // Mostrar primera imagen
    actualizarImagen();
}

void Ruleta::iniciarGiro()
{
    if (girosRealizados >= 4) {
        emit sinGiros();
        QMessageBox::information(nullptr, "Fin del juego", "Ya has girado las 4 secciones.");
        return;
    }

    int nuevoResultado;
    do {
        nuevoResultado = QRandomGenerator::global()->bounded(4);
    } while (yaSalio[nuevoResultado]);

    resultado = nuevoResultado;
    yaSalio[resultado] = true;
    girosRealizados++;

    tiempoGiro = 3000 + QRandomGenerator::global()->bounded(2000); // duración entre 3 y 5 segundos
    tiempoTranscurrido = 0;
    velocidad = 100; // iniciar rápido

    timer->start(velocidad);
}

void Ruleta::reset()
{
    // Habilita un nuevo ciclo de 4 giros
    for (int i=0;i<4;++i) yaSalio[i]=false;
    girosRealizados   = 0;
    indiceActual      = 0;
    resultado         = 0;
    velocidad         = 100;
    tiempoGiro        = 0;
    tiempoTranscurrido= 0;

    // vuelve a mostrar todas las puertas cerradas si quieres
    emit fondoCambiado("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/puertas.png");

    // vuelve visible por si se había ocultado
    this->show();
    actualizarImagen();
}


void Ruleta::animarRuleta()
{
    // Cambiar imagen en secuencia
    indiceActual = (indiceActual + 1) % 4;
    actualizarImagen();

    // Aumentar tiempo transcurrido
    tiempoTranscurrido += velocidad;

    // Simular desaceleración (cada 500 ms, hacer más lenta la animación)
    if (tiempoTranscurrido > tiempoGiro * 0.6)
        velocidad += 20;
    else if (tiempoTranscurrido > tiempoGiro * 0.8)
        velocidad += 30;

    timer->setInterval(velocidad);

    // Detener cuando se supera el tiempo total
    if (tiempoTranscurrido >= tiempoGiro) {
        timer->stop();

        // Forzar imagen final según el resultado aleatorio
        indiceActual = resultado;
        actualizarImagen();

        mostrarResultado();
    }
}

void Ruleta::actualizarImagen()
{
    QPixmap img(rutasImagenes[indiceActual]);
    if (img.isNull())
        return;

    QPixmap scaledImg = img.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // --- Si deseas mantener el estilo circular ---
    QPixmap circular(200, 200);
    circular.fill(Qt::transparent);
    QPainter painter(&circular);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addEllipse(0, 0, 200, 200);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, scaledImg);
    painter.end();

    setPixmap(circular);
}

void Ruleta::mostrarResultado()
{
    QString seccion;
    QString fondoPath;

    switch (resultado) {
    case 0:
        seccion = "POLITICA";
        fondoPath = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/politicaAbierta.png";
        break;
    case 1:
        seccion = "ARTE";
        fondoPath = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/arteAbierta.png";
        break;
    case 2:
        seccion = "CIENCIA";
        fondoPath = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/cienciaAbierta.png";
        break;
    case 3:
        seccion = "HISTORIA";
        fondoPath = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/historiaAbierta.png";
        break;
    }

    emit fondoCambiado(fondoPath);
    emit resultadoListo(seccion);

    if (girosRealizados > 4) {
        emit fondoCambiado("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/puertas.png");
        this->hide();
    }
}




