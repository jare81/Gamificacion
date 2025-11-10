#include "escenapolitica.h"
#include "ui_escenapolitica.h"
#include <QMessageBox>
#include "gamestate.h"

EscenaPolitica::EscenaPolitica(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EscenaPolitica)
    , scenePolitica(nullptr)
    , Chica(nullptr)
    , dx(0)
    , dy(0)
{
    ui->setupUi(this);

    // ----- Indicadores de vidrio -----
    vidrios = {
        ui->lbVidrio1,
        ui->lbVidrio2,
        ui->lbVidrio3,
        ui->lbVidrio4,
        ui->lbVidrio5
    };

    // Carga la imagen de vidrio roto (cambia la ruta a la tuya)
    iconoVidrioRoto.load("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/roto2.png");

    // Todos inician reparados
    estaRoto = QVector<bool>(vidrios.size(), false);
    for (auto *lbl : vidrios) lbl->clear();



    // Crear escena
    scenePolitica = new QGraphicsScene(this);
    ui->graphicsViewPolitica->setScene(scenePolitica);
    ui->graphicsViewPolitica->setRenderHint(QPainter::Antialiasing);
    ui->graphicsViewPolitica->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsViewPolitica->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    // Fondo nuevo
    QPixmap fondoDentro("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/backPolitica.jpg");
    fondoItemDentro = scenePolitica->addPixmap(
        fondoDentro.scaled(ui->graphicsViewPolitica->size(),
                           Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation));
    fondoItemDentro->setZValue(0);


    // --- Temporizador para animación ---
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &EscenaPolitica::animarMovimiento);


    // --- Asegurar que reciba eventos de teclado ---
    setFocusPolicy(Qt::StrongFocus);
    ui->graphicsViewPolitica->setFocusPolicy(Qt::NoFocus);
    this->setFocus();

    //PREGUNTAS
    preguntas.cargarPreguntasPolitica();
    totalPreguntas = 5;
    numeroPreguntaActual = 0;
    respuestasCorrectas = 0;


    // Conectar botones a la función de verificación
    connect(ui->bnOpcionA, &QPushButton::clicked, this, &EscenaPolitica::verificarRespuesta);
    connect(ui->bnOpcionB, &QPushButton::clicked, this, &EscenaPolitica::verificarRespuesta);
    connect(ui->bnOpcionC, &QPushButton::clicked, this, &EscenaPolitica::verificarRespuesta);
    connect(ui->bnOpcionD, &QPushButton::clicked, this, &EscenaPolitica::verificarRespuesta);


     siguientePregunta(); // Mostrar la primera

    // ----- Sonido de vidrio -----
    sonidoVidrio = new QSoundEffect(this);
    sonidoVidrio->setSource(QUrl::fromLocalFile(
        "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/glass1.wav"));
    sonidoVidrio->setVolume(0.9f); // volumen de 0.0 a 1.0

    //overlay
    initOverlay();

    //DERROTA
    // --- Fondo de derrota
    fondoDerrota.load("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/politicaBack2.png");

    // --- Icono de monstruo
    iconoMonstruo.load("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/sus.png");
    if (ui->sus) ui->sus->clear(); // oculto al inicio

    // --- Sonido de derrota
    sonidoDerrota = new QSoundEffect(this);
    sonidoDerrota->setSource(QUrl::fromLocalFile(
        "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/jump2.wav"));
    sonidoDerrota->setVolume(0.95f);

    // --- Sonido de victoria
    sonidoVictoria = new QSoundEffect(this);
    sonidoVictoria->setSource(QUrl::fromLocalFile(
        "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/victoria2.wav"));
    sonidoVictoria->setVolume(0.95f);



}

void EscenaPolitica::setSprite(isSprite *chica)
{
    Chica = chica;
    if (Chica) {
        scenePolitica->addItem(Chica);
        Chica->setZValue(2);
        Chica->setPos(220, 370);
    }


}

void EscenaPolitica::keyPressEvent(QKeyEvent *event)
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
            dx = dy = 0;
            animTimer->stop();

            Chica->atacar();
        }
        return; // ← MUY IMPORTANTE: no sigas a start(90)


    default:
        QWidget::keyPressEvent(event);
        return;
    }

    if (!animTimer->isActive())
        animTimer->start(90);
}

void EscenaPolitica::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;
    dx = dy = 0;
    animTimer->stop();
    if (Chica) Chica->quieto();
}



void EscenaPolitica::animarMovimiento()
{
    if (!Chica) return;

    // Mueve el sprite
    Chica->mover(dx, dy);

}

