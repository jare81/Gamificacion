#include "chooseapectdialog.h"
#include "ui_chooseapectdialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

ChooseApectDialog::ChooseApectDialog(QWidget *parent)
    : QDialog(parent)
    , ui(nullptr)
{
    //ui->setupUi(this);

    setWindowTitle("Elige tu aspecto");
    setModal(true);
    setFixedSize(420, 240);

    auto *lbl = new QLabel("Elige tu escuela filosófica:");
    auto *bnEmp = new QPushButton("EMPIRISTAS");
    auto *bnRac = new QPushButton("RACIONALISTAS");

    // (Opcional) Iconos de vista previa:
    bnEmp->setIcon(QIcon("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/previaV.png"));
    bnRac->setIcon(QIcon("C:/Users/Dell/Documents/Documentos/PROYECTO/Gamificacion/images/previaR.png"));
    bnEmp->setMinimumHeight(64);
    bnRac->setMinimumHeight(64);

    connect(bnEmp, &QPushButton::clicked, this, &ChooseApectDialog::onEmpirista);
    connect(bnRac, &QPushButton::clicked, this, &ChooseApectDialog::onRacionalista);

    auto *row = new QHBoxLayout();
    row->addWidget(bnEmp, 1);
    row->addWidget(bnRac, 1);

    auto *col = new QVBoxLayout(this);
    col->addWidget(lbl);
    col->addSpacing(16);
    col->addLayout(row);
    setLayout(col);


}

ChooseApectDialog::~ChooseApectDialog() = default;

void ChooseApectDialog::onEmpirista()    { elegido = Empirista;    accept(); }
void ChooseApectDialog::onRacionalista() { elegido = Racionalista; accept(); }
