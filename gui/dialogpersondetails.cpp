#include "dialogpersondetails.h"
#include "ui_dialogpersondetails.h"

#include "diagramitem.h"
#include "dialogviewphoto.h"
#include "fileutils.h"
#include "undo/editpersondetailsundo.h"
#include "undo/undomanager.h"

#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>
#include <QImage>
#include <QDesktopServices>

const static int CBOX_GENDER_MALE = 0;
const static int CBOX_GENDER_FEMALE = 1;
const static int CBOX_GENDER_UNKNOWN = 2;

DialogPersonDetails::DialogPersonDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPersonDetails)
{
    ui->setupUi(this);
//    setTextGrayedOut(ui->dateEditBirth, true);
//    setTextGrayedOut(ui->dateEditDeath, true);

    m_viewPhotoDialog = nullptr;
}

DialogPersonDetails::~DialogPersonDetails()
{
    delete ui;
}

void DialogPersonDetails::setItem(DiagramItem *item)
{
    ui->lineEditFirstName->setText(item->getFirstName());
    ui->lineEditLastName->setText(item->getLastName());
    ui->lineEditName->setText(item->name());
    ui->plainTextEditBio->setPlainText(item->bio());
    ui->dateEditBirth->setDate(item->getDateOfBirth());
//    ui->checkBoxDateOfBirthUnknown->setChecked(item->getDateOfBirth().isNull());
    ui->dateEditDeath->setDate(item->getDateOfDeath());
//    ui->checkBoxDateOfDeathUnknown->setChecked(item->getDateOfDeath().isNull());
    ui->lineEditPlaceOfBirth->setText(item->getPlaceOfBirth());
    ui->lineEditCountryOfBirth->setText(item->getCountryOfBirth());
    ui->lineEditPlaceOfDeath->setText(item->getPlaceOfDeath());

    if (item->isGenderKnown())
    {
        if (item->getGender() == "M")
        {
            ui->comboBoxGender->setCurrentIndex(CBOX_GENDER_MALE);
        }
        else
        {
            ui->comboBoxGender->setCurrentIndex(CBOX_GENDER_FEMALE);
        }
    }
    else
    {
        ui->comboBoxGender->setCurrentIndex(CBOX_GENDER_UNKNOWN);
    }

    ui->listWidgetPhotos->clear();

    for (auto photo: item->photos())
    {
        addPhoto(photo);
    }

    // Update window title.
    setWindowTitle(QString("Person Details - ") + item->name());

    m_item = item;
}

void DialogPersonDetails::viewDefaultTab()
{
    ui->tabWidgetMain->setCurrentIndex(0);
}

void DialogPersonDetails::setXmlFile(const QString &xmlFile)
{
    m_xmlFile = xmlFile;
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
        EditPersonDetailsUndo *undo = new EditPersonDetailsUndo(m_item);

        m_item->setFirstName(ui->lineEditFirstName->text());
        m_item->setLastName(ui->lineEditLastName->text());
        m_item->setName(ui->lineEditName->text());
        m_item->setBio(ui->plainTextEditBio->toPlainText());

//        if (ui->checkBoxDateOfBirthUnknown->isChecked())
//        {
//            m_item->setDateOfBirth(QDate());
//        }
//        else
//        {
            m_item->setDateOfBirth(ui->dateEditBirth->date());
//        }

//        if (ui->checkBoxDateOfDeathUnknown->isChecked())
//        {
//            m_item->setDateOfDeath(QDate());
//        }
//        else
//        {
            m_item->setDateOfDeath(ui->dateEditDeath->date());
//        }

        m_item->setPlaceOfBirth(ui->lineEditPlaceOfBirth->text());
        m_item->setCountryOfBirth(ui->lineEditCountryOfBirth->text());
        m_item->setPlaceOfDeath(ui->lineEditPlaceOfDeath->text());

        switch (ui->comboBoxGender->currentIndex()) {
        case CBOX_GENDER_MALE:
            m_item->setGender("M");
            break;

        case CBOX_GENDER_FEMALE:
            m_item->setGender("F");
            break;

        case CBOX_GENDER_UNKNOWN:
            m_item->setGender("");
            break;
        }

        QStringList photos;
        for (int i = 0; i < ui->listWidgetPhotos->count(); ++i)
        {
            auto fullFileName = ui->listWidgetPhotos->item(i)->data(Qt::UserRole).toString();
            photos << fullFileName;
        }
        m_item->setPhotos(photos);

        undo->setAfterState(m_item);
        UndoManager::add(undo);
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

void DialogPersonDetails::viewPhoto(const QString &fileName)
{
    if (!m_viewPhotoDialog) {
        m_viewPhotoDialog = new DialogViewPhoto(this);
    }
    m_viewPhotoDialog->show();
    m_viewPhotoDialog->setPhoto(fileName);
}

QString DialogPersonDetails::getPhotosFolder() const
{
    return FileUtils::getPhotosFolderFor(m_xmlFile);
}

//void DialogPersonDetails::setTextGrayedOut(QWidget *widget, bool grayedOut)
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

        //
        // TODO: Copy the photo to the save-file directory, if enabled.
        //
//        bool copyImmediatelyToProjectFolder = false;

//        if (copyImmediatelyToProjectFolder)
//        {
//            // Create the directory.
//            QDir photoFolder(getPhotosFolder());
//            QString personFolderName = m_item->id().toString();
//            photoFolder.mkdir(personFolderName);
//            QDir personFolder(photoFolder.filePath(personFolderName));

//            // Copy photo to folder.
//            QFileInfo info(fileName);
//            QString destFilePath = personFolder.filePath(info.fileName());
//            bool copiedOK = QFile::copy(fileName, destFilePath);

//            // Check for existing files.
//            if (copiedOK) {
//                qDebug() << "Photo copied successfully.";
//                fileName = destFilePath;
//            }
//            else {
//                // TODO: Try renaming the copied file.
//                qDebug() << "Could not copy photo" << fileName << "to" << destFilePath;
//            }
//        }

        // Add the photo.
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

void DialogPersonDetails::on_listWidgetPhotos_itemDoubleClicked(QListWidgetItem *item)
{
    auto fullFileName = item->data(Qt::UserRole).toString();
//    QString url = QString("file://") + fullFileName;
//    QDesktopServices::openUrl(url);
    viewPhoto(fullFileName);
}


//void DialogPersonDetails::on_dateEditBirth_dateChanged(const QDate &date)
//{
//    Q_UNUSED(date);
//    ui->checkBoxDateOfBirthUnknown->setChecked(false);
//}

//void DialogPersonDetails::on_dateEditDeath_dateChanged(const QDate &date)
//{
//    Q_UNUSED(date);
//    ui->checkBoxDateOfDeathUnknown->setChecked(false);
//}

//void DialogPersonDetails::on_checkBoxDateOfBirthUnknown_stateChanged(int state)
//{
//    Q_UNUSED(state);
//    setTextGrayedOut (ui->dateEditBirth, ui->checkBoxDateOfBirthUnknown->isChecked());
//}

//void DialogPersonDetails::on_checkBoxDateOfDeathUnknown_stateChanged(int state)
//{
//    Q_UNUSED(state);
//    setTextGrayedOut (ui->dateEditDeath, ui->checkBoxDateOfDeathUnknown->isChecked());
//}
