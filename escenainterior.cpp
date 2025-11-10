#include "escenainterior.h"
#include "ui_escenainterior.h"
#include "ruleta.h"
#include "escenahistoria.h"
#include "escenaciencia.h"
#include "escenapolitica.h"
#include "escenaarte.h"
#include "batalla.h"
#include <QDebug>

#include <QMessageBox>



EscenaInterior::EscenaInterior(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EscenaInterior)
    , sceneDentro(nullptr)
    , Chica(nullptr)
    , dx(0)
    , dy(0)
{
    ui->setupUi(this);

    // Crear escena
    sceneDentro = new QGraphicsScene(this);
    ui->graphicsViewDentro->setScene(sceneDentro);
    ui->graphicsViewDentro->setRenderHint(QPainter::Antialiasing);
    ui->graphicsViewDentro->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsViewDentro->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Fondo nuevo
    QPixmap fondoDentro("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/puertas.png");
    fondoItemDentro = sceneDentro->addPixmap(
        fondoDentro.scaled(ui->graphicsViewDentro->size(),
                           Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation));
    fondoItemDentro->setZValue(0);

    // --- Temporizador para animación ---
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &EscenaInterior::animarMovimiento);



    // --- Crear RUETA como miembro (no static en el lambda) ---
    ruleta = new Ruleta(this);
    sceneDentro->addItem(ruleta);
    ruleta->setPos(435, 40);
    ruleta->setZValue(3);

    // Cambiar fondo cuando ruleta decide
    connect(ruleta, &Ruleta::fondoCambiado, this, &EscenaInterior::actualizarFondo);

    // Feedback cuando ya no hay giros
    connect(ruleta, &Ruleta::sinGiros, this, [this](){
        // Opcional: deshabilitar botón de girar
        if (ui->giro) ui->giro->setEnabled(false);
    });

    // Botón "Girar"
    connect(ui->giro, &QPushButton::clicked, this, [this](){
        if (ruleta) ruleta->iniciarGiro();
    });

    // // --- Crear la ruleta y agregarla a la escena ---
    // connect(ui->giro, &QPushButton::clicked, this, [this]() {
    //     static Ruleta *ruleta = nullptr;

    //     if (!ruleta) {
    //         ruleta = new Ruleta(this);
    //         sceneDentro->addItem(ruleta);
    //         ruleta->setPos(435, 40); // posición en la escena
    //         ruleta->setZValue(3);     // encima del fondo

    //         //Conectar la señal cambia el fondo
    //         connect(ruleta, &Ruleta::fondoCambiado, this, &EscenaInterior::actualizarFondo);

    //     }



    //     ruleta->iniciarGiro();
    // });




    // --- Asegurar que reciba eventos de teclado ---
    setFocusPolicy(Qt::StrongFocus);
    ui->graphicsViewDentro->setFocusPolicy(Qt::NoFocus);
    this->setFocus();


    //Limpiar estado de teclado (por si hay key repeat activo)
    if (Chica) {
        Chica->quieto();
    }
    dx = dy = 0;
    animTimer->stop();
    QCoreApplication::processEvents();





    // Crear paredes
    //El método addRect(x (posición horizontal (desde la izquierda)), y(posición vertical (desde arriba)), ancho, alto, ...)
    QGraphicsRectItem *puertaHistoria = sceneDentro->addRect(160, 250, 120, 100, QPen(Qt::NoPen), QBrush(Qt::NoBrush));
    puertaHistoria->setData(0, "historia");

    QGraphicsRectItem *puertaCiencia = sceneDentro->addRect(330, 250, 120, 100, QPen(Qt::NoPen), QBrush(Qt::NoBrush));
    puertaCiencia->setData(0, "ciencia");

    QGraphicsRectItem *puertaPolitica = sceneDentro->addRect(500, 250, 120, 100, QPen(Qt::NoPen), QBrush(Qt::NoBrush));
    puertaPolitica->setData(0, "politica");


    QGraphicsRectItem *puertaArte = sceneDentro->addRect(675, 250, 120, 100, QPen(Qt::NoPen), QBrush(Qt::NoBrush));
    puertaArte->setData(0, "arte");
    //puertaPolitica->setZValue(1);


    // === HUD Corazones ===
    crearHUDCorazones();
    actualizarHUDCorazones(GS.vidas());
    connect(&GS, &Gamestate::vidasCambiaron, this, &EscenaInterior::actualizarHUDCorazones);

    // Si la ruleta se resetea (jugó 4 y ganó 0)…
    connect(&GS, &Gamestate::ruletaReset, this, [this](){
        QMessageBox::information(this, "Inténtalo de nuevo",
                                 "Jugaste las 4 puertas y no ganaste vidas.\n"
                                 "La ruleta se ha reseteado: puedes volver a intentarlo.");
        if (ruleta) ruleta->reset();
        if (ui->giro) ui->giro->setEnabled(true);
    });

    // si cambian puertas o vidas, revisa si activar el portal
    connect(&GS, &Gamestate::puertasActualizadas, this, &EscenaInterior::actualizarParedBatalla);
    connect(&GS, &Gamestate::vidasCambiaron,      this, &EscenaInterior::actualizarParedBatalla);

    // Llama una vez al inicio por si entras con estado previo:
    actualizarParedBatalla();

}

