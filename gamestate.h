#ifndef GAMESTATE_H
#define GAMESTATE_H

#pragma once
#include <QObject>
#include <QSet>

class Gamestate  : public QObject
{
       Q_OBJECT
public:


     enum Puerta { Historia=0, Ciencia=1, Politica=2, Arte=3 };

      static Gamestate& instance(); // singleton

     // Vidas (0..4)
     int vidas() const { return m_vidas; }
     void setVidas(int v);
     void addVida(int delta = 1);   // clamp 0..4

     // Progreso de mini-juegos
     bool jugado(Puerta p) const { return m_jugados.contains(p); }
     bool ganado(Puerta p) const { return m_ganados.contains(p); }
     void registrarResultado(Puerta p, bool gano); // marca jugado y, si gano, suma vida

     // Resumen
     int totalGanados()   const { return m_ganados.size(); }
     bool todosJugados()  const { return m_jugados.size() == 4; }

 signals:
     void vidasCambiaron(int vidas);
     void puertasActualizadas();
     void ruletaReset(); // cuando se jugaron las 4 y no se ganó nada

 private:
     explicit Gamestate(QObject* parent=nullptr);
     int m_vidas = 0;           // empieza en 0 (ajústalo si querés)
     QSet<Puerta> m_jugados;
     QSet<Puerta> m_ganados;
};

#endif // GAMESTATE_H
