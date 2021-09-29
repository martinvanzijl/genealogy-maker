#include "dialogpersondetails.h"
#include "ui_dialogpersondetails.h"

#include "diagramitem.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QImage>

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

void DialogPersonDetails::on_pushButtonAddPhoto_clicked()
{
    // Default to home.
    QString folderPath = QDir::home().path();

    // Try to use standard pictures folder.
    auto list = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    if (list.isEmpty()) {
        folderPath = list.first();
    }

    // Ask for picture file.
    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Open Image"), folderPath,
                                         tr("Image Files (*.png *.jpg *.bmp)"));

    // Add to list view.
    if (!fileName.isEmpty()) {
        //QImage image(fileName);
        QIcon icon(fileName);
        QFileInfo info(fileName);
        auto item = new QListWidgetItem(icon, info.fileName());
        ui->listWidgetPhotos->addItem(item);
    }
}
