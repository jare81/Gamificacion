#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "escenainterior.h"
#include "mapa.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(3);

    ui->graphicsView->setFocusPolicy(Qt::StrongFocus);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_KeyCompression, false);
    ui->graphicsView->setFocus();

    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();


    // Crear la escena
    scene = new QGraphicsScene(this);
    sceneDentro = nullptr;

    // Ajustar tamaño de escena según graphicsView
    scene->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    QPixmap fondo("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/mus.jpg");
    QGraphicsPixmapItem *fondoItem = scene->addPixmap(fondo.scaled(ui->graphicsView->size(),
                                                                   Qt::KeepAspectRatioByExpanding,
                                                                   Qt::SmoothTransformation));

     fondoItem->setZValue(0);

    // Crear el sprite
    Chica = new sprite();
    scene->addItem(Chica);
    Chica->setZValue(2);

    // Aparece el sprite Esquina inferior izquierda
    Chica->setPos(40,460);


    dx = dy = 0;
    enZonaSalida = false;
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &MainWindow::animarMovimiento);


    ui->graphicsView->setFocus();



    // Crear paredes
    //El método addRect(x (posición horizontal (desde la izquierda)), y(posición vertical (desde arriba)), ancho, alto, ...)
    QGraphicsRectItem *pared1 = scene->addRect(440, 175, 150, 140, QPen(Qt::NoPen), QBrush(Qt::NoBrush));
    pared1->setData(0, "pared");

    QGraphicsRectItem *pared2 = scene->addRect(0, 170, 425, 200, QPen(Qt::NoPen), QBrush(Qt::NoBrush));
    pared2->setData(0, "pared");

    QGraphicsRectItem *pared3 = scene->addRect(615, 170, 410, 200, QPen(Qt::NoPen), QBrush(Qt::NoBrush));
    pared3->setData(0, "pared");


    QGraphicsRectItem *zonaSalida = scene->addRect(450, 320, 140, 160, QPen(Qt::NoPen), QBrush(Qt::NoBrush));
    zonaSalida->setData(0, "zonaSalida");
    zonaSalida->setZValue(1);

    this->setFocus();

    ui->labelMensaje->setVisible(false);


    // // --- Configurar escena del graphicsViewDentro ---
    // QGraphicsScene *sceneRuleta = new QGraphicsScene(this);
    // ui->graphicsViewDentro->setScene(sceneRuleta);
    // ui->graphicsViewDentro->setRenderHint(QPainter::Antialiasing);
    // ui->graphicsViewDentro->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // ui->graphicsViewDentro->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // // --- Crear la ruleta y agregarla a la escena ---
    // connect(ui->giro, &QPushButton::clicked, this, [this]() {
    //     Ruleta *ruleta = new Ruleta();  // sin padre → ventana separada
    //     ruleta->setWindowTitle("Ruleta de Temas");
    //     ruleta->setFixedSize(250, 250); // tamaño de la ventana
    //     ruleta->setWindowModality(Qt::ApplicationModal); // bloquea la ventana principal mientras esté abierta
    //     ruleta->show();

    //     ruleta->iniciarGiro();
    // });
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

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
    case Qt::Key_E:
        if (enZonaSalida) {

            // ui->stackedWidget->setCurrentIndex(1);
            // ui->stackedWidget->repaint();

            // // Crear escena
            // if (!sceneDentro) {
            //     sceneDentro = new QGraphicsScene(this);
            //     ui->graphicsViewDentro->setScene(sceneDentro);
            //     sceneDentro->setSceneRect(0, 0,
            //                               ui->graphicsViewDentro->width(),
            //                               ui->graphicsViewDentro->height());

            //     // Fondo nuevo
            //     QPixmap fondoDentro("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/dentro3.png");
            //     QGraphicsPixmapItem *fondoItemDentro =
            //         sceneDentro->addPixmap(fondoDentro.scaled(
            //             ui->graphicsViewDentro->size(),
            //             Qt::KeepAspectRatioByExpanding,
            //             Qt::SmoothTransformation));
            //     fondoItemDentro->setZValue(0);
            // }

            // // Mover sprite a la nueva escena
            // scene->removeItem(Chica);
            // sceneDentro->addItem(Chica);
            // Chica->setZValue(2);

            // // Aparece el sprite Esquina inferior izquierda
            // Chica->setPos(100,440);

            if (Chica) {
                Chica->quieto();
            }
            dx = dy = 0;
            animTimer->stop();

            // Limpiar estado de teclado (por si hay key repeat activo)
            QCoreApplication::processEvents();
            EscenaInterior *ventanaDentro = new EscenaInterior(this);

            // Pasar el sprite actual
            scene->removeItem(Chica);
            ventanaDentro->setSprite(Chica);

            // Mostrasr la ventana
            ventanaDentro->setWindowTitle("Escena Interior");
            ventanaDentro->show();

            // Opcional: ocultar la principal
            this->hide();

        }
        break;

    default:
        QMainWindow::keyPressEvent(event);
        return;
    }

    if (!animTimer->isActive())
        animTimer->start(90); // Cambia el frame cada 100 ms
}



void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    dx = dy = 0;
    animTimer->stop();
    Chica->quieto();
}


void MainWindow::animarMovimiento()
{
    if (dx != 0 || dy != 0)
        Chica->mover(dx, dy);


    if (Chica->estaEnZona("zonaSalida")) {
        ui->labelMensaje->setVisible(true);
        ui->labelMensaje->setText("Presiona E para entrar");
        //qDebug() << "Colisión con zonaSalida:" << Chica->estaEnZona("zonaSalida");

        enZonaSalida = true;
    } else {
         ui->labelMensaje->setVisible(false);
        ui->labelMensaje->clear();
        enZonaSalida = false;
    }

}




void MainWindow::on_bnNuevaPartida_clicked()
{
     ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_clicked()
{




}


void MainWindow::on_giro_2_clicked()
{


}

