#include "dialogmarriagedetails.h"
#include "ui_dialogmarriagedetails.h"

#include "marriageitem.h"
#include "diagramitem.h"

DialogMarriageDetails::DialogMarriageDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMarriageDetails),
    m_marriage(nullptr)
{
    ui->setupUi(this);
//    setTextGrayedOut(ui->dateEditDate, true);
}

DialogMarriageDetails::~DialogMarriageDetails()
{
    delete ui;
}

void DialogMarriageDetails::setMarriage(MarriageItem *marriage)
{
    m_marriage = marriage;

    auto name1 = marriage->personLeft()->name();
    auto name2 = marriage->personRight()->name();
    ui->labelBetween->setText(QString("Marriage between %1 and %2:").arg(name1, name2));
    ui->dateEditDate->setDate(marriage->getDate());
//    ui->checkBoxDateUnknown->setChecked(!marriage->isDateKnown());
    ui->lineEditPlace->setText(marriage->getPlace());
}

void DialogMarriageDetails::save()
{
//    if (ui->checkBoxDateUnknown->isChecked())
//    {
//        m_marriage->setDate(QDate());
//    }
//    else
//    {
        m_marriage->setDate(ui->dateEditDate->date());
//    }
    m_marriage->setPlace(ui->lineEditPlace->text());
}

void DialogMarriageDetails::on_pushButtonSave_clicked()
{
    save();
    close();
}

void DialogMarriageDetails::on_pushButtonClose_clicked()
{
    close();
}

//void DialogMarriageDetails::setTextGrayedOut(QWidget *widget, bool grayedOut)
//{
//    if (grayedOut)
//    {
//        QPalette pal = widget->palette();
//        pal.setColor(QPalette::Text, Qt::gray);
//        widget->setPalette(pal);
//    }
//    else
//    {
//        widget->setPalette(QPalette());
//    }
//}

//void DialogMarriageDetails::on_checkBoxDateUnknown_stateChanged(int state)
//{
//    Q_UNUSED(state);
//    setTextGrayedOut (ui->dateEditDate, ui->checkBoxDateUnknown->isChecked());
//}

//void DialogMarriageDetails::on_dateEditDate_dateChanged(const QDate &date)
//{
//    Q_UNUSED(date);
//    ui->checkBoxDateUnknown->setChecked(false);
//}
