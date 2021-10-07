#include "dialogfind.h"
#include "ui_dialogfind.h"

DialogFind::DialogFind(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFind)
{
    ui->setupUi(this);
}

DialogFind::~DialogFind()
{
    delete ui;
}

void DialogFind::setStatus(const QString &text)
{
    ui->labelStatus->setText(text);
}

void DialogFind::on_pushButtonClose_clicked()
{
    close();
}

void DialogFind::on_pushButtonFind_clicked()
{
    auto text = ui->lineEditText->text();
    if (!text.isEmpty())
    {
        emit search(text);
    }
}
