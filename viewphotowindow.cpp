/*
 * This file is part of the Genealogy Maker program (https://github.com/martinvanzijl/genealogy-maker).
 * Copyright (c) 2023 Martin van Zijl.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtWidgets>

#include "viewphotowindow.h"

ViewPhotoWindow::ViewPhotoWindow(QWidget *parent)
    : QMainWindow(parent)
    , imageLabel(new QLabel)
    , scrollArea(new QScrollArea)
    , scaleFactor(1)
{
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    scrollArea->setAlignment(Qt::AlignCenter);
//    setCentralWidget(scrollArea);

    // Create nagivation panel.
    navigationPanel = new QFrame(this);

    QPushButton *buttonPrevious = new QPushButton("< Previous", this);
    QPushButton *buttonNext = new QPushButton("Next >", this);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(buttonPrevious);
    layout->addWidget(buttonNext);
    layout->setMargin(4);
    navigationPanel->setLayout(layout);

    // Create overall layout.
    QVBoxLayout *overallLayout = new QVBoxLayout();
    overallLayout->addWidget(scrollArea);
    overallLayout->addWidget(navigationPanel);
    overallLayout->setMargin(0);

    // Create central widget.
    QFrame *overallFrame = new QFrame(this);
    overallFrame->setLayout(overallLayout);
    setCentralWidget(overallFrame);

    // Create the actions.
    createActions();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

    // Set "fit image" as default.
//    fitToWindowAct->setChecked(true);

    // Connect button actions.
    connect(buttonPrevious, SIGNAL(clicked()), this, SLOT(goToPrevious()));
    connect(buttonNext, SIGNAL(clicked()), this, SLOT(goToNext()));

    // Hide navigation panel by default.
    navigationPanel->setVisible(false);

    //
    // Center the window on the parent.
    //
    if (parent)
    {
        auto geometry = parent->geometry();
        auto halfSize = size() / 2.0;
        QPoint offset(halfSize.width(), halfSize.height());
        move(geometry.center() - offset);
    }
}

bool ViewPhotoWindow::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    setImage(newImage);

    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
            .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    statusBar()->showMessage(message);
    return true;
}

void ViewPhotoWindow::resizeEvent(QResizeEvent *ev)
{
    Q_UNUSED(ev)

    if (fitToWindowAct->isChecked()) {
        fitToWindow();
    }
}

void ViewPhotoWindow::setImage(const QImage &newImage)
{
    image = newImage;
    imageLabel->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1.0;

    scrollArea->setVisible(true);
    fitToWindowAct->setEnabled(true);
    updateActions();

    if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();
    else
        fitToWindow();
}


bool ViewPhotoWindow::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}


static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
            ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void ViewPhotoWindow::saveAs()
{
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    // Allow typing in a file name.
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
//    dialog.setDirectory(imageFilePath);

    // Show the dialog.
    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

void ViewPhotoWindow::copy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}

void ViewPhotoWindow::zoomIn()
{
    scaleImage(1.25);
}

void ViewPhotoWindow::zoomOut()
{
    scaleImage(0.8);
}

void ViewPhotoWindow::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void ViewPhotoWindow::fitToWindow()
{
    // Safety check.
    if (image.isNull()) return;

    bool fitToWindow = fitToWindowAct->isChecked();
    //scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    else {
        // Scale to fill window, while keeping aspect ratio.
        double imageHeight = imageLabel->pixmap()->height();
        double imageWidth = imageLabel->pixmap()->width();
        double labelHeight = height() - menuBar()->height() - statusBar()->height() - 2;

        if (navigationPanel->isVisible()) {
            labelHeight -= navigationPanel->height() + navigationPanel->layout()->margin() + 2;
        }

        double labelWidth = width() - 2;
        double widthRatio = labelWidth / imageWidth;
        double heightRatio = labelHeight / imageHeight;
        double scaleBy = qMin(widthRatio, heightRatio);

        scaleFactor = 1.0;
        scaleImage(scaleBy);
    }
    updateActions();
}

void ViewPhotoWindow::goToNext()
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

void ViewPhotoWindow::goToPrevious()
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

void ViewPhotoWindow::toggleNavigationPanel()
{
    navigationPanel->setVisible(showNavigationPanelAct->isChecked());

    // Adjust image if "fit to window" is enabled.
    if (fitToWindowAct->isChecked()) {
        fitToWindow();
    }
}

void ViewPhotoWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &ViewPhotoWindow::saveAs);
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    saveAsAct->setEnabled(false);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("C&lose"), this, &QWidget::close);
    exitAct->setShortcut(QKeySequence::Cancel);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    copyAct = editMenu->addAction(tr("&Copy"), this, &ViewPhotoWindow::copy);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setEnabled(false);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &ViewPhotoWindow::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &ViewPhotoWindow::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &ViewPhotoWindow::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &ViewPhotoWindow::fitToWindow);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));

    QMenu *navigateMenu = menuBar()->addMenu(tr("&Navigate"));

    goToNextAct = navigateMenu->addAction(tr("Go to &Next"), this, &ViewPhotoWindow::goToNext);
    goToNextAct->setShortcut(tr("Ctrl+N"));

    goToPreviousAct = navigateMenu->addAction(tr("Go to &Previous"), this, &ViewPhotoWindow::goToPrevious);
    goToPreviousAct->setShortcut(tr("Ctrl+P"));

    navigateMenu->addSeparator();

    showNavigationPanelAct = navigateMenu->addAction(tr("&Show Navigation Panel"), this, &ViewPhotoWindow::toggleNavigationPanel);
    showNavigationPanelAct->setCheckable(true);
    showNavigationPanelAct->setShortcut(tr("Ctrl+H"));
}

void ViewPhotoWindow::updateActions()
{
    saveAsAct->setEnabled(!image.isNull());
    copyAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void ViewPhotoWindow::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ViewPhotoWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

bool ViewPhotoWindow::photoLoadedOK() const
{
    return m_photoLoadedOK;
}

void ViewPhotoWindow::setPhoto(const QString &fileName)
{
    m_photoLoadedOK = loadFile(fileName);
}

/// Set the current photo index.
void ViewPhotoWindow::setPhotoIndex(int index)
{
    if (index >= 0 && index < m_photoList.size()) {
        m_currentPhotoIndex = index;
        setPhoto(m_photoList[m_currentPhotoIndex]);
    }
    else {
        qDebug() << "Invalid photo index:" << index;
    }
}

void ViewPhotoWindow::setPhotoList(const QStringList &list)
{
    m_photoList = list;

    // Hide navigation buttons if there are less than two photos.
//    ui->frameNavigation->setVisible( m_photoList.size() > 1 );
}