void EscenaInterior::setSprite(isSprite *chica)
{
    Chica = chica;
    if (Chica) {
        sceneDentro->addItem(Chica);
        Chica->setZValue(2);
        Chica->setPos(100, 440);
    }

}

void EscenaInterior::keyPressEvent(QKeyEvent *event)
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

void EscenaInterior::keyReleaseEvent(QKeyEvent *event)
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
        Chica->quieto();
    }
}


void EscenaInterior::cambiarFondo(int seccion)
{
    QString ruta;

    switch (seccion) {
    case 0: ruta = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/historiaAbierta.png"; break;
    case 1: ruta = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/politicaAbierta.png"; break;
    case 2: ruta = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/cienciaAbierta.png"; break;
    case 3: ruta = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/arteAbierta.png"; break;
    default: ruta = "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/puertas.png"; break;
    }


    QPixmap nuevoFondo(ruta);
    if (!nuevoFondo.isNull()) {
        if (!fondoItemDentro) {
            fondoItemDentro = sceneDentro->addPixmap(nuevoFondo);
            fondoItemDentro->setZValue(0);
        } else {
            fondoItemDentro->setPixmap(
                nuevoFondo.scaled(ui->graphicsViewDentro->size(),
                                  Qt::KeepAspectRatioByExpanding,
                                  Qt::SmoothTransformation));
        }
    }
}

void EscenaInterior::actualizarFondo(const QString &ruta)
{
    if (!fondoItemDentro) return;

    // Detectar tipo por nombre
    if (ruta.contains("historia")) fondoActual = "historia";
    else if (ruta.contains("arte")) fondoActual = "arte";
    else if (ruta.contains("ciencia")) fondoActual = "ciencia";
    else if (ruta.contains("politica")) fondoActual = "politica";
    else fondoActual.clear(); // ninguno abierto

    QPixmap nuevoFondo(ruta);
    if (!nuevoFondo.isNull()) {
        fondoItemDentro->setPixmap(
            nuevoFondo.scaled(ui->graphicsViewDentro->size(),
                              Qt::KeepAspectRatioByExpanding,
                              Qt::SmoothTransformation)
            );
    }
}






