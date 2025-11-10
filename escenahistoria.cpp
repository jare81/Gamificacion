#include "escenahistoria.h"
#include "ui_escenahistoria.h"
#include <QMessageBox>



EscenaHistoria::EscenaHistoria(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EscenaHistoria)
    , sceneHistoria(nullptr)
    , Chica(nullptr)
    , dx(0)
    , dy(0)
    , tiempoRestante(15)
{
    ui->setupUi(this);


    // Crear escena
    sceneHistoria = new QGraphicsScene(this);
    ui->graphicsViewHistoria->setScene(sceneHistoria);
    ui->graphicsViewHistoria->setRenderHint(QPainter::Antialiasing);
    ui->graphicsViewHistoria->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsViewHistoria->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Fondo nuevo
    QPixmap fondoDentro("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/backHistoria.jpg");
    fondoItemDentro = sceneHistoria->addPixmap(
        fondoDentro.scaled(ui->graphicsViewHistoria->size(),
                           Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation));
    fondoItemDentro->setZValue(0);

    // --- Temporizador para animación ---
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &EscenaHistoria::animarMovimiento);

    QGraphicsRectItem *regresar = sceneHistoria->addRect(70, 250, 60, 200, QPen(Qt::NoPen), QBrush(Qt::NoBrush));
    regresar->setData(0, "atras");



    // --- Asegurar que reciba eventos de teclado ---
    setFocusPolicy(Qt::StrongFocus);
    ui->graphicsViewHistoria->setFocusPolicy(Qt::NoFocus);
    this->setFocus();


    //PREGUNTAS
    preguntas.cargarPreguntasHistoria();
    totalPreguntas = 5;
    numeroPreguntaActual = 0;
    respuestasCorrectas = 0;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &EscenaHistoria::actualizarTiempo);

    // Conectar botones a la función de verificación
    connect(ui->bnOpcionA, &QPushButton::clicked, this, &EscenaHistoria::verificarRespuesta);
    connect(ui->bnOpcionB, &QPushButton::clicked, this, &EscenaHistoria::verificarRespuesta);
    connect(ui->bnOpcionC, &QPushButton::clicked, this, &EscenaHistoria::verificarRespuesta);
    connect(ui->bnOpcionD, &QPushButton::clicked, this, &EscenaHistoria::verificarRespuesta);

    siguientePregunta(); // Mostrar la primera

    villana = new spriteVillanaHistoria();
    sceneHistoria->addItem(villana);
    villana->setPos(70, 430); // posición inicial


}


void EscenaHistoria::setSprite(isSprite *chica)
{
    Chica = chica;
    if (Chica) {
        sceneHistoria->addItem(Chica);
        Chica->setZValue(2);
        Chica->setPos(700, 460);
    }

    if (villana) {
        villana->setChica(Chica);
        connect(villana, &spriteVillanaHistoria::colisionConChica,
                this, &EscenaHistoria::onVillanaColisionaChica);

    }

}

void EscenaHistoria::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    int paso = 20;

    switch (event->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
        dy = -paso; dx = 0;
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        dy = paso; dx = 0;
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
        dx = -paso; dy = 0;
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        dx = paso; dy = 0;
        break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }

    if (!animTimer->isActive())
        animTimer->start(90);
}

void EscenaHistoria::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;
    dx = dy = 0;
    animTimer->stop();
    if (Chica) Chica->quieto();
}

void EscenaHistoria::animarMovimiento()
{
    if (!Chica) return;

    // Mueve el sprite
    Chica->mover(dx, dy);

    //pared para regresar
    QList<QGraphicsItem*> colisiones = Chica->collidingItems();
    for (QGraphicsItem *item : colisiones) {
        QString tipo = item->data(0).toString();

        if (tipo == "atras") {
            QCoreApplication::processEvents();
            regresarEscenaInterior();
            return; // Detenemos animación
        }
    }

}
void EscenaHistoria::setEscenaAnterior(QWidget *escena)
{
    escenaAnterior = escena;
}


void EscenaHistoria::regresarEscenaInterior()
{
    // Detener animación
    animTimer->stop();
    dx = dy = 0;
    if (Chica) Chica->quieto();

    //  la escena anterior
    if (escenaAnterior) {
        EscenaInterior *interior = qobject_cast<EscenaInterior*>(escenaAnterior);
        if (interior && Chica) {
            sceneHistoria->removeItem(Chica);       // quitarlo de esta escena
            interior->agregarSpriteDesdeHistoria(Chica);  // pasar el sprite a la anterior
        }

        escenaAnterior->show();
        this->close();

        // 💡 Reasignar el foco después de que la ventana se haya cerrado
        QTimer::singleShot(50, [interior]() {
            if (interior) {
                interior->activateWindow();
                interior->raise();
                interior->setFocus();
                if (interior->getGraphicsView())
                    interior->getGraphicsView()->clearFocus(); // aseguramos que no robe foco
            }
        });
    } else {
        this->close();
    }

}

