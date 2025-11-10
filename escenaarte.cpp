#include "escenaarte.h"
#include "ui_escenaarte.h"
#include <QMessageBox>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QEasingCurve>


EscenaArte::EscenaArte(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EscenaArte)
    , sceneArte(nullptr)
    , Chica(nullptr)
    , dx(0)
    , dy(0)
    , fogRight(nullptr)
    , fogAnim(nullptr)
    , fogProgress(0.0)
    , escenaAnterior(nullptr)
{
    ui->setupUi(this);

    // Crear escena
    // --- Escena y View ---
    sceneArte = new QGraphicsScene(this);
    ui->graphicsViewArte->setScene(sceneArte);
    ui->graphicsViewArte->setRenderHint(QPainter::Antialiasing);
    ui->graphicsViewArte->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsViewArte->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsViewArte->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->graphicsViewArte->setFrameShape(QFrame::NoFrame);
    ui->graphicsViewArte->setBackgroundBrush(Qt::NoBrush);

    // Placeholder: la escena tendrá el tamaño real cuando el widget ya esté visible
    sceneArte->setSceneRect(0, 0, 1, 1);

    // --- Fondo (item vacío por ahora; se escalará en showEvent/resizeEvent) ---
    fondoItemDentro = sceneArte->addPixmap(QPixmap());
    fondoItemDentro->setZValue(0);
    fondoItemDentro->setPos(0, 0);

    // Fondo anclado al (0,0) de la escena
    QSize viewSz = ui->graphicsViewArte->viewport()->size();
    QPixmap fondoDentro("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/backArte1.gif");
    QPixmap fondoEscalado = fondoDentro.scaled(viewSz, Qt::KeepAspectRatioByExpanding,
                                               Qt::SmoothTransformation);

    // Niebla
    initNiebla();

    // Temporizador para animación del personaje
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &EscenaArte::animarMovimiento);

    // Foco
    setFocusPolicy(Qt::StrongFocus);
    ui->graphicsViewArte->setFocusPolicy(Qt::NoFocus);
    this->setFocus();

    // PREGUNTAS
    preguntas.cargarPreguntasArte();
    totalPreguntas = 5;
    numeroPreguntaActual = 0;
    respuestasCorrectas = 0;

    // Conectar botones a la verificación
    connect(ui->bnOpcionA, &QPushButton::clicked, this, &EscenaArte::verificarRespuesta);
    connect(ui->bnOpcionB, &QPushButton::clicked, this, &EscenaArte::verificarRespuesta);
    connect(ui->bnOpcionC, &QPushButton::clicked, this, &EscenaArte::verificarRespuesta);
    connect(ui->bnOpcionD, &QPushButton::clicked, this, &EscenaArte::verificarRespuesta);

    siguientePregunta(); // Mostrar la primera

    // Ajusta todo al tamaño real cuando ya esté mostrado
    QTimer::singleShot(0, this, [this]{ ajustarATamanoDeView(); });


    initOverlayArte();

}

void EscenaArte::setSprite(isSprite *chica)
{
    Chica = chica;
    if (Chica) {
        sceneArte->addItem(Chica);
        Chica->setZValue(1);       // Si quieres que la niebla tape a la chica, deja la niebla con Z mayor (1.5 ya lo es)
        Chica->setPos(460, 420);
    }
}

void EscenaArte::keyPressEvent(QKeyEvent *event)
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

void EscenaArte::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;
    dx = dy = 0;
    animTimer->stop();
    if (Chica) Chica->quieto();
}

void EscenaArte::animarMovimiento()
{
    if (!Chica) return;
    Chica->mover(dx, dy);
    checkFogCollision();
}

void EscenaArte::mostrarPregunta(const Pregunta &pregunta)
{
    ui->lbPregunta->setText(pregunta.texto);
    ui->bnOpcionA->setText(pregunta.respuestaA);
    ui->bnOpcionB->setText(pregunta.respuestaB);
    ui->bnOpcionC->setText(pregunta.respuestaC);
    ui->bnOpcionD->setText(pregunta.respuestaD);
}