void EscenaInterior::animarMovimiento()
{
    if (!Chica) return;

    // Mueve el sprite


     const QPointF oldPos = Chica->pos();

     Chica->mover(dx, dy);

    // Verificar colisión con puertas
    QList<QGraphicsItem*> colisiones = Chica->collidingItems();
    for (QGraphicsItem *item : colisiones) {
        QString tipo = item->data(0).toString();

        // // Verificar si es una puerta abierta
        // if (tipo == "historia" && fondoActual == "historia") {
        //     abrirEscenaHistoria();
        //     return;
        // } else if (tipo == "arte" && fondoActual == "arte") {
        //     abrirEscenaArte();
        //     return;
        // } else if (tipo == "ciencia" && fondoActual == "ciencia") {
        //     abrirEscenaCiencia();
        //     return;
        // } else if (tipo == "politica" && fondoActual == "politica") {
        //     abrirEscenaPolitica();
        //     return;
        // }

        // Verifica que la puerta esté "abierta" en el fondo actual

        // --- PORTAL FINAL ---
        if (tipo == "portalBatalla") {
            Chica->setPos(oldPos);
            animTimer->stop();
            dx = dy = 0;
            Chica->quieto();

            if (!abriendoBatalla) {
                abriendoBatalla = true;
                abrirEscenaBatalla();
            }
            return;
        }

        // --- PUERTAS ---
        if (tipo == "historia" || tipo == "arte" || tipo == "ciencia" || tipo == "politica") {

            // a) Puerta no abierta por la ruleta -> actúa como pared (no mensaje)
            if (tipo != fondoActual) {
                // Revertir y frenar
                Chica->setPos(oldPos);
                animTimer->stop();
                dx = dy = 0;
                Chica->quieto();
                return;
            }

            // b) Puerta abierta pero ya jugada -> bloquear + mensaje 1 sola vez
            auto p = clavePuerta(tipo); // tu helper que mapea a GameState::Puerta
            if (GS.jugado(p)) {
                // Revertir y frenar
                Chica->setPos(oldPos);
                animTimer->stop();
                dx = dy = 0;
                Chica->quieto();

                if (!mostrandoAvisoPuerta) {
                    mostrandoAvisoPuerta = true;
                    QMessageBox::information(this, "Ya intentado",
                                             "Esta puerta ya fue jugada. No se puede volver a jugar.");
                    // pequeño cooldown para no repetir si el usuario mantiene la tecla
                    QTimer::singleShot(700, this, [this](){ mostrandoAvisoPuerta = false; });
                }
                return;
            }


        // 2) Abrir escena correspondiente
        if (p == Gamestate::Historia) { abrirEscenaHistoria(); return; }
        if (p == Gamestate::Arte)     { abrirEscenaArte();     return; }
        if (p == Gamestate::Ciencia)  { abrirEscenaCiencia();  return; }
        if (p == Gamestate::Politica) { abrirEscenaPolitica(); return; }
        }
    }
}

void EscenaInterior::abrirEscenaHistoria()
{
    if (Chica) {
        Chica->quieto();
    }
    dx = dy = 0;
    animTimer->stop();

    // Limpiar estado de teclado
    QCoreApplication::processEvents();
    EscenaHistoria *ventana = new EscenaHistoria(this);
    ventana->setEscenaAnterior(this);
    ventana->setSprite(Chica);
    ventana->show();
    this->hide();
}

void EscenaInterior::abrirEscenaCiencia()
{
    if (Chica) {
        Chica->quieto();
    }
    dx = dy = 0;
    animTimer->stop();

    // Limpiar estado de teclado
    QCoreApplication::processEvents();
    EscenaCiencia *ventana = new EscenaCiencia(this);
    ventana->setEscenaAnterior(this); ////////NO RECUEDO PORQUE ESTA AQUI
    ventana->setSprite(Chica);
    ventana->show();
    this->hide();
}

void EscenaInterior::abrirEscenaPolitica()
{
    if (Chica) {
        Chica->quieto();
    }
    dx = dy = 0;
    animTimer->stop();

    // Limpiar estado de teclado
    QCoreApplication::processEvents();
    EscenaPolitica *ventana = new EscenaPolitica(this);
    ventana->setEscenaAnterior(this); ////////NO RECUEDO PORQUE ESTA AQUI
    ventana->setSprite(Chica);
    ventana->show();
    this->hide();
}

void EscenaInterior::abrirEscenaArte()
{
    if (Chica) {
        Chica->quieto();
    }
    dx = dy = 0;
    animTimer->stop();

    // Limpiar estado de teclado
    QCoreApplication::processEvents();
    EscenaArte *ventana = new EscenaArte(this);
    ventana->setEscenaAnterior(this); ////////NO RECUEDO PORQUE ESTA AQUI
    ventana->setSprite(Chica);
    ventana->show();
    this->hide();
}

