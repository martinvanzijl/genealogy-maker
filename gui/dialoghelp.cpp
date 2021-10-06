#include "dialoghelp.h"
#include "ui_dialoghelp.h"

#include <QFile>
#include <QMessageBox>

DialogHelp::DialogHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHelp)
{
    ui->setupUi(this);
    loadHelpFile();
}

DialogHelp::~DialogHelp()
{
    delete ui;
}

void DialogHelp::loadHelpFile()
{
    // Open file.
    QString fileName = "doc/genealogy-maker-manual.html";
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Genealogy Maker"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    // Show in form.
    QString html = file.readAll();
    ui->textBrowserMain->setHtml(html);
}

void DialogHelp::on_pushButtonClose_clicked()
{
    close();
}