void EscenaArte::siguientePregunta()
{
    numeroPreguntaActual++;

    // Fin del reto
    if (numeroPreguntaActual > totalPreguntas) {

        const bool gano = (respuestasCorrectas == totalPreguntas);

        // **Registrar resultado en el estado global**
        Gamestate::instance().registrarResultado(Gamestate::Arte, gano);

        QString mensaje = QString("Has contestado %1 de %2 correctamente.")
        .arg(respuestasCorrectas)
            .arg(totalPreguntas);

        if (!gano) {
             derrota();
        } else {
             victoria();
        }
        return;
    }

    // Progreso
    ui->lbCantidad->setText(QString("%1 / %2").arg(numeroPreguntaActual).arg(totalPreguntas));

    // Mostrar siguiente
    if (!preguntas.isEmpty()) {
        preguntaActual = preguntas.dequeue();
        mostrarPregunta(preguntaActual);
    }
}

bool EscenaArte::isCollidingWithFog() const
{
    if (!Chica || !fogLeft || !fogRight) return false;
    if (fogLeft->opacity() <= 0.05 && fogRight->opacity() <= 0.05) return false;

    // Usa la forma (alfa) gracias a MaskShape
    return fogLeft->collidesWithItem(Chica, Qt::IntersectsItemShape) ||
           fogRight->collidesWithItem(Chica, Qt::IntersectsItemShape);
}

void EscenaArte::checkFogCollision()
{
    if (gameOver) return;
    if (isCollidingWithFog()) {
        derrota();   // aquí se gatilla la animación de muerte
    }
}

void EscenaArte::derrota()
{
    if (gameOver) return;
    gameOver = true;

    if (animTimer) animTimer->stop();
    dx = dy = 0;

    if (Chica) Chica->morir();  // << animación de muerte del sprite

    // (Opcional) cambia fondo si quieres:
    // QPixmap fondoDerrota(":/images/backArte_derrota.jpg");
    // if (!fondoDerrota.isNull() && fondoItemDentro) {
    //     fondoItemDentro->setPixmap(
    //         fondoDerrota.scaled(ui->graphicsViewArte->size(),
    //                             Qt::KeepAspectRatioByExpanding,
    //                             Qt::SmoothTransformation));
    // }

    //if (sonidoDerrota) { sonidoDerrota->stop(); sonidoDerrota->play(); }

    // Desactiva botones de respuesta
    ui->bnOpcionA->setEnabled(false);
    ui->bnOpcionB->setEnabled(false);
    ui->bnOpcionC->setEnabled(false);
    ui->bnOpcionD->setEnabled(false);

    showOverlayArte(" D E R R O T A ", /*estiloDerrota=*/true);

    // (Opcional) volver a escena anterior tras unos segundos:
    QTimer::singleShot(2600, this, [this]{
        regresarEscenaInterior();
    });
}

void EscenaArte::victoria()
{
    if (gameOver) return;
    gameOver = true;

    if (animTimer) animTimer->stop();
    dx = dy = 0;
    if (Chica) Chica->quieto();

    //if (sonidoVictoria) { sonidoVictoria->stop(); sonidoVictoria->play(); }

    // Desactiva botones
    ui->bnOpcionA->setEnabled(false);
    ui->bnOpcionB->setEnabled(false);
    ui->bnOpcionC->setEnabled(false);
    ui->bnOpcionD->setEnabled(false);

    showOverlayArte(" V I C T O R I A ", /*estiloDerrota=*/false);

    QTimer::singleShot(2200, this, [this]{
        regresarEscenaInterior();
    });
}






