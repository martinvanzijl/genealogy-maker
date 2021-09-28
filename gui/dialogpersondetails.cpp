#include "dialogpersondetails.h"
#include "ui_dialogpersondetails.h"

#include "diagramitem.h"

DialogPersonDetails::DialogPersonDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPersonDetails)
{
    ui->setupUi(this);
}

DialogPersonDetails::~DialogPersonDetails()
{
    delete ui;
}

void DialogPersonDetails::setItem(DiagramItem *item)
{
    ui->lineEditName->setText(item->name());
}

void DialogPersonDetails::on_pushButtonClose_clicked()
{
    close();
}

void DialogPersonDetails::on_pushButtonSave_clicked()
{
    save();
    close();
}

void DialogPersonDetails::save()
{
    // TODO: Save.
}