void EscenaPolitica::mostrarPregunta(const Pregunta &pregunta) {
    ui->lbPregunta->setText(pregunta.texto);
    ui->bnOpcionA->setText(pregunta.respuestaA);
    ui->bnOpcionB->setText(pregunta.respuestaB);
    ui->bnOpcionC->setText(pregunta.respuestaC);
    ui->bnOpcionD->setText(pregunta.respuestaD);
}

// Cargar la siguiente pregunta
// =============================
void EscenaPolitica::siguientePregunta() {
    numeroPreguntaActual++;

    // // Si ya no hay más preguntas, evaluar resultado final
    // if (numeroPreguntaActual > totalPreguntas) {
    //     // Mostrar resumen
    //     QString mensaje = QString("Has contestado %1 de %2 correctamente.")
    //                           .arg(respuestasCorrectas)
    //                           .arg(totalPreguntas);

    //     if (respuestasCorrectas < totalPreguntas) {
    //         QMessageBox::warning(this, "Fin del juego", mensaje + "\nPerdiste el reto de politica... ");

    //         // Ejecuta animación de muerte, pero no la dejes en ese estado permanentemente
    //         if (Chica) {
    //             Chica->morir();
    //         }

    //         regresarEscenaInterior();


    //     }
    //     else {
    //         QMessageBox::information(this, "¡Excelente!", mensaje + "\n¡Has sobrevivido al reto de ciencia! ");

    //         regresarEscenaInterior();


    //     }

    if (numeroPreguntaActual > totalPreguntas) {
        const bool gano = (respuestasCorrectas == totalPreguntas);

        // **Registrar resultado en el estado global**
        Gamestate::instance().registrarResultado(Gamestate::Politica, gano);


        if (!gano) {
            // Antes hacías QMessageBox + morir + regresarEscenaInterior();
            derrota();
            // Si quieres volver a otra escena tras unos segundos, puedes hacer:
             QTimer::singleShot(3500, this, [this]{ regresarEscenaInterior(); });
        } else {
            // Victoria
            victoria();               // (opcional, crea un método similar para victoria)
            // O tu lógica actual:
            // showTetricoOverlay("¡VICTORIA!", /*esRoto=*/false);
            // regresarEscenaInterior();
             QTimer::singleShot(3500, this, [this]{ regresarEscenaInterior(); });
        }
        return;
    }


    // Mostrar progreso
    ui->lbCantidad->setText(
        QString("%1 / %2").arg(numeroPreguntaActual).arg(totalPreguntas)
        );

    // Mostrar siguiente pregunta
    if (!preguntas.isEmpty()) {
        preguntaActual = preguntas.dequeue();
        mostrarPregunta(preguntaActual);
    }
}


// Verificar la respuesta elegida
// =============================
void EscenaPolitica::verificarRespuesta() {

    QPushButton *boton = qobject_cast<QPushButton*>(sender());
    if (!boton) return;

    char respuesta = ' ';
    if (boton == ui->bnOpcionA) respuesta = 'A';
    else if (boton == ui->bnOpcionB) respuesta = 'B';
    else if (boton == ui->bnOpcionC) respuesta = 'C';
    else if (boton == ui->bnOpcionD) respuesta = 'D';

    // 1) Disparar animación de ataque
    if (Chica) Chica->atacar();
    // sonido de vidrio roto



    // 2) Efecto vidrio + feedback
    if (preguntaActual.respuestaCorrecta == respuesta) {
        respuestasCorrectas++;
        repararVidrioUno();  // repara un vidrio roto al azar (si hay)
        showTetricoOverlay("El vidrio se repara", /*esRoto=*/false);
    } else {
        romperVidrioAleatorio(); // rompe un vidrio sano al azar (si queda alguno)
        showTetricoOverlay("¡EL vidrio se esta quebrando!", /*esRoto=*/true);

    }

    // 3) Pasar a la siguiente pregunta
    siguientePregunta();

}



void EscenaPolitica::setEscenaAnterior(QWidget *escena)
{
    escenaAnterior = escena;
}