void EscenaArte::verificarRespuesta()
{
    QPushButton *boton = qobject_cast<QPushButton*>(sender());
    if (!boton) return;

    char respuesta = ' ';
    if (boton == ui->bnOpcionA) respuesta = 'A';
    else if (boton == ui->bnOpcionB) respuesta = 'B';
    else if (boton == ui->bnOpcionC) respuesta = 'C';
    else if (boton == ui->bnOpcionD) respuesta = 'D';

    const double pasoMal  = 0.20; // se cierra por error
    const double pasoBien = 0.15; // se abre por acierto

    if (preguntaActual.respuestaCorrecta == respuesta) {
        respuestasCorrectas++;
        showOverlayArte("¡Correcto! La niebla se aleja", /*estiloDerrota=*/false);
        animateFogTo(fogProgress - pasoBien); // abrir un poco
    } else {
         showOverlayArte("Incorrecto... la niebla se acerca", /*estiloDerrota=*/true);
        animateFogTo(fogProgress + pasoMal);  // cerrar un poco
    }

    // Nota: fogProgress se actualiza durante la animación; si quieres derrota inmediata al llegar a 1.0,
    // podrías mover esta lógica a setFogProgress() cuando detecte >= 0.999.
    if (fogProgress >= 0.999) {
        derrota();
        return;
    }

    siguientePregunta();
}

void EscenaArte::setEscenaAnterior(QWidget *escena)
{
    escenaAnterior = escena;
}

void EscenaArte::regresarEscenaInterior()
{
    // Detener animación
    if (animTimer) animTimer->stop();
    dx = dy = 0;
    if (Chica) Chica->quieto();

    // Volver a la escena anterior
    if (escenaAnterior) {
        EscenaInterior *interior = qobject_cast<EscenaInterior*>(escenaAnterior);
        if (interior && Chica) {
            sceneArte->removeItem(Chica);                 // quitarlo de esta escena
            interior->agregarSpriteDesdeHistoria(Chica);  // pasarlo a la anterior
        }

        escenaAnterior->show();
        this->close();

        QTimer::singleShot(50, [interior]() {
            if (interior) {
                interior->activateWindow();
                interior->raise();
                interior->setFocus();
                if (interior->getGraphicsView())
                    interior->getGraphicsView()->clearFocus();
            }
        });
    } else {
        this->close();
    }
}


void EscenaArte::initNiebla()
{
    // Carga del PNG de niebla (con alfa)
    fogPixmap.load("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/niebla.png");

    fogLeft  = sceneArte->addPixmap(QPixmap());
    fogRight = sceneArte->addPixmap(QPixmap());

    for (auto* f : {fogLeft, fogRight}) {
        f->setZValue(1.5);
        f->setOpacity(0.95);
        f->setPos(0,0);
        f->setShapeMode(QGraphicsPixmapItem::MaskShape); // << usa el alfa para colisiones
        f->setFlag(QGraphicsItem::ItemIsSelectable, false);
        f->setFlag(QGraphicsItem::ItemIsMovable, false);
    }

    fogAnim = new QVariantAnimation(this);
    fogAnim->setEasingCurve(QEasingCurve::InOutSine);
    connect(fogAnim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v){
        setFogProgress(v.toDouble());
    });

    setFogProgress(0.0);
}

void EscenaArte::setFogProgress(double p)
{
    fogProgress = std::clamp(p, 0.0, 1.0);
    layoutFog();
}

void EscenaArte::animateFogTo(double target, int ms)
{
    target = std::clamp(target, 0.0, 1.0);
    if (!fogAnim) return;

    fogAnim->stop();
    fogAnim->setDuration(ms);
    fogAnim->setStartValue(fogProgress);
    fogAnim->setEndValue(target);
    fogAnim->start();
}



