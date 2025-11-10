#include "mapa.h"
#include "ui_mapa.h"

Mapa::Mapa(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Mapa)
    , sceneMapa(nullptr)
    , Chica(nullptr)
    , dx(0)
    , dy(0)
{
    ui->setupUi(this);

    sceneMapa = new QGraphicsScene(this);
    ui->graphicsViewMapa->setScene(sceneMapa);
    ui->graphicsViewMapa->setRenderHint(QPainter::Antialiasing);
    ui->graphicsViewMapa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsViewMapa->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QPixmap fondo("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/mapa.png");
    QGraphicsPixmapItem *fondoItem = sceneMapa->addPixmap(fondo.scaled(ui->graphicsViewMapa->size(),

                                                                       Qt::KeepAspectRatioByExpanding,
                                                                   Qt::SmoothTransformation));


    fondoItem->setZValue(0);

    // --- Temporizador para animación ---
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &Mapa::animarMovimiento);

    // --- Asegurar que reciba eventos de teclado ---
    setFocusPolicy(Qt::StrongFocus);
    ui->graphicsViewMapa->setFocusPolicy(Qt::NoFocus);
    this->setFocus();


}

void Mapa::setSprite(isSprite *chica)
{
    Chica = chica;
    if (Chica) {
        sceneMapa->addItem(Chica);
        Chica->setZValue(2);
        Chica->setPos(100, 440);
    }

}

void Mapa::keyPressEvent(QKeyEvent *event)
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

void Mapa::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;
    dx = dy = 0;
    animTimer->stop();
    if (Chica) Chica->quieto();
}

void Mapa::animarMovimiento()
{
    if (!Chica) return;

    // Mueve el sprite
    Chica->mover(dx, dy);
}

Mapa::~Mapa()
{
    delete ui;
}
