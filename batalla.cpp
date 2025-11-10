#include "batalla.h"
#include "ui_batalla.h"
#include <QMessageBox>


#include <QSequentialAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QRandomGenerator>

Batalla::Batalla(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Batalla)
    , sceneBatalla(nullptr)
    , Chica(nullptr)
    , dx(0)
    , dy(0)
{
    ui->setupUi(this);

    // Crear escena
    sceneBatalla = new QGraphicsScene(this);
    ui->graphicsViewBatalla->setScene(sceneBatalla);
    ui->graphicsViewBatalla->setRenderHint(QPainter::Antialiasing);
    ui->graphicsViewBatalla->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsViewBatalla->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Fondo nuevo
    QPixmap fondoDentro("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/combate.jpg");
    fondoItemDentro = sceneBatalla->addPixmap(
        fondoDentro.scaled(ui->graphicsViewBatalla->size(),
                           Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation));
    fondoItemDentro->setZValue(0);

    // --- Temporizador para animación ---
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &Batalla::animarMovimiento);


    // --- Asegurar que reciba eventos de teclado ---
    setFocusPolicy(Qt::StrongFocus);
    ui->graphicsViewBatalla->setFocusPolicy(Qt::NoFocus);
    this->setFocus();

    //PREGUNTAS
    preguntas.cargarPreguntasCombate();
    totalPreguntas = 5;
    numeroPreguntaActual = 0;
    respuestasCorrectas = 0;


    // Conectar botones a la función de verificación
    connect(ui->bnOpcionA, &QPushButton::clicked, this, &Batalla::verificarRespuesta);
    connect(ui->bnOpcionB, &QPushButton::clicked, this, &Batalla::verificarRespuesta);
    connect(ui->bnOpcionC, &QPushButton::clicked, this, &Batalla::verificarRespuesta);
    connect(ui->bnOpcionD, &QPushButton::clicked, this, &Batalla::verificarRespuesta);



    //IA
    aiTimer = new QTimer(this);
    aiTimer->setSingleShot(true);
    connect(aiTimer, &QTimer::timeout, this, &Batalla::onAIRespondePrimero);



    initOverlay();
    // al final del constructor, después de initOverlay();
    strikeTimer = new QTimer(this);
    strikeTimer->setInterval(tickMs);
    connect(strikeTimer, &QTimer::timeout, this, &Batalla::tickStrike);

}

// Batalla.cpp
// Batalla.cpp (fragmentos relevantes)

// setSprite: recibe el sprite que venía caminando en EscenaInterior (puede ser temporal)
void Batalla::setSprite(isSprite *chica)
{
    // 'chica' es el sprite “portado” desde EscenaInterior, pero
    // todavía NO sabemos si el usuario lo va a elegir o cambiar.
    // Lo guardamos de forma provisional en 'jugador' hasta pedir la elección.
    jugador = chica;

    if (jugador && jugador->scene() && jugador->scene() != sceneBatalla) {
        jugador->scene()->removeItem(jugador);
    }
    if (jugador && !sceneBatalla->items().contains(jugador)) {
        sceneBatalla->addItem(jugador);
    }

    if (jugador) {
        jugador->setZValue(2);
        jugador->setPos(100, 440);
        jugador->quieto();
    }

    // Pedimos la elección SIN reentrancia
    QTimer::singleShot(0, this, [this]{ pedirAspectoJugador(); });
}

void Batalla::pedirAspectoJugador()
{
    ChooseApectDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) {
        // Si canceló, mantenemos el que venía de fuera como jugador
        // y creamos enemigo opuesto por inferencia
        ChooseApectDialog::Eleccion e = ChooseApectDialog::Empirista;
        // Si tu 'jugador' ya es un spriteV o spriteR puedes inferir aquí:
        if (dynamic_cast<spriteV*>(jugador)) e = ChooseApectDialog::Empirista;
        else if (dynamic_cast<spriteR*>(jugador)) e = ChooseApectDialog::Racionalista;

        enemigo = crearEnemigoOpuesto(e);
        if (enemigo) {
            sceneBatalla->addItem(enemigo);
        }
        colocarFrenteAFrente();
        lookAtEachOther();
        return;
    }

    // Usuario eligió explícitamente
    auto e = dlg.eleccion();

    // Reemplazamos 'jugador' por el elegido si es distinto
    isSprite* nuevo = crearSpritePorEleccion(e); // parent nullptr
    if (!nuevo) return;

    QPointF posBase = jugador ? jugador->pos() : QPointF(100, 440);

    if (jugador && jugador->scene() == sceneBatalla) {
        sceneBatalla->removeItem(jugador);
    }
    jugador = nuevo;
    sceneBatalla->addItem(jugador);
    jugador->setZValue(2);
    jugador->setPos(posBase);
    jugador->quieto();

    // Crear el oponente opuesto a la elección
    enemigo = crearEnemigoOpuesto(e);
    if (enemigo && !enemigo->scene()) {
        sceneBatalla->addItem(enemigo);
    }

    colocarFrenteAFrente();
    lookAtEachOther();

    // ¡Ahora sí! Arranca combate recién después de elegir
    QTimer::singleShot(0, this, [this]{ iniciarCombate(); });
}