void EscenaArte::layoutFog()
{
    if (!fogLeft || !fogRight) return;

    const QRectF sr = sceneArte->sceneRect();

    // ---- Parámetros para "más niebla" ----
    const double fogOverlap   = 0.60;                 // solape extra por lado al 100% (20%)
    const double baseOpacity  = 0.90;                 // opacidad base alta
    const double extraOpacity = 0.40 * fogProgress;   // más denso al cerrar

    // ---- Reescalado de sprites de niebla ----
    const int targetH = int(sr.height());
    bool needRescale = fogLeft->pixmap().isNull() || (fogLeft->pixmap().height() != targetH);

    // Calcula el ancho mínimo por lado: 50% + solape, con un margen extra
    const int minHalfW = int(sr.width() * (0.5 + fogOverlap)) + 32;

    if (needRescale) {
        // Escalar por altura primero
        QPixmap fogScaled = fogPixmap.scaledToHeight(targetH, Qt::SmoothTransformation);
        // Si la textura es angosta, ensanchar para cubrir el solape deseado
        if (fogScaled.width() < minHalfW) {
            fogScaled = fogScaled.scaled(minHalfW, targetH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }

        fogLeft->setPixmap(fogScaled);
        fogRight->setPixmap(fogScaled.transformed(QTransform().scale(-1, 1)));
    } else {
        // La altura ya coincide; asegurar ancho mínimo por cambios de tamaño de la escena
        if (fogLeft->pixmap().width() < minHalfW) {
            QPixmap fogScaled = fogPixmap.scaled(minHalfW, targetH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            fogLeft->setPixmap(fogScaled);
            fogRight->setPixmap(fogScaled.transformed(QTransform().scale(-1, 1)));
        }
    }

    // ---- Posicionamiento según progreso ----
    const double wL     = fogLeft->pixmap().width();
    const double sceneW = sr.width();

    // 0.0 = abierto; 1.0 = cerrado con solape al centro
    const double maxInvasionPerSide = sceneW * (0.5 + fogOverlap);
    const double invasion = maxInvasionPerSide * std::clamp(fogProgress, 0.0, 1.0);

    const double leftX  = sr.left() + (-wL + invasion);     // entra desde fuera por izquierda
    const double rightX = sr.left() + (sceneW - invasion);  // entra por derecha

    fogLeft->setPos(leftX,  sr.top());
    fogRight->setPos(rightX, sr.top());

    // ---- Densidad (opacidad) ----
    const double op = std::clamp(baseOpacity + extraOpacity, 0.0, 1.0);
    fogLeft->setOpacity(op);
    fogRight->setOpacity(op);



    checkFogCollision();


}


void EscenaArte::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    ajustarATamanoDeView();

    if (overlay && ui->graphicsViewArte) {
        const int w = ui->graphicsViewArte->width() - 600;
        const int h = 54;
        overlay->setGeometry((ui->graphicsViewArte->width()-w)/2,
                             ui->graphicsViewArte->height() - h - 28,
                             w, h);
    }
}

void EscenaArte::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    ajustarATamanoDeView();
}

