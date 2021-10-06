#include "dialoghelp.h"
#include "ui_dialoghelp.h"

#include <QHelpContentWidget>

DialogHelp::DialogHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHelp)
{
    ui->setupUi(this);
}

DialogHelp::~DialogHelp()
{
    delete ui;
}