isSprite* Batalla::crearSpritePorEleccion(ChooseApectDialog::Eleccion e)
{
    switch (e) {
    case ChooseApectDialog::Empirista:
        return new spriteV(nullptr);
    case ChooseApectDialog::Racionalista:
        return new spriteR(nullptr);
    default:
        return nullptr;
    }
}

// Crea el enemigo (IA) como el opuesto a la elección del jugador
isSprite* Batalla::crearEnemigoOpuesto(ChooseApectDialog::Eleccion e)
{
    switch (e) {
    case ChooseApectDialog::Empirista:
        return new spriteR(nullptr); // opuesto
    case ChooseApectDialog::Racionalista:
        return new spriteV(nullptr); // opuesto
    default:
        return nullptr;
    }
}

// Posicionamiento bonito: jugador a la izquierda, enemigo a la derecha
void Batalla::colocarFrenteAFrente()
{
    if (!jugador || !enemigo) return;

    // Suponiendo altura de suelo ~ y=440 según tu código
    jugador->setPos(160, 440);
    enemigo->setPos(560, 440);

    jugador->setZValue(2);
    enemigo->setZValue(2);

    jugador->quieto();
    enemigo->quieto();
}

void Batalla::iniciarCombate()
{
    // reset contadores
    numeroPreguntaActual = 0;
    respuestasCorrectas  = 0;

    // Pequeño conteo 3-2-1 antes de la primera (opcional pero útil)
    preRoundCountdown(3);
}

void Batalla::preRoundCountdown(int desde)
{
    // Usa tu overlay como “cartel”
    if (desde <= 0) {
        showTetricoOverlay("¡YA!", /*esRoto=*/false);
        QTimer::singleShot(500, this, [this]{
            siguientePregunta();
        });
        return;
    }

    showTetricoOverlay(QString::number(desde), /*esRoto=*/false);
    QTimer::singleShot(700, this, [this, desde]{
        preRoundCountdown(desde - 1);
    });
}


// Si tus sprites tienen orientación, aquí puedes voltearlos hacia el otro
void Batalla::lookAtEachOther()
{
    if (!jugador || !enemigo) return;

    // Ejemplos si implementaste algo como setMirandoDerecha(bool).
    // jugador->setMirandoDerecha(true);
    // enemigo->setMirandoDerecha(false);
    // Si no tienes orientación, puedes ignorar esto.
}








void Batalla::keyPressEvent(QKeyEvent *event)
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

    case Qt::Key_P:
        if (Chica) {
            Chica->atacar();
        }
        return;


    default:
        QWidget::keyPressEvent(event);
        return;
    }

    if (!animTimer->isActive())
        animTimer->start(90);
}

void Batalla::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
    case Qt::Key_S:
    case Qt::Key_Down:
        dy = 0;
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
    case Qt::Key_D:
    case Qt::Key_Right:
        dx = 0;
        break;
    default:
        QWidget::keyReleaseEvent(event);
        return;
    }

    // Si no hay movimiento, detiene la animación
    if (dx == 0 && dy == 0) {
        animTimer->stop();
        if (Chica) Chica->quieto();   // <-- chequeo
    }
}

void Batalla::animarMovimiento()
{
    if (!Chica) return;

    // Mueve el sprite
    Chica->mover(dx, dy);


}

// Mostrar una pregunta en el UI
// =============================
void Batalla::mostrarPregunta(const Pregunta &pregunta) {
    ui->lbPregunta->setText(pregunta.texto);
    ui->bnOpcionA->setText(pregunta.respuestaA);
    ui->bnOpcionB->setText(pregunta.respuestaB);
    ui->bnOpcionC->setText(pregunta.respuestaC);
    ui->bnOpcionD->setText(pregunta.respuestaD);
}

