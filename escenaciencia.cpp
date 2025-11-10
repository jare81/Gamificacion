#include "escenaciencia.h"
#include "ui_escenaciencia.h"
#include <QMessageBox>



EscenaCiencia::EscenaCiencia(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EscenaCiencia)
    , sceneCiencia(nullptr)
    , Chica(nullptr)
    , dx(0)
    , dy(0)
{
    ui->setupUi(this);

    // Crear escena
    sceneCiencia = new QGraphicsScene(this);
    ui->graphicsViewCiencia->setScene(sceneCiencia);
    ui->graphicsViewCiencia->setRenderHint(QPainter::Antialiasing);
    ui->graphicsViewCiencia->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsViewCiencia->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    // //LABELS TIMBAS
    QLabel* label1 = new QLabel;
    label1->setPixmap(QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/tNormal.png").scaled(140, 140));
    QLabel* label2 = new QLabel;
    label2->setPixmap(QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/tNormal.png").scaled(140, 140));
    QLabel* label3 = new QLabel;
    label3->setPixmap(QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/tNormal.png").scaled(140, 140));
    QLabel* label4 = new QLabel;
    label4->setPixmap(QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/tNormal.png").scaled(140, 140));

    // --- 2️⃣ Agregar labels a la escena usando QGraphicsProxyWidget ---
    QGraphicsProxyWidget* proxy1 = sceneCiencia->addWidget(label1);
    proxy1->setPos(210, 330);
    proxy1->setData(1, "tumba1");
    proxy1->setZValue(1); // detrás del sprite

    QGraphicsProxyWidget* proxy2 = sceneCiencia->addWidget(label2);
    proxy2->setPos(360, 330);
    proxy2->setData(1, "tumba2");
    proxy2->setZValue(1);

    QGraphicsProxyWidget* proxy3 = sceneCiencia->addWidget(label3);
    proxy3->setPos(500, 330);
    proxy3->setData(1, "tumba3");
    proxy3->setZValue(1);

    QGraphicsProxyWidget* proxy4 = sceneCiencia->addWidget(label4);
    proxy4->setPos(640, 330);
    proxy4->setData(1, "tumba4");
    proxy4->setZValue(1);

    for (QLabel* lbl : {label1, label2, label3, label4}) {
        lbl->setAttribute(Qt::WA_TranslucentBackground);
        lbl->setStyleSheet("background: transparent;");
    }

    // Fondo nuevo
    QPixmap fondoDentro("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/backCiencia.jpg");
    fondoItemDentro = sceneCiencia->addPixmap(
        fondoDentro.scaled(ui->graphicsViewCiencia->size(),
                           Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation));
    fondoItemDentro->setZValue(0);

    // --- Temporizador para animación ---
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &EscenaCiencia::animarMovimiento);

    // --- Asegurar que reciba eventos de teclado ---
    setFocusPolicy(Qt::StrongFocus);
    ui->graphicsViewCiencia->setFocusPolicy(Qt::NoFocus);
    this->setFocus();


    //PREGUNTAS
    preguntas.cargarPreguntasCiencia();
    totalPreguntas = 5;
    numeroPreguntaActual = 0;
    respuestasCorrectas = 0;


    //AQUI NO USO LOS BOTONES SINO COLISIONES
    // Conectar botones a la función de verificación
    // connect(ui->bnOpcionA, &QPushButton::clicked, this, &EscenaCiencia::verificarRespuesta);
    // connect(ui->bnOpcionB, &QPushButton::clicked, this, &EscenaCiencia::verificarRespuesta);
    // connect(ui->bnOpcionC, &QPushButton::clicked, this, &EscenaCiencia::verificarRespuesta);
    // connect(ui->bnOpcionD, &QPushButton::clicked, this, &EscenaCiencia::verificarRespuesta);

    siguientePregunta(); // Mostrar la primera



    // CREAR COLISIONES PARA TUMBA
    //El método addRect(x (posición horizontal (desde la izquierda)), y(posición vertical (desde arriba)), ancho, alto, ...)
    QGraphicsRectItem *pared1 = sceneCiencia->addRect(230, 330, 85, 10, QPen(Qt::NoPen), QBrush(Qt::red));
    pared1->setData(1, "pared1");

    QGraphicsRectItem *pared2 = sceneCiencia->addRect(380, 330, 85, 10, QPen(Qt::NoPen), QBrush(Qt::yellow));
    pared2->setData(1, "pared2");

    QGraphicsRectItem *pared3 = sceneCiencia->addRect(520, 330, 85, 10, QPen(Qt::NoPen), QBrush(Qt::gray));
    pared3->setData(1, "pared3");

    QGraphicsRectItem *pared4 = sceneCiencia->addRect(660, 330, 85, 10, QPen(Qt::NoPen), QBrush(Qt::blue));
    pared4->setData(1, "pared4");

    tumbas << proxy1 << proxy2 << proxy3 << proxy4;


}


void EscenaCiencia::setSprite(isSprite *chica)
{
    Chica = chica;
    if (Chica) {
        sceneCiencia->addItem(Chica);
        Chica->setZValue(2);

        Chica->setPos(120, 470);
    }

}

void EscenaCiencia::keyPressEvent(QKeyEvent *event)
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







void EscenaCiencia::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;
    dx = dy = 0;
    animTimer->stop();
    if (Chica) Chica->quieto();
}

void EscenaCiencia::animarMovimiento()
{
    if (!Chica) return;

    Chica->mover(dx, dy);

    QList<QGraphicsItem*> colisiones = Chica->collidingItems();

    for (auto* item : colisiones) {
        QVariant tipo = item->data(1);
        if (tipo.isValid() && tipo.toString().startsWith("pared")) {
            colisionPared(item);
            break;
        }
    }
}


void EscenaCiencia::colisionPared(QGraphicsItem* pared)
{
    QString tipo = pared->data(1).toString();
    char respuesta = ' ';

    if (tipo == "pared1") respuesta = 'A';
    else if (tipo == "pared2") respuesta = 'B';
    else if (tipo == "pared3") respuesta = 'C';
    else if (tipo == "pared4") respuesta = 'D';

    bool esCorrecta = (preguntaActual.respuestaCorrecta == respuesta);

    // Cambiar imagen de la tumba correspondiente
    int indice = respuesta - 'A'; // A→0, B→1, C→2, D→3
    if (indice >= 0 && indice < tumbas.size()) {
        QLabel* lbl = qobject_cast<QLabel*>(tumbas[indice]->widget());
        if (lbl) {
            QString nuevaImg = esCorrecta
                                   ? "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/tCorrecta.png"
                                   : "C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/tIncorrecta.png";

            lbl->setPixmap(QPixmap(nuevaImg).scaled(140, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    // Mostrar feedback
    if (esCorrecta) {
        respuestasCorrectas++;
        QMessageBox::information(this, "Correcto", "Respuesta correcta ");
    } else {
        QMessageBox::warning(this, "Incorrecto", "Respuesta incorrecta ");
    }

    animTimer->stop();
    dx = 0;
    dy = 0;
    if (Chica) Chica->quieto();

    // --- Reiniciar posición del jugador de forma segura ---
    Chica->setFlag(QGraphicsItem::ItemIsFocusable, false);
    Chica->setPos(120, 470);
    QTimer::singleShot(100, this, [this]() {
        Chica->setFlag(QGraphicsItem::ItemIsFocusable, true);
        Chica->setFocus();
    });

    // --- Avanzar pregunta y restaurar tumbas ---
    QTimer::singleShot(900, this, [this]() {
        siguientePregunta();  // Solo una vez

        // Restaurar todas las tumbas a su imagen normal
        for (auto* proxy : tumbas) {
            QLabel* lbl = qobject_cast<QLabel*>(proxy->widget());
            if (lbl) {
                lbl->setPixmap(QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/tNormal.png")
                                   .scaled(140, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
    });
}






// Mostrar una pregunta en el UI
// =============================
void EscenaCiencia::mostrarPregunta(const Pregunta &pregunta) {
    ui->lbPregunta->setText(pregunta.texto);
    ui->bnOpcionA->setText(pregunta.respuestaA);
    ui->bnOpcionB->setText(pregunta.respuestaB);
    ui->bnOpcionC->setText(pregunta.respuestaC);
    ui->bnOpcionD->setText(pregunta.respuestaD);
}

// Cargar la siguiente pregunta
// =============================
void EscenaCiencia::siguientePregunta() {
    numeroPreguntaActual++;

    // Si ya no hay más preguntas, evaluar resultado final
    if (numeroPreguntaActual > totalPreguntas) {

        const bool gano = (respuestasCorrectas == totalPreguntas);

        Gamestate::instance().registrarResultado(Gamestate::Ciencia, gano);

        // Mostrar resumen
        QString mensaje = QString("Has contestado %1 de %2 correctamente.")
                              .arg(respuestasCorrectas)
                              .arg(totalPreguntas);

        if(!gano){
            QMessageBox::warning(this, "Fin del juego", mensaje + "\nPerdiste el reto de ciencia... ");

            // Ejecuta animación de muerte, pero no la dejes en ese estado permanentemente
            if (Chica) {
                Chica->morir();
            }

            regresarEscenaInterior();


        }else{
            QMessageBox::information(this, "¡Excelente!", mensaje + "\n¡Has sobrevivido al reto de arte! ");

            regresarEscenaInterior();
        }


        return; // salir para no intentar cargar más preguntas
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


//ARREGLAR NO DEBE IR POR TIPO DE BOTON DEBE IR POR
//COLISION CON PAREDES DE LAS TUMBAS
// Verificar la respuesta elegida VERIFICAR NO
// =============================
void EscenaCiencia::verificarRespuesta() {

    QPushButton *boton = qobject_cast<QPushButton*>(sender());
    if (!boton) return;

    char respuesta = ' ';
    if (boton == ui->bnOpcionA) respuesta = 'A';
    else if (boton == ui->bnOpcionB) respuesta = 'B';
    else if (boton == ui->bnOpcionC) respuesta = 'C';
    else if (boton == ui->bnOpcionD) respuesta = 'D';

    if (preguntaActual.respuestaCorrecta == respuesta) {
        respuestasCorrectas++;
        QMessageBox::information(this, "Correcto", "Tumba correcta");
    } else {
        QMessageBox::warning(this, "Incorrecto", "Tumba incorrecta");
    }

    siguientePregunta();
}

void EscenaCiencia::setEscenaAnterior(QWidget *escena)
{
    escenaAnterior = escena;
}

void EscenaCiencia::regresarEscenaInterior()
{
    // Detener animación
    animTimer->stop();
    dx = dy = 0;
    if (Chica) Chica->quieto();

    //  la escena anterior
    if (escenaAnterior) {
        EscenaInterior *interior = qobject_cast<EscenaInterior*>(escenaAnterior);
        if (interior && Chica) {
            sceneCiencia->removeItem(Chica);       // quitarlo de esta escena
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





EscenaCiencia::~EscenaCiencia()
{
    delete ui;
}
