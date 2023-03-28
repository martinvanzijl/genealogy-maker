#include "helpwindow.h"
#include "ui_helpwindow.h"

#include <QFile>
#include <QMessageBox>

HelpWindow::HelpWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HelpWindow)
{
    ui->setupUi(this);
    loadHelpFile();
}

HelpWindow::~HelpWindow()
{
    delete ui;
}

void HelpWindow::on_pushButtonClose_clicked()
{
    close();
}

void HelpWindow::loadHelpFile()
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