void EscenaPolitica::regresarEscenaInterior()
{
    // Detener animación
    animTimer->stop();
    dx = dy = 0;
    if (Chica) Chica->quieto();

    //  la escena anterior
    if (escenaAnterior) {
        EscenaInterior *interior = qobject_cast<EscenaInterior*>(escenaAnterior);
        if (interior && Chica) {
            scenePolitica->removeItem(Chica);       // quitarlo de esta escena
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

//VIDRIOS ROTOS
void EscenaPolitica::setVidrioEstado(int idx, bool roto) {
    if (idx < 0 || idx >= vidrios.size()) return;
    estaRoto[idx] = roto;

    if (roto) {
        // Escalar para que se vea bien en el label
        vidrios[idx]->setPixmap(
            iconoVidrioRoto.scaled(vidrios[idx]->size(),
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation));
        vidrios[idx]->setVisible(true);
    } else {
        vidrios[idx]->clear(); // “reparado”: quitamos la imagen
    }
}

void EscenaPolitica::romperVidrioAleatorio() {
    QList<int> sanos;
    for (int i = 0; i < estaRoto.size(); ++i)
        if (!estaRoto[i]) sanos << i;

    if (sanos.isEmpty()) return; // todos están rotos, no hacemos nada

    int pick = sanos.at(QRandomGenerator::global()->bounded(sanos.size()));
    setVidrioEstado(pick, true);

    if (sonidoVidrio) {
        sonidoVidrio->stop();  // reinicia si se está reproduciendo
        sonidoVidrio->play();
    }


}

void EscenaPolitica::repararVidrioUno() {
    QList<int> rotos;
    for (int i = 0; i < estaRoto.size(); ++i)
        if (estaRoto[i]) rotos << i;

    if (rotos.isEmpty()) return; // no hay nada que reparar

    int pick = rotos.at(QRandomGenerator::global()->bounded(rotos.size()));
    setVidrioEstado(pick, false);
}

void EscenaPolitica::derrota()
{
    // 1) detener movimiento/animación y matar sprite
    animTimer->stop();
    dx = dy = 0;
    if (Chica) {
        Chica->morir();  // tu sprite ya tiene la secuencia de muerte
    }

    // 2) cambiar el fondo de la escena
    if (!fondoDerrota.isNull() && scenePolitica && ui->graphicsViewPolitica) {
        QPixmap scaled = fondoDerrota.scaled(
            ui->graphicsViewPolitica->size(),
            Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation);

        if (fondoItemDentro) {
            fondoItemDentro->setPixmap(scaled);
            fondoItemDentro->setZValue(0);
        } else {
            // fallback si no existía
            fondoItemDentro = scenePolitica->addPixmap(scaled);
            fondoItemDentro->setZValue(0);
        }
    }

    // 4) reproducir sonido de derrota
    if (sonidoDerrota) {
        sonidoDerrota->stop();
        sonidoDerrota->play();
    }

    // 3) mostrar monstruo en label específico
    if (ui->sus && !iconoMonstruo.isNull()) {
        ui->sus->setPixmap(
            iconoMonstruo.scaled(ui->sus->size(),
                                 Qt::KeepAspectRatio,
                                 Qt::SmoothTransformation));
        ui->sus->setVisible(true);
    }



    // 5) desactivar botones de respuesta
    ui->bnOpcionA->setEnabled(false);
    ui->bnOpcionB->setEnabled(false);
    ui->bnOpcionC->setEnabled(false);
    ui->bnOpcionD->setEnabled(false);

    ui->lbVidrio1->setEnabled(false);
    ui->lbVidrio2->setEnabled(false);
    ui->lbVidrio3->setEnabled(false);
    ui->lbVidrio4->setEnabled(false);
    ui->lbVidrio5->setEnabled(false);

    // (Opcional) overlay tétrico abajo
    showTetricoOverlay(" DERROTA ", /*esRoto=*/true);
}

void EscenaPolitica::victoria()
{
    animTimer->stop();
    dx = dy = 0;
    if (Chica) Chica->quieto();

    // puedes cambiar a un fondo de victoria si quieres:
    // QPixmap fondoWin(".../backPolitica_victoria.jpg"); ... setPixmap(...)

    // ocultar monstruo si estaba visible
    if (ui->sus) ui->sus->clear();

    // 4) reproducir sonido de derrota
    if (sonidoVictoria) {
        sonidoVictoria->stop();
        sonidoVictoria->play();
    }

    // overlay
    showTetricoOverlay(" VICTORIA ", /*esRoto=*/false);

    // desactivar botones
    ui->bnOpcionA->setEnabled(false);
    ui->bnOpcionB->setEnabled(false);
    ui->bnOpcionC->setEnabled(false);
    ui->bnOpcionD->setEnabled(false);
}






void EscenaPolitica::initOverlay() {
    // Padre: el graphicsView, para que se quede encima
    auto *view = ui->graphicsViewPolitica; // o graphicsViewArte
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

void EscenaPolitica::showTetricoOverlay(const QString& txt, bool esRoto) {
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

void EscenaPolitica::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event); // llama al comportamiento base

    if (!overlay || !overlayLabel || !ui || !ui->graphicsViewPolitica)
        return;

    auto *view = ui->graphicsViewPolitica;
    const int w = view->width() - 600; // menos ancho
    const int h = 50;                  // menos alto
    overlay->setGeometry((view->width()-w)/2,
                         view->height() - h - 30, // posición Y abajo
                         w,
                         h);
    overlayLabel->setGeometry(10, 5, w-20, h-10);

}







EscenaPolitica::~EscenaPolitica()
{
    delete ui;
}
