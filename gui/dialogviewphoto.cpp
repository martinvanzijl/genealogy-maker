#include "dialogviewphoto.h"
#include "ui_dialogviewphoto.h"

#include <QDebug>
#include <QDir>
#include <QImageReader>
#include <QMessageBox>

DialogViewPhoto::DialogViewPhoto(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogViewPhoto),
    m_photoLoadedOK(false),
    m_currentPhotoIndex(-1)
{
    ui->setupUi(this);
}

DialogViewPhoto::~DialogViewPhoto()
{
    delete ui;
}

bool DialogViewPhoto::photoLoadedOK() const
{
    return m_photoLoadedOK;
}

void DialogViewPhoto::setPhoto(const QString &fileName)
{
    // Set flag.
    m_photoLoadedOK = false;

    // Create reader.
    QImageReader reader(fileName);
    reader.setAutoTransform(true);

    // Read image.
    const QImage image = reader.read();
    if (image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return;
    }

    // Set image.
    ui->labelPhoto->setPixmap(QPixmap::fromImage(image));

//    if (!fitToWindowAct->isChecked())
//        imageLabel->adjustSize();

    adjustSize();

    // Set flag.
    m_photoLoadedOK = true;
}

/// Set the current photo index.
void DialogViewPhoto::setPhotoIndex(int index)
{
    if (index >= 0 && index < m_photoList.size()) {
        m_currentPhotoIndex = index;
        setPhoto(m_photoList[m_currentPhotoIndex]);
    }
    else {
        qDebug() << "Invalid photo index:" << index;
    }
}

void DialogViewPhoto::setPhotoList(const QStringList &list)
{
    m_photoList = list;

    // Hide navigation buttons if there are less than two photos.
   ui->frameNavigation->setVisible( m_photoList.size() > 1 );
}

void DialogViewPhoto::on_pushButtonPrevious_clicked()
{
    goToPrevious();
}

/// Go to the previous photo.
void DialogViewPhoto::goToPrevious()
{
    // Exit if list if empty.
    if (m_photoList.isEmpty()) {
        return;
    }

    // Decrement index.
    m_currentPhotoIndex -= 1;

    // Ensure in bounds.
    if (m_currentPhotoIndex < 0) {
        m_currentPhotoIndex = m_photoList.size() - 1;
    }

    // Show photo.
    setPhoto(m_photoList[m_currentPhotoIndex]);
}

void DialogViewPhoto::on_pushButtonNext_clicked()
{
    goToNext();
}

/// Go to the next photo.
void DialogViewPhoto::goToNext()
{
    // Exit if list if empty.
    if (m_photoList.isEmpty()) {
        return;
    }

    // Increment index.
    m_currentPhotoIndex += 1;

    // Ensure in bounds.
    if (m_currentPhotoIndex >= m_photoList.size()) {
        m_currentPhotoIndex = 0;
    }

    // Show photo.
    setPhoto(m_photoList[m_currentPhotoIndex]);
}