// Cargar la siguiente pregunta
// =============================
void Batalla::siguientePregunta() {
    numeroPreguntaActual++;

    if (numeroPreguntaActual > totalPreguntas) {
        const bool gano = (respuestasCorrectas == totalPreguntas);
        //////////////////////////////////////aquiii
        // Evaluar si ganó o perdió en base a las respuestas correctas
        if (!gano) {
            derrota();
            // Si quieres volver a otra escena tras unos segundos, puedes hacer:
            //QTimer::singleShot(3500, this, [this]{ regresarEscenaInterior(); });
        } else {
            // Victoria
            victoria();
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
    }



    // === Habilita botones y “activa” la carrera ===
    ui->bnOpcionA->setEnabled(true);
    ui->bnOpcionB->setEnabled(true);
    ui->bnOpcionC->setEnabled(true);
    ui->bnOpcionD->setEnabled(true);
    preguntaViva = true;

    // === IA: “gracia” + reacción humana ===
    // Gracia: 600–900 ms antes de que siquiera pueda empezar su timer
    const int graciaMs = 600 + QRandomGenerator::global()->bounded(300); // 600–900

    // Reacción IA: 2600–4200 ms (ajústalo a tu gusto)
    const int iaMin = 2600;
    const int iaMax = 4200;
    const int iaDelay = iaMin + QRandomGenerator::global()->bounded(iaMax - iaMin + 1);

    aiTimer->stop();

    // Primero la gracia...
    QTimer::singleShot(graciaMs, this, [this, iaDelay](){
        if (!preguntaViva) return; // por si ya respondió el jugador o timeout
        aiTimer->start(iaDelay);
    });
}

// Reiniciar el timer
// =============================





// Verificar la respuesta elegida
// =============================
void Batalla::verificarRespuesta() {
    if (!preguntaViva) return;
    preguntaViva = false;

    aiTimer->stop();

    auto *boton = qobject_cast<QPushButton*>(sender());
    if (!boton) return;

    char r = ' ';
    if (boton == ui->bnOpcionA) r = 'A';
    else if (boton == ui->bnOpcionB) r = 'B';
    else if (boton == ui->bnOpcionC) r = 'C';
    else if (boton == ui->bnOpcionD) r = 'D';

    // Bloquea entrada
    ui->bnOpcionA->setEnabled(false);
    ui->bnOpcionB->setEnabled(false);
    ui->bnOpcionC->setEnabled(false);
    ui->bnOpcionD->setEnabled(false);

    if (preguntaActual.respuestaCorrecta == r) {
        // Jugador ganó el “speed”
        showTetricoOverlay("¡Respondiste primero!", /*esRoto=*/false);
        strike(jugador, enemigo);          // <--- NUEVO
        respuestasCorrectas++;
    } else {
        // Jugador se equivocó
        showTetricoOverlay("¡Respuesta incorrecta!", /*esRoto=*/true);
        strike(enemigo, jugador);          // <--- NUEVOS
    }

    siguientePregunta();
}

void Batalla::onAIRespondePrimero()
{
    if (!preguntaViva) return;
    preguntaViva = false;

    ui->bnOpcionA->setEnabled(false);
    ui->bnOpcionB->setEnabled(false);
    ui->bnOpcionC->setEnabled(false);
    ui->bnOpcionD->setEnabled(false);

    // IA golpea por “speed”
    if (enemigo) enemigo->atacar();
    if (jugador) jugador->dolor();

    // Mensajito (si quieres mantenerlo)
    // QMessageBox::information(this, "IA", "La computadora respondió primero…");
    showTetricoOverlay("La IA respondió primero…", /*esRoto=*/true);

    strike(enemigo, jugador);              // <--- NUEVO
}

void Batalla::strike(isSprite* attacker, isSprite* defender)
{
    if (strikeInProgress || !attacker || !defender) {
        // fallback seguro
        QTimer::singleShot(200, this, [this]{ siguientePregunta(); });
        return;
    }

    // Paramos cualquier movimiento libre
    dx = dy = 0;
    if (animTimer && animTimer->isActive()) animTimer->stop();

    strikeInProgress = true;
    atkPtr = attacker;
    defPtr = defender;

    atkHome = atkPtr->pos();

    // Dirección hacia el oponente
    stepSign = (defPtr->x() > atkPtr->x()) ? +1 : -1;

    // ¿Cuántos pasos hasta tocar?
    int steps = qMax(1, advancePixels / qMax(1, stepPixels));
    stepsRemaining = steps;

    // Empezamos avanzando
    strikePhase = StrikePhase::Advance;
    strikeTimer->start();
}

void Batalla::tickStrike()
{
    if (!atkPtr || !defPtr) {
        strikeTimer->stop();
        strikeInProgress = false;
        QTimer::singleShot(150, this, [this]{ siguientePregunta(); });
        return;
    }

    switch (strikePhase) {
    case StrikePhase::Advance: {
        // avanzar hacia el rival
        atkPtr->mover(stepSign * stepPixels, 0);
        stepsRemaining--;

        if (stepsRemaining <= 0) {
            // Ejecuta el golpe
            if (atkPtr) atkPtr->atacar();
            if (defPtr) defPtr->dolor();

            // Pausa para que se vea la animación de ataque
            strikePhase = StrikePhase::AttackPause;
            strikeTimer->stop();
            QTimer::singleShot(pauseAttackMs, this, [this]{
                // prepara la retirada
                int stepsBack = qMax(1, advancePixels / qMax(1, stepPixels));
                stepsRemaining = stepsBack;
                strikePhase = StrikePhase::Retreat;
                strikeTimer->start();
            });
        }
        break;
    }

    case StrikePhase::AttackPause:
        // (no se usa; la pausa se maneja con singleShot)
        break;

    case StrikePhase::Retreat: {
        // regresar a casa
        atkPtr->mover(-stepSign * stepPixels, 0);
        stepsRemaining--;

        if (stepsRemaining <= 0) {
            strikeTimer->stop();

            // Correcciones finas: deja exactamente en su casa
            atkPtr->setPos(atkHome);
            atkPtr->quieto();
            if (defPtr) defPtr->quieto();

            strikeInProgress = false;

            // Pequeña pausa y seguimos con la siguiente
            QTimer::singleShot(180, this, [this]{
                siguientePregunta();
            });
        }
        break;
    }
    }
}





void Batalla::derrota()
{
    // 1) detener movimiento/animación y matar sprite
    // animTimer->stop();
    // dx = dy = 0;
    // if (Chica) {
    //     //Chica->morir();  // tu sprite ya tiene la secuencia de muerte
    // }


    // // 4) reproducir sonido de derrota
    // if (sonidoDerrota) {
    //     sonidoDerrota->stop();
    //     sonidoDerrota->play();
    // }





    // 5) desactivar botones de respuesta
    ui->bnOpcionA->setEnabled(false);
    ui->bnOpcionB->setEnabled(false);
    ui->bnOpcionC->setEnabled(false);
    ui->bnOpcionD->setEnabled(false);


    // (Opcional) overlay tétrico abajo
    showTetricoOverlay("xxx DERROTA xxx", /*esRoto=*/true);
}

void Batalla::victoria()
{
    // animTimer->stop();
    // dx = dy = 0;
    // if (Chica) Chica->quieto();




    // // 4) reproducir sonido de derrota
    // if (sonidoVictoria) {
    //     sonidoVictoria->stop();
    //     sonidoVictoria->play();
    // }

    ui->bnOpcionA->setEnabled(false);
    ui->bnOpcionB->setEnabled(false);
    ui->bnOpcionC->setEnabled(false);
    ui->bnOpcionD->setEnabled(false);

    // overlay
    showTetricoOverlay(" --> VICTORIA <-- ", /*esRoto=*/false);

    // desactivar botones

}

void Batalla::initOverlay() {
    // Padre: el graphicsView, para que se quede encima
    auto *view = ui->graphicsViewBatalla; // o graphicsViewArte
    overlay = new QFrame(view);
    overlay->setObjectName("overlayTetrico");
    overlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    overlay->setVisible(false);

    // Tamaño y posicion centrada (barra alta)
    const int w = view->width() - 600; // menos ancho
    const int h = 50;                  // menos alto
    overlay->setGeometry((view->width()-w)/2,
                         view->height() - h - 30, // posición Y abajo
                         w,
                         h);


    // Estilo “tétrico”
    overlay->setStyleSheet(
        "#overlayTetrico {"
        "  background: rgba(15, 0, 0, 190);"
        "  border: 2px solid rgba(255, 0, 0, 140);"
        "  border-radius: 14px;"
        "  box-shadow: 0 0 24px rgba(255,0,0,0.45);"
        "}"
        "QLabel {"
        "  color: #ffdbdb;"
        "  font-size: 15px;"
        "  font-weight: 800;"
        "  letter-spacing: 2px;"
        "}"
        );

    overlayLabel = new QLabel("...", overlay);
    overlayLabel->setAlignment(Qt::AlignCenter);
    overlayLabel->setWordWrap(true);
    overlayLabel->setGeometry(16, 10, w-32, h-20);

    // Efecto de opacidad + animaciones
    overlayFx = new QGraphicsOpacityEffect(overlay);
    overlay->setGraphicsEffect(overlayFx);

    fadeIn = new QPropertyAnimation(overlayFx, "opacity", overlay);
    fadeIn->setDuration(180);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);

    fadeOut = new QPropertyAnimation(overlayFx, "opacity", overlay);
    fadeOut->setDuration(220);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);

    // “Shake” (temblor) lateral
    shakeGroup = new QSequentialAnimationGroup(overlay);
    auto *shake1 = new QPropertyAnimation(overlay, "pos");
    auto *shake2 = new QPropertyAnimation(overlay, "pos");
    auto *shake3 = new QPropertyAnimation(overlay, "pos");
    auto *shake4 = new QPropertyAnimation(overlay, "pos");

    QPoint basePos = overlay->pos();
    int dx = 12;

    for (auto *a : {shake1,shake2,shake3,shake4}) a->setDuration(60);
    shake1->setStartValue(basePos);               shake1->setEndValue(basePos + QPoint(+dx, 0));
    shake2->setStartValue(basePos + QPoint(+dx,0));shake2->setEndValue(basePos + QPoint(-dx, 0));
    shake3->setStartValue(basePos + QPoint(-dx,0));shake3->setEndValue(basePos + QPoint(+dx/2, 0));
    shake4->setStartValue(basePos + QPoint(+dx/2,0));shake4->setEndValue(basePos);

    shakeGroup->addAnimation(shake1);
    shakeGroup->addAnimation(shake2);
    shakeGroup->addAnimation(shake3);
    shakeGroup->addAnimation(shake4);

    // Auto-ocultar
    autoHide = new QTimer(this);
    autoHide->setSingleShot(true);
    connect(autoHide, &QTimer::timeout, this, [this]{
        fadeOut->start();
        connect(fadeOut, &QPropertyAnimation::finished, this, [this]{ overlay->hide(); });
    });


}

