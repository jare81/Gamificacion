#ifndef CHOOSEAPECTDIALOG_H
#define CHOOSEAPECTDIALOG_H

#pragma once
#include <QDialog>
#include <QPixmap>

namespace Ui {
class ChooseApectDialog;
}

class ChooseApectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseApectDialog(QWidget *parent = nullptr);
    ~ChooseApectDialog();
    enum Eleccion { Empirista, Racionalista, Ninguno };

    Eleccion eleccion() const { return elegido; }

private slots:
    void onEmpirista();
    void onRacionalista();

private:
    Ui::ChooseApectDialog *ui;
    Eleccion elegido = Ninguno;
};

#endif // CHOOSEAPECTDIALOG_H