void EscenaArte::ajustarATamanoDeView()
{
    // Tamaño real del área donde pinta el QGraphicsView
    const QSize viewSz = ui->graphicsViewArte->viewport()->size();
    if (viewSz.isEmpty()) return;

    // Fija el rectángulo de escena al tamaño del viewport
    sceneArte->setSceneRect(0, 0, viewSz.width(), viewSz.height());
    playArea = sceneArte->sceneRect();

    // --- Fondo a pantalla completa ---
    static QPixmap fondoSrc("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/backArte1.gif");
    QPixmap fondoEsc = fondoSrc.scaled(viewSz, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    if (fondoItemDentro) {
        fondoItemDentro->setPixmap(fondoEsc);
        fondoItemDentro->setPos(0, 0);
        fondoItemDentro->setZValue(0);
    }

    // --- Niebla a pantalla completa (alto de la escena) ---
    if (!fogPixmap.isNull()) {
        QPixmap fogScaled = fogPixmap.scaledToHeight(viewSz.height(), Qt::SmoothTransformation);
        if (fogLeft)  fogLeft->setPixmap(fogScaled);
        if (fogRight) fogRight->setPixmap(fogScaled.transformed(QTransform().scale(-1, 1)));
    }

    // Recoloca la niebla según el progreso actual
    layoutFog();
}

void EscenaArte::initOverlayArte()
{
    auto *view = ui->graphicsViewArte;
    overlay = new QFrame(view);
    overlay->setObjectName("overlayArte");
    overlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    overlay->setVisible(false);

    const int w = view->width() - 600;
    const int h = 54;
    overlay->setGeometry((view->width()-w)/2,
                         view->height() - h - 28,
                         w, h);

    overlay->setStyleSheet(
        "#overlayArte {"
        "  background: rgba(15, 0, 0, 190);"
        "  border: 2px solid rgba(255, 0, 0, 140);"
        "  border-radius: 14px;"
        "}"
        "QLabel {"
        "  color: #ffdbdb;"
        "  font-size: 16px;"
        "  font-weight: 800;"
        "  letter-spacing: 2px;"
        "}"
        );

    overlayLabel = new QLabel("...", overlay);
    overlayLabel->setAlignment(Qt::AlignCenter);
    overlayLabel->setWordWrap(true);
    overlayLabel->setGeometry(16, 10, w-32, h-20);

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

    // shake
    shakeGroup = new QSequentialAnimationGroup(overlay);
    auto *shake1 = new QPropertyAnimation(overlay, "pos");
    auto *shake2 = new QPropertyAnimation(overlay, "pos");
    auto *shake3 = new QPropertyAnimation(overlay, "pos");
    auto *shake4 = new QPropertyAnimation(overlay, "pos");

    QPoint basePos = overlay->pos();
    int dxShake = 12;
    for (auto *a : {shake1,shake2,shake3,shake4}) a->setDuration(60);
    shake1->setStartValue(basePos);                 shake1->setEndValue(basePos + QPoint(+dxShake, 0));
    shake2->setStartValue(basePos + QPoint(+dxShake,0)); shake2->setEndValue(basePos + QPoint(-dxShake, 0));
    shake3->setStartValue(basePos + QPoint(-dxShake,0)); shake3->setEndValue(basePos + QPoint(+dxShake/2, 0));
    shake4->setStartValue(basePos + QPoint(+dxShake/2,0)); shake4->setEndValue(basePos);

    shakeGroup->addAnimation(shake1);
    shakeGroup->addAnimation(shake2);
    shakeGroup->addAnimation(shake3);
    shakeGroup->addAnimation(shake4);

    autoHide = new QTimer(this);
    autoHide->setSingleShot(true);
    connect(autoHide, &QTimer::timeout, this, [this]{
        fadeOut->start();
        connect(fadeOut, &QPropertyAnimation::finished, this, [this]{ overlay->hide(); });
    });
}

void EscenaArte::showOverlayArte(const QString& txt, bool estiloDerrota)
{
    if (!overlay) return;

    overlayLabel->setText(txt);

    if (estiloDerrota) {
        overlay->setStyleSheet(
            "#overlayArte {"
            "  background: rgba(20, 0, 0, 205);"
            "  border: 2px solid rgba(255, 0, 0, 180);"
            "  border-radius: 14px;"
            "}"
            "QLabel { color: #ffb3b3; font-size: 16px; font-weight: 900; letter-spacing: 2px; }"
            );
    } else {
        overlay->setStyleSheet(
            "#overlayArte {"
            "  background: rgba(0, 25, 10, 205);"
            "  border: 2px solid rgba(0, 255, 140, 150);"
            "  border-radius: 14px;"
            "}"
            "QLabel { color: #d5ffe9; font-size: 16px; font-weight: 900; letter-spacing: 2px; }"
            );
    }

    overlay->show();
    overlayFx->setOpacity(0.0);
    fadeIn->start();
    shakeGroup->start();

    autoHide->start(1800);
}



EscenaArte::~EscenaArte()
{
    delete ui;
}