void Batalla::showTetricoOverlay(const QString& txt, bool esRoto) {
    if (!overlay) return;

    overlayLabel->setText(txt);

    // Cambia el borde/fondo según roto/reparado
    if (esRoto) {
        overlay->setStyleSheet(
            "#overlayTetrico {"
            "  background: rgba(20, 0, 0, 205);"
            "  border: 2px solid rgba(255, 0, 0, 180);"
            "  border-radius: 14px;"
            "  box-shadow: 0 0 28px rgba(255,0,0,0.55);"
            "}"
            "QLabel { color: #ffb3b3; font-size: 15px; font-weight: 900; letter-spacing: 2px; }"
            );
    } else {
        overlay->setStyleSheet(
            "#overlayTetrico {"
            "  background: rgba(0, 25, 10, 205);"
            "  border: 2px solid rgba(0, 255, 140, 150);"
            "  border-radius: 14px;"
            "  box-shadow: 0 0 26px rgba(0,255,160,0.45);"
            "}"
            "QLabel { color: #d5ffe9; font-size: 15px; font-weight: 900; letter-spacing: 2px; }"
            );
    }

    overlay->show();
    overlayFx->setOpacity(0.0);
    fadeIn->start();
    shakeGroup->start();

    // Se queda ~1.1 s y se apaga
    autoHide->start(2200);
}

void Batalla::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event); // llama al comportamiento base

    if (!overlay || !overlayLabel || !ui || !ui->graphicsViewBatalla)
        return;

    auto *view = ui->graphicsViewBatalla;
    const int w = view->width() - 600; // menos ancho
    const int h = 50;                  // menos alto
    overlay->setGeometry((view->width()-w)/2,
                         view->height() - h - 30, // posición Y abajo
                         w,
                         h);
    overlayLabel->setGeometry(10, 5, w-20, h-10);

}




Batalla::~Batalla()
{
    delete ui;
}