// Mostrar una pregunta en el UI
// =============================
void EscenaHistoria::mostrarPregunta(const Pregunta &pregunta) {
    ui->lbPregunta->setText(pregunta.texto);
    ui->bnOpcionA->setText(pregunta.respuestaA);
    ui->bnOpcionB->setText(pregunta.respuestaB);
    ui->bnOpcionC->setText(pregunta.respuestaC);
    ui->bnOpcionD->setText(pregunta.respuestaD);
}

// Cargar la siguiente pregunta
// =============================
void EscenaHistoria::siguientePregunta() {
    numeroPreguntaActual++;

    if (numeroPreguntaActual > totalPreguntas) {
        // Detener timers/animaciones antes de evaluar
        if (timer->isActive()) timer->stop();
        if (animTimer->isActive()) animTimer->stop();

        // Detener movimiento de villana y poner todo quieto
        if (villana) villana->detenerMovimiento();
        if (Chica) Chica->quieto();

        const bool gano = (respuestasCorrectas == totalPreguntas);
        // **Registrar resultado en el estado global**
        Gamestate::instance().registrarResultado(Gamestate::Historia, gano);


        //////////////////////////////////////aquiii
        // Evaluar si ganó o perdió en base a las respuestas correctas
        if (gano) {
            // ganó — no hubo colisión (si hubo colisión ya se hubiese manejado por la señal)
            QMessageBox::information(this, "¡Ganaste!", "Derrotaste al enemigo ");
            QCoreApplication::processEvents();
            regresarEscenaInterior();
            //return; // Detenemos animación
        } else {
            // perdió por no contestar suficientes preguntas correctamente
            mostrarDerrota();
        }
        return;
    }


    // Mostrar el progreso: "Pregunta 1 de 5"
    ui->lbCantidad->setText(
        QString("%1 / %2").arg(numeroPreguntaActual).arg(totalPreguntas)
        );

    // Mostrar siguiente pregunta
    if (!preguntas.isEmpty()) {
        preguntaActual = preguntas.dequeue();
        mostrarPregunta(preguntaActual);
        reiniciarTimer();
    }

}

// Reiniciar el timer
// =============================
void EscenaHistoria::reiniciarTimer() {
    tiempoRestante = 15; // segundos
    ui->lbTiempo->setText(QString::number(tiempoRestante));
    timer->start(1000); // cada segundo
}

void EscenaHistoria::actualizarTiempo() {
    tiempoRestante--;
    ui->lbTiempo->setText(QString::number(tiempoRestante));

    if (tiempoRestante <= 0) {
        timer->stop();
        QMessageBox::warning(this, "Tiempo agotado", "¡El enemigo se acerca!");

        // Penalizar como incorrecta
        if (villana) villana->seguirAvanzando();

        siguientePregunta();
    }
}

// Verificar la respuesta elegida
// =============================
void EscenaHistoria::verificarRespuesta() {
    timer->stop();

    QPushButton *boton = qobject_cast<QPushButton*>(sender());
    if (!boton) return;

    char respuesta = ' ';
    if (boton == ui->bnOpcionA) respuesta = 'A';
    else if (boton == ui->bnOpcionB) respuesta = 'B';
    else if (boton == ui->bnOpcionC) respuesta = 'C';
    else if (boton == ui->bnOpcionD) respuesta = 'D';

    if (preguntaActual.respuestaCorrecta == respuesta) {
        respuestasCorrectas++;
        QMessageBox::information(this, "Correcto", "El enemigo retrocede");
         villana->retroceder();
    } else {
        QMessageBox::warning(this, "Incorrecto", "El enemigo se acerca");
          villana->seguirAvanzando();
    }

    siguientePregunta();
}

void EscenaHistoria::onVillanaColisionaChica()
{
    // Detener todos los timers/animaciones
    if (animTimer->isActive()) animTimer->stop();
    if (timer->isActive()) timer->stop();

    dx = dy = 0;
    if (Chica) {
        Chica->morir(); // llama a la animación de muerte de la chica
    }
    if (villana) {
        villana->detenerMovimiento();
    }

    // Mostrar derrota y regresar a escena anterior
    mostrarDerrota(); // este método ya muestra el QMessageBox y regresa
}

void EscenaHistoria::mostrarDerrota() {
    // ✅ Mostrar imagen de derrota

    QMessageBox::information(this, "Has perdido!",  "El enemigo te atrapó...");

    regresarEscenaInterior();

}








EscenaHistoria::~EscenaHistoria()
{
    delete ui;
}
