#include "spriter.h"

spriteR::spriteR(QObject *parent)
    : QObject(parent), isSprite()
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &spriteR::actualizacion);

    // Carga de hojas (ajusta rutas)
    pixmapIdle  = new QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/normalR.png");
    pixmapLeft  = new QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/caminarRI.png"); // izquierda
    pixmapRight = new QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/caminarRD.png"); // derecha
    pixmapAtk   = new QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/ataqueR.png");
    pixmapHurt  = new QPixmap("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/dolorR.png");

    // Calcula frames por hoja (evita 0 si falla la carga)
    framesIdle  = (pixmapIdle  && !pixmapIdle->isNull())  ? pixmapIdle->width()  / wIdle  : 0;
    framesLeft  = (pixmapLeft  && !pixmapLeft->isNull())  ? pixmapLeft->width()  / wWalk  : 0;
    framesRight = (pixmapRight && !pixmapRight->isNull()) ? pixmapRight->width() / wWalk  : 0;
    framesAtk   = (pixmapAtk   && !pixmapAtk->isNull())   ? pixmapAtk->width()   / wAtk   : 0;
    framesHurt  = (pixmapHurt  && !pixmapHurt->isNull())  ? pixmapHurt->width()  / wHurt  : 0;

    // Estado inicial
    setEstado(Estado::Idle);
    timer->start(90); // anima idle continuamente
}

QRectF spriteR::boundingRect() const
{
    // Calcula una vez (puedes dejarlo con const estáticos si los w/h son constantes)
        const int maxW = std::max(std::max(wIdle, wWalk), std::max(wAtk,  wHurt));
    const int maxH = std::max(std::max(hIdle, hWalk), std::max(hAtk,  hHurt));
    return QRectF(-maxW/2, -maxH/2, maxW, maxH);
}

void spriteR::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!pixmapActual || pixmapActual->isNull()) return;

    const int w = frameWidthActual();
    const int h = frameHeightActual();
    p->drawPixmap(-w/2, -h/2, *pixmapActual, columnas, 0, w, h);
}

void spriteR::setEstado(Estado e)
{
    if (estado == Estado::Dead) return; // nada sobre dead
    // Prioridades: Hurt > Attack > Walk > Idle.
    // Si ya estás en Hurt/Attack y piden caminar, ignora hasta terminar el ciclo.
    if ((estado == Estado::Hurt || estado == Estado::Attack) &&
        (e == Estado::WalkLeft || e == Estado::WalkRight || e == Estado::Idle)) {
        return;
    }

    const QSizeF oldSize = sizeFor(estado);
    const QSizeF newSize = sizeFor(e);
    if (oldSize != newSize) {
        prepareGeometryChange(); // <-- ¡la clave!
    }

    estado = e;
    seleccionarPixmapPorEstado();
    columnas = 0;

    ajustarTickPorEstado();
    if (!timer->isActive()) timer->start(90);
    update();
}

QSizeF spriteR::sizeFor(Estado e) const {
    switch (e) {
    case Estado::Attack: return QSizeF(wAtk,  hAtk);
    case Estado::Hurt:   return QSizeF(wHurt, hHurt);
    case Estado::WalkLeft:
    case Estado::WalkRight: return QSizeF(wWalk, hWalk);
    case Estado::Idle:
    default: return QSizeF(wIdle, hIdle);
    }
}
void spriteR::ajustarTickPorEstado() {
    if (!timer) return;

    int ms = 100; // base
    switch (estado) {
    case Estado::Idle:      ms = 140; break;  // más lento
    case Estado::WalkLeft:
    case Estado::WalkRight: ms = 110; break;  // caminata fluida
    case Estado::Attack:    ms = 150; break;  // que se vea el swing
    case Estado::Hurt:      ms = 160; break;  // que se note el impacto
    case Estado::Dead:      ms = 180; break;
    }

    if (timer->interval() != ms) timer->start(ms);
}


void spriteR::seleccionarPixmapPorEstado()
{
    switch (estado) {
    case Estado::Idle:      pixmapActual = pixmapIdle;  break;
    case Estado::WalkLeft:  pixmapActual = pixmapLeft;  break;
    case Estado::WalkRight: pixmapActual = pixmapRight; break;
    case Estado::Attack:    pixmapActual = pixmapAtk;   break;
    case Estado::Hurt:      pixmapActual = pixmapHurt;  break;
    case Estado::Dead:      pixmapActual = pixmapIdle;  break; // o una hoja de “muerto”
    }
}

int spriteR::frameWidthActual() const
{
    switch (estado) {
    case Estado::Attack: return wAtk;
    case Estado::Hurt:   return wHurt;
    case Estado::WalkLeft:
    case Estado::WalkRight: return wWalk;
    case Estado::Idle:
    default: return wIdle;
    }
}

int spriteR::frameHeightActual() const
{
    switch (estado) {
    case Estado::Attack: return hAtk;
    case Estado::Hurt:   return hHurt;
    case Estado::WalkLeft:
    case Estado::WalkRight: return hWalk;
    case Estado::Idle:
    default: return hIdle;
    }
}

int spriteR::framesTotalesActual() const
{
    switch (estado) {
    case Estado::Attack: return framesAtk;
    case Estado::Hurt:   return framesHurt;
    case Estado::WalkLeft:  return framesLeft;
    case Estado::WalkRight: return framesRight;
    case Estado::Idle:
    default: return framesIdle;
    }
}

void spriteR::actualizacion()
{
    // Avanza columnas por frame
    const int w = frameWidthActual();
    const int total = framesTotalesActual();
    if (total <= 0 || !pixmapActual) return;

    columnas += w;

    if (columnas >= total * w) {
        // Qué hacer al final de la hoja
        if (estado == Estado::Attack || estado == Estado::Hurt) {
            // Terminan y regresan a Idle
            setEstado(Estado::Idle);
        } else {
            // Idle/Walk: loop
            columnas = 0;
        }
    }

    update();
}

void spriteR::mover(int dx, int dy)
{
    if (estado == Estado::Dead || estado == Estado::Hurt || estado == Estado::Attack)
        return;

    if (dx == 0 && dy == 0) {
        setEstado(Estado::Idle);
        return;
    }

    // Mover posición
    setPos(x() + dx, y() + dy);

    // Elegir estado de caminar (prioriza dirección horizontal)
    if (dx > 0)      setEstado(Estado::WalkRight);
    else if (dx < 0) setEstado(Estado::WalkLeft);
    else {
        // Si solo hay movimiento vertical, decide “mirada” según último lado que prefieras
        setEstado(Estado::WalkLeft); // o WalkRight si te conviene
    }
}

void spriteR::atacar()
{
    if (estado == Estado::Dead || estado == Estado::Attack || framesAtk <= 0) return;
    setEstado(Estado::Attack);
}

void spriteR::dolor()
{
    if (estado == Estado::Dead || estado == Estado::Hurt || framesHurt <= 0) return;
    setEstado(Estado::Hurt);
}

void spriteR::quieto()
{
    if (estado == Estado::Dead) return;
    setEstado(Estado::Idle);
}

void spriteR::morir()
{
    estado = Estado::Dead;
    timer->stop();
    update();
}

bool spriteR::estaEnZona(const QString &nombreZona) const {
    Q_UNUSED(nombreZona);
    return false; // o puedes copiar la lógica de colisiones si te sirve
}


// en isSprite (o en spriteR/V si prefieres)
void spriteR::quietoForce()
{
    estado = Estado::Idle;          // salta bloqueo
    seleccionarPixmapPorEstado();
    columnas = 0;
    ajustarTickPorEstado();
    update();
}



