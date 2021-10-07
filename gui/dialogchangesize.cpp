#include "dialogchangesize.h"
#include "ui_dialogchangesize.h"

DialogChangeSize::DialogChangeSize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogChangeSize)
{
    ui->setupUi(this);
}

DialogChangeSize::~DialogChangeSize()
{
    delete ui;
}

void DialogChangeSize::setCurrentSize(const QSizeF &size)
{
    ui->spinBoxHorizontal->setValue(size.width());
    ui->spinBoxVertical->setValue(size.height());
}

int DialogChangeSize::getNewWidth() const
{
    return ui->spinBoxHorizontal->value();
}

int DialogChangeSize::getNewHeight() const
{
    return ui->spinBoxVertical->value();
}

void DialogChangeSize::on_pushButtonCancel_clicked()
{
    reject();
}

void DialogChangeSize::on_pushButtonOK_clicked()
{
    accept();
}
