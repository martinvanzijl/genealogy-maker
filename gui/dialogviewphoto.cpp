#include "dialogviewphoto.h"
#include "ui_dialogviewphoto.h"

#include <QDir>
#include <QImageReader>
#include <QMessageBox>

DialogViewPhoto::DialogViewPhoto(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogViewPhoto) //,
//    m_photoLoadedOK(false)
{
    ui->setupUi(this);
}

DialogViewPhoto::~DialogViewPhoto()
{
    delete ui;
}

//bool DialogViewPhoto::photoLoadedOK() const
//{
//    return m_photoLoadedOK;
//}

void DialogViewPhoto::setPhoto(const QString &fileName)
{
    // Set flag.
//    m_photoLoadedOK = false;

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
//    m_photoLoadedOK = true;
}
