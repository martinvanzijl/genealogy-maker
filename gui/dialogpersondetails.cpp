#include "dialogpersondetails.h"
#include "ui_dialogpersondetails.h"

#include "diagramitem.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QImage>
#include <QDebug>

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
    ui->plainTextEditBio->setPlainText(item->bio());

    ui->listWidgetPhotos->clear();

    for (auto photo: item->photos())
    {
        addPhoto(photo);
    }

    m_item = item;
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
    if (m_item)
    {
        m_item->setName(ui->lineEditName->text());
        m_item->setBio(ui->plainTextEditBio->toPlainText());

        QStringList photos;
        for (int i = 0; i < ui->listWidgetPhotos->count(); ++i)
        {
            auto fullFileName = ui->listWidgetPhotos->item(i)->data(Qt::UserRole).toString();
            photos << fullFileName;
        }
        m_item->setPhotos(photos);
    }

}

void DialogPersonDetails::addPhoto(const QString &fileName)
{
    //QImage image(fileName);
    QIcon icon(fileName);
    QFileInfo info(fileName);
    auto item = new QListWidgetItem(icon, info.fileName());
    item->setData(Qt::UserRole, fileName);
    ui->listWidgetPhotos->addItem(item);
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
        addPhoto(fileName);
    }
}

void DialogPersonDetails::on_pushButtonRemovePhoto_clicked()
{
    auto selectedItems = ui->listWidgetPhotos->selectedItems();

    for (auto item: selectedItems)
    {
        delete item;
    }
}
