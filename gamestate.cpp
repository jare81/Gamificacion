#include "gamestate.h"

#include <algorithm>


Gamestate& Gamestate::instance() {
    static Gamestate s;
    return s;
}


Gamestate::Gamestate(QObject* parent) : QObject(parent) {}

void Gamestate::setVidas(int v) {
    v = std::clamp(v, 0, 4);
    if (m_vidas != v) {
        m_vidas = v;
        emit vidasCambiaron(m_vidas);
    }
}

void Gamestate::addVida(int delta) {
    setVidas(m_vidas + delta);
}

void Gamestate::registrarResultado(Puerta p, bool gano) {
    // Si ya se jugó esta puerta, no vuelvas a contarla
    if (!m_jugados.contains(p)) {
        m_jugados.insert(p);
        if (gano) {
            m_ganados.insert(p);
            addVida(+1); // solo si ganó
        }
        emit puertasActualizadas();

        // Regla de “reinicio de ruleta si perdió todo”
        if (todosJugados() && m_ganados.isEmpty()) {
            // Resetea INTENTOS (no vidas)
            m_jugados.clear();
            m_ganados.clear();
            emit puertasActualizadas();
            emit ruletaReset(); // le avisa a la EscenaInterior para permitir otro intento
        }
    }
}


