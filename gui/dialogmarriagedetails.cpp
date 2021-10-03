#include "dialogmarriagedetails.h"
#include "ui_dialogmarriagedetails.h"

DialogMarriageDetails::DialogMarriageDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMarriageDetails)
{
    ui->setupUi(this);
}

DialogMarriageDetails::~DialogMarriageDetails()
{
    delete ui;
}

void DialogMarriageDetails::setMarriage(MarriageItem *marriage)
{

}