// EscenaInterior.cpp
void EscenaInterior::abrirEscenaBatalla()
{
    if (Chica) {
        Chica->quieto();
        dx = dy = 0;
        animTimer->stop();

        if (auto *sc = Chica->scene())
            sc->removeItem(Chica);

        Chica->setParentItem(nullptr); // importante si usaste parentItem antes
    }

    QCoreApplication::processEvents();

    auto* ventana = new Batalla(this);
    ventana->setAttribute(Qt::WA_DeleteOnClose, true);
    ventana->setSprite(Chica);   // aquí ya se encarga Batalla de pedir elección y crear rival
    ventana->show();
    this->hide();
}



void EscenaInterior::agregarSpriteDesdeHistoria(isSprite* chica)
{
    if (!chica) return;

    Chica = chica;
    if (!sceneDentro->items().contains(Chica))
        sceneDentro->addItem(Chica);

    Chica->setZValue(2);
    Chica->setPos(100, 440);  // posición base al volver
    Chica->quieto();

    setFocus();  // recuperar el control de teclado
}



//CORAZONSSSSSS

void EscenaInterior::crearHUDCorazones() {
    heartsHUD = new QLabel(ui->graphicsViewDentro->viewport());
    heartsHUD->setAttribute(Qt::WA_TranslucentBackground);
    heartsHUD->setStyleSheet("background: red;");
    heartsHUD->setScaledContents(true);
    heartsHUD->resize(140, 130); // tamaño del sprite; ajusta a gusto

    // Colocarlo arriba-derecha con margen
    const int margen = 12;
    auto place = [this, margen](){
        QPoint p(ui->graphicsViewDentro->viewport()->width() - heartsHUD->width() - margen,
                 margen);
        heartsHUD->move(p);
        heartsHUD->raise();
    };
    place();


    // Mejor: instalar un eventFilter para reubicar en resize
    ui->graphicsViewDentro->viewport()->installEventFilter(this);
}

bool EscenaInterior::eventFilter(QObject* obj, QEvent* ev) {
    if (obj == ui->graphicsViewDentro->viewport() && ev->type() == QEvent::Resize && heartsHUD) {
        const int margen = 12;
        QPoint p(ui->graphicsViewDentro->viewport()->width() - heartsHUD->width() - margen,
                 margen);
        heartsHUD->move(p);
        heartsHUD->raise();
    }
    return QDialog::eventFilter(obj, ev);
}

void EscenaInterior::actualizarHUDCorazones(int vidas) {
    // RUTAS de tus sprites de corazones:
    static const QStringList rutas = {
        "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/corazones0.png",
        "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/corazones1.png",
        "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/corazones2.png",
        "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/corazones3.png",
        "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/corazones4.png"
    };
    vidas = std::clamp(vidas, 0, 4);
    QPixmap px(rutas[vidas]);
    if (!px.isNull()) heartsHUD->setPixmap(px);
}

///COLISION BATALLA
void EscenaInterior::actualizarParedBatalla()
{
    const bool condicionActiva =
        GS.todosJugados() && (GS.totalGanados() >= 1);

    if (condicionActiva) {
        if (!paredBatalla) {
            QRectF r(95, 435, 40, 40); // (x,y,w,h)
            paredBatalla = sceneDentro->addRect(
                r, QPen(Qt::NoPen), QBrush(Qt::NoBrush)); // invisible

            // paredFinal->setBrush(QColor(0,255,0,80));
            paredBatalla->setZValue(1);
            paredBatalla->setData(0, "portalBatalla");
        }
    } else {
        if (paredBatalla) {
            sceneDentro->removeItem(paredBatalla);
            delete paredBatalla;
            paredBatalla = nullptr;
        }
    }
}







EscenaInterior::~EscenaInterior()
{
    delete ui;
}



void EscenaInterior::on_pushButton_clicked()
{
    Mapa *ventanaMapa = new Mapa(this);
    ventanaMapa->setSprite(Chica);

    // Mostrasr la ventanas
    ventanaMapa->setWindowTitle("Mapa");
    ventanaMapa->show();

    // Opcional: ocultar la principal
    //this->hide();
}


void EscenaInterior::on_giro_clicked()
{

}

QGraphicsView* EscenaInterior::getGraphicsView() {
    return ui->graphicsViewDentro;
}


