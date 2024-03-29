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

#include "mainform.h"
#include "ui_mainform.h"

#include "arrow.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
#include "fileutils.h"
#include "mygraphicsview.h"
#include "percentvalidator.h"
#include "undo/addarrowundo.h"
#include "undo/additemundo.h"
#include "undo/deleteitemsundo.h"
#include "undo/moveitemsundo.h"
#include "gui/dialogfind.h"
#include "gui/dialogpersondetails.h"
#include "undo/marriageundo.h"
#include "marriageitem.h"
#include "gui/dialogmarriagedetails.h"
#include "undo/removemarriageundo.h"
//#include "gui/dialoghelp.h"
#include "gui/helpwindow.h"
#include "gui/dialogchangesize.h"
#include "undo/undomanager.h"
#include "draggablebutton.h"
#include "preferenceswindow.h"
#include "gui/reportwindow.h"
#include "gui/timelinereportwindow.h"
#include "gui/dialogfileproperties.h"
#include "undo/changediagramsizeundo.h"

#include <QtWidgets>
#include <QPrinter>
#include <QPrintDialog>
#include <QUndoStack>
#include <QSlider>

const int InsertArrowButton = 11;

#include <QDesktopWidget>

MainForm::MainForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainForm),
    m_beingDestroyed(false),
    m_gedcomWasImported(false),
    treeFocusedItem(nullptr),
    m_openingRecentFile(false),
    m_disableZoomSliderSignal(false)
{
    ui->setupUi(this);

    undoStack = new QUndoStack(this);
    connect(undoStack, SIGNAL(cleanChanged(bool)), this, SLOT(onUndoStackCleanChanged(bool)));
    UndoManager::setStack(undoStack);
    m_appName = "Genealogy Maker Qt";

    createActions();
    createToolBox();
    createMenus();

    scene = new DiagramScene(ui->itemMenu, this);
    scene->setSceneRect(QRectF(0, 0, 5000, 5000));
    scene->setWindow(this);
    connect(scene, SIGNAL(itemInserted(DiagramItem*, bool)),
            this, SLOT(itemInserted(DiagramItem*, bool)));
    connect(scene, SIGNAL(itemRemoved(DiagramItem*)),
            this, SLOT(onItemRemoved(DiagramItem*)));
    connect(scene, SIGNAL(itemSelected(QGraphicsItem*)),
            this, SLOT(itemSelected(QGraphicsItem*)));
    connect(scene, SIGNAL(peopleMarried(DiagramItem*,DiagramItem*)),
            this, SLOT(onPeopleMarried(DiagramItem*,DiagramItem*)));
    createToolbars();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(toolBox);

    // Add "collapse/expand" button.
    QString buttonText = "<<";
    collapseButton = new QPushButton(buttonText, this);

    int textWidth = collapseButton->fontMetrics().boundingRect(buttonText).width();
    int margin = 6;
    int buttonWidth = textWidth + (2 * margin);
    collapseButton->setMaximumWidth(buttonWidth);
    collapseButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    connect(collapseButton, SIGNAL(clicked(bool)), this, SLOT(onCollapseButtonClicked(bool)));

    layout->addWidget(collapseButton);

    // Add diagram.
    view = new MyGraphicsView(scene);
    connect(scene, SIGNAL(mouseReleased()), view, SLOT(onMouseReleased()));
    connect(scene, SIGNAL(arrowAdded(Arrow*)), this, SLOT(onArrowAdded(Arrow*)));
    connect(scene, SIGNAL(itemsAboutToMove()), this, SLOT(onItemsAboutToMove()));
    connect(scene, SIGNAL(itemsFinishedMoving()), this, SLOT(onItemsFinishedMoving()));
    connect(scene, SIGNAL(cleared()), this, SLOT(onSceneCleared()));
    connect(scene, SIGNAL(personDoubleClicked(DiagramItem*)), this, SLOT(onPersonDoubleClicked(DiagramItem*)));
    connect(view, SIGNAL(mouseWheelZoomed()), this, SLOT(onMouseWheelZoomed()));
    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    updateWindowTitle();
    setUnifiedTitleAndToolBarOnMac(true);

    view->setFocus();

    scaleTextEditedByUser = false;
    moveItemsUndo = nullptr;
    dialogFind = nullptr;
    dialogPersonDetails = nullptr;
    dialogMarriageDetails = nullptr;
//    dialogHelp = nullptr;
    helpWindow = nullptr;
    preferencesWindow = nullptr;
    dialogFileProperties = nullptr;

    // Set preference fields.
    QCoreApplication::setOrganizationName("Martin van Zijl");
    QCoreApplication::setOrganizationDomain("martinvz.com");
    QCoreApplication::setApplicationName("Genealogy Maker");

    // Load preferences.
    onPreferencesChanged();

    // Create "Recent Files" menu.
    QMenu *recentMenu = new QMenu(tr("Recent Files"), this);
    recentMenu->setObjectName("recentFilesMenu");
    ui->menuFile->insertMenu(ui->saveAction, recentMenu);
    connect(recentMenu, SIGNAL(aboutToShow()), this, SLOT(updateRecentFileActions()));
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, SLOT(openRecentFile()));
        recentFileActs[i]->setVisible(false);
    }

    recentMenu->addSeparator();
    recentMenu->addAction(tr("&Clear"), this, SLOT(clearRecentFilesMenu()));

    setRecentFilesVisible(hasRecentFiles());
}

MainForm::~MainForm()
{
    m_beingDestroyed = true;
    delete ui;
}

void MainForm::backgroundButtonGroupClicked(QAbstractButton *button)
{
    QList<QAbstractButton *> buttons = backgroundButtonGroup->buttons();
    foreach (QAbstractButton *myButton, buttons) {
        if (myButton != button)
            button->setChecked(false);
    }
    QString text = button->text();
    if (text == tr("Blue Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background1.png"));
    else if (text == tr("White Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background2.png"));
    else if (text == tr("Gray Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background3.png"));
    else
        scene->setBackgroundBrush(QPixmap(":/images/background4.png"));

    scene->update();
    view->update();
}

void MainForm::buttonGroupClicked(int id)
{
    // Highlight the correct button.
    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    foreach (QAbstractButton *button, buttons) {
        button->setChecked(buttonGroup->button(id) == button);
    }

    // Set the diagram mode.
    if (id == InsertArrowButton)  {
//        linePointerButton->click();
        scene->setMode(DiagramScene::InsertLine);
    }
    else {
        scene->setItemType(DiagramItem::DiagramType(id));
        scene->setMode(DiagramScene::InsertItem);
    }

    // Deselet the "pointer" button.
    pointerButton->setChecked(false);
}

void MainForm::deleteItem()
{
    QSet<QGraphicsItem *> itemsRemoved;

    //
    // Delete relationships.
    //
    foreach (QGraphicsItem *item, scene->selectedItems()) {
        if (item->type() == Arrow::Type) {
            scene->removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            arrow->startItem()->removeArrow(arrow);
            arrow->endItem()->removeArrow(arrow);
            itemsRemoved << arrow;
        }
    }

    //
    // Delete persons.
    //
    foreach (QGraphicsItem *item, scene->selectedItems()) {
        if (item->type() == DiagramItem::Type) {
            DiagramItem *diagramItem = qgraphicsitem_cast<DiagramItem *> (item);

            for (Arrow *arrow: diagramItem->getArrows()) {
                itemsRemoved << arrow;
            }

            diagramItem->removeArrows();
            delete treeItems[diagramItem->id()];
        }
        scene->removeItem(item);
        itemsRemoved << item;
    }

    undoStack->push(new DeleteItemsUndo(scene, itemsRemoved.toList()));
}

void MainForm::pointerGroupClicked(int)
{
    // Hack: Ensure the pointer button is checked, since it is the
    // only button in the group.
    pointerButton->setChecked(true);

    // Set the diagram mode.
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));

    // Update button group in side bar.
    QList<QAbstractButton *> buttons = buttonGroup->buttons();

    // 1) Deselect all buttons.
    foreach (QAbstractButton *button, buttons) {
        button->setChecked(false);
    }

    // 2) Select the arrow button if required.
    if (pointerTypeGroup->checkedButton() == linePointerButton) {
        arrowButton->setChecked(true);
    }
}

void MainForm::bringToFront()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);
}

void MainForm::sendToBack()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() <= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() - 0.1;
    }
    selectedItem->setZValue(zValue);
}

void MainForm::itemInserted(DiagramItem *item, bool fromLoad)
{
    // Add item to list view.
    auto id = item->id();
    QStringList list;
    list << item->name();
    auto treeItem = new QTreeWidgetItem(list);
    tree->addTopLevelItem(treeItem);
    treeItem->setData(0, Qt::UserRole, id);
    treeItems[id] = treeItem;
    connect(item->textItem(), &DiagramTextItem::textEdited, this, &MainForm::onItemTextEdited);

    if (!fromLoad)
    {
        pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
        scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
        buttonGroup->button(int(item->diagramType()))->setChecked(false);

        // Update undo stack.
        undoStack->push(new AddItemUndo(scene, item));
    }
}

void MainForm::onItemRemoved(DiagramItem *item)
{
    delete treeItems[item->id()];
}

void MainForm::currentFontChanged(const QFont &)
{
//    handleFontChange();
}

void MainForm::fontSizeChanged(const QString &)
{
//    handleFontChange();
}

void MainForm::sceneScaleActivated(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    setSceneScale(newScale);

    // Update slider.
    m_disableZoomSliderSignal = true;
    zoomSlider->setValue(newScale * 100);
    m_disableZoomSliderSignal = false;
}

void MainForm::sceneScaleEditingFinished()
{
    if (scaleTextEditedByUser) {
        sceneScaleActivated(sceneScaleCombo->currentText());
    }
    scaleTextEditedByUser = false;
}

void MainForm::sceneScaleTextEdited(const QString &scale)
{
    Q_UNUSED(scale);

    scaleTextEditedByUser = true;
}

void MainForm::zoomSliderValueChanged(int value)
{
    // Disable if updating from combo-box.
    if (m_disableZoomSliderSignal) {
        return;
    }

    // Set the zoom.
    double scale = static_cast<double> (value) / 100.0;
    setSceneScale(scale);

    // Update combo-box.
    sceneScaleCombo->setCurrentText(QString::number(value) + tr("%"));
}

void MainForm::textColorChanged()
{
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/textpointer.png",
                                     qvariant_cast<QColor>(textAction->data())));
    textButtonTriggered();
}

void MainForm::itemColorChanged()
{
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/floodfill.png",
                                     qvariant_cast<QColor>(fillAction->data())));
    fillButtonTriggered();
}

void MainForm::lineColorChanged()
{
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/linecolor.png",
                                     qvariant_cast<QColor>(lineAction->data())));
    lineButtonTriggered();
}

void MainForm::textButtonTriggered()
{
    scene->setTextColor(qvariant_cast<QColor>(textAction->data()));
}

void MainForm::fillButtonTriggered()
{
    const QColor color = QColorDialog::getColor(Qt::green, this, "Select Fill Color");

    if (color.isValid()) {
        scene->setItemColor(color);
        fillColorToolButton->setIcon(createColorToolButtonIcon(":/images/floodfill.png", color));
    }
}

void MainForm::lineButtonTriggered()
{
    scene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
}

//void MainForm::handleFontChange()
//{
//    QFont font = fontCombo->currentFont();
//    font.setPointSize(fontSizeCombo->currentText().toInt());
//    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
//    font.setItalic(italicAction->isChecked());
//    font.setUnderline(underlineAction->isChecked());

//    scene->setFont(font);
//}

void MainForm::itemSelected(QGraphicsItem *item)
{
    Q_UNUSED(item);

//    DiagramTextItem *textItem =
//    qgraphicsitem_cast<DiagramTextItem *>(item);

//    QFont font = textItem->font();
//    fontCombo->setCurrentFont(font);
//    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
//    boldAction->setChecked(font.weight() == QFont::Bold);
//    italicAction->setChecked(font.italic());
//    underlineAction->setChecked(font.underline());
}

void MainForm::about()
{
    QString message = tr("This program is for creating genealogies. It is "
                         "written using the Qt Framework. The program is "
                         "open-source and you are welcome to contribute "
                         "to it."
                         "\n\n"
                         "The code is available on GitHub here: "
                         "https://github.com/martinvanzijl/genealogy-maker"
                         "\n\n"
                         "Version: 29");
    QMessageBox::about(this, tr("About Genealogy Maker"), message);
}

void MainForm::newDiagram()
{
    // Ask whether to save unsaved changes.
    if (!maybeSave()) {
        return;
    }

    // Clear undo stack.
    undoStack->clear();
    undoStack->setClean();

    // Clear scene.
    scene->clear();
    tree->clear();
    treeItems.clear();

    // Clear save file name.
    m_saveFileName.clear();

    // Reset flag.
    m_gedcomWasImported = false;

    // Update window title.
    updateWindowTitle();

    // Set default size.
    scene->setSceneRect(QRectF(0, 0, 5000, 5000));
}

void MainForm::open()
{
    // Use last folder if possible.
    QString folderPath;
    if (!m_lastDiagramOpenFolder.isEmpty())
    {
        folderPath = m_lastDiagramOpenFolder;
    }
    else
    {
        folderPath = saveFileDir();
    }

    // Choose file to open.
    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Open Genealogy File"),
                                         folderPath,
                                         tr("Genealogy XML Files (*.xml)"));

    if (fileName.isEmpty())
        return;

    // Save the folder path.
    QFileInfo info(fileName);
    m_lastDiagramOpenFolder = info.dir().path();

    // Try to open the file.
    open(fileName);
}

void MainForm::save()
{
    // Select file if required.
    if (!saveFileExists()) {
        QString title = tr("Save Genealogy File");
        QString dir = saveFileDir();
        QString filter = tr("Genealogy XML Files (*.xml)");
        QString fileName = QFileDialog::getSaveFileName(this, title, dir, filter);

        if (fileName.isEmpty())
            return;

        if (!fileName.endsWith(".xml")) {
            fileName += ".xml";
        }

        // Store save file.
        m_saveFileName = fileName;
    }

    // Open file.
     QFile file(m_saveFileName);
     if (!file.open(QFile::WriteOnly | QFile::Text)) {
         QString message = tr("Cannot write file %1:\n%2.").arg(m_saveFileName).arg(file.errorString());
         QMessageBox::warning(this, tr("Genealogy Maker"), message);
         return;
     }

     // Save to file.
     scene->save(&file, FileUtils::getPhotosFolderFor(m_saveFileName));

     // Reset flag.
     m_gedcomWasImported = false;

     // Update window title.
     updateWindowTitle();

     // Mark undo stack.
     undoStack->setClean();

     // Update "Recent Files" menu.
     prependToRecentFiles(m_saveFileName);
}

void MainForm::saveAs()
{
    // Select file if required.
    QString title = tr("Save Genealogy File As");
    QString dir = saveFileDir();
    if (saveFileExists()) {
        dir = m_saveFileName;
    }
    QString filter = tr("Genealogy XML Files (*.xml)");
    QString fileName = QFileDialog::getSaveFileName(this, title, dir, filter);

    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".xml")) {
        fileName += ".xml";
    }

    // Open file.
     QFile file(fileName);
     if (!file.open(QFile::WriteOnly | QFile::Text)) {
         QString message = tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString());
         QMessageBox::warning(this, tr("Genealogy Maker"), message);
         return;
     }

     // Store save file.
     m_saveFileName = fileName;

     // Save to file.
     scene->save(&file, getPhotosFolderFor(m_saveFileName));

     // Set flag.
     m_gedcomWasImported = false;

     // Update window title.
     updateWindowTitle();

     // Mark undo stack.
     undoStack->setClean();

     // Update "Recent Files" menu.
     prependToRecentFiles(fileName);
}

void MainForm::onTreeItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    QUuid id = item->data(0, Qt::UserRole).toUuid();
    auto diagramItem = scene->itemWithId(id);
    if (diagramItem) {
        view->centerOn(diagramItem);
        scene->highlightForSearch(diagramItem);
    }
}

void MainForm::selectAll()
{
    scene->selectAll();
}

void MainForm::selectNone()
{
    scene->clearSelection();
}

void MainForm::print()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPaperSize(QPrinter::A4);

    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        //scene->render(&painter);
        QRectF destRect;
        int MARGIN = 10;
        QRectF sourceRect = scene->itemsBoundingRect().adjusted(-MARGIN, -MARGIN, MARGIN, MARGIN);
        scene->render(&painter, destRect, sourceRect);
    }
}

void MainForm::alignItemsHorizontally()
{
    QList<QGraphicsItem *> itemsToAlign;

    // Only apply to person items.
    for (auto item: scene->selectedItems())
    {
        if (item->type() == DiagramItem::Type) {
            itemsToAlign << item;
        }
    }

    auto count = itemsToAlign.count();
    if (count < 2) {
        return;
    }

    QPointF center;
    for (auto item: itemsToAlign) {
        center += item->pos();
    }

    center = center / count;

    MoveItemsUndo *undo = new MoveItemsUndo(scene, itemsToAlign);
    for (auto item: itemsToAlign) {
        item->setPos(item->pos().x(), center.y());
    }
    undo->storeAfterState();
    undo->setText("align items");
    undoStack->push(undo);
}

void MainForm::alignItemsVertically()
{
    QList<QGraphicsItem *> itemsToAlign;

    // Only apply to person items.
    for (auto item: scene->selectedItems())
    {
        if (item->type() == DiagramItem::Type) {
            itemsToAlign << item;
        }
    }

    auto count = itemsToAlign.count();
    if (count < 2) {
        return;
    }

    QPointF center;
    for (auto item: itemsToAlign) {
        center += item->pos();
    }

    center = center / count;

    MoveItemsUndo *undo = new MoveItemsUndo(scene, itemsToAlign);
    for (auto item: itemsToAlign) {
        item->setPos(center.x(), item->pos().y());
    }
    undo->storeAfterState();
    undo->setText("align items");
    undoStack->push(undo);
}

void MainForm::onMouseWheelZoomed()
{
    double scale = view->matrix().m11();
    double percent = scale * 100.0;
    QString text = QString::number((int)percent) + "%";
    sceneScaleCombo->setCurrentText(text);

    m_disableZoomSliderSignal = true;
    zoomSlider->setValue(percent);
    m_disableZoomSliderSignal = false;
}

void MainForm::onArrowAdded(Arrow *arrow)
{
    undoStack->push(new AddArrowUndo(scene, arrow));

    // Back to "pointer mode".
    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
    buttonGroup->button(InsertArrowButton)->setChecked(false);
}

void MainForm::onItemsAboutToMove()
{
    auto items = scene->selectedItems();
    if (!items.empty()) {
        moveItemsUndo = new MoveItemsUndo(scene, items);
    }
}

void MainForm::onItemsFinishedMoving()
{
    if (moveItemsUndo) {
        moveItemsUndo->storeAfterState();
        undoStack->push(moveItemsUndo);
        moveItemsUndo = nullptr;
    }
}

void MainForm::onFind()
{
    // Reset search index.
    m_searchFoundIndex = -1;

    // Show dialog.
    if (!dialogFind) {
        dialogFind = new DialogFind(this);
        connect(dialogFind, SIGNAL(search(QString)), this, SLOT(onSearch(QString)));
    }

    dialogFind->beforeShow();
    dialogFind->show();
}

void MainForm::onSearch(const QString &text)
{
    // Get items and count.
    auto items = scene->items();
    auto count = items.size();

    // Search.
    for (int i = m_searchFoundIndex + 1; i < count; ++i) {
        if (searchCheckPerson(items, i, text)) return;
    }

    // Wrap search.
    for (int i = 0; (i <= m_searchFoundIndex) && (i < count); ++i) {
        if (searchCheckPerson(items, i, text)) return;
    }

    // Show message if not found.
    if (dialogFind) {
        dialogFind->setStatus("Person not found.");
    }
}

/**
 * @brief MainForm::searchCheckPerson Check if the item matches the search text. If so,
 * go to the item and update the "Find" dialog.
 * @param items The list of items.
 * @param index The index of the item to check.
 * @param text The search text.
 * @return True if the item matches the text.
 */
bool MainForm::searchCheckPerson(QList<QGraphicsItem *> &items, int index, const QString &text)
{
    // Get item.
    auto item = items[index];

    // Check if person.
    if (item->type() == DiagramItem::Type) {
        auto diagramItem = qgraphicsitem_cast<DiagramItem *> (item);

        // Check if name matches.
        if (diagramItem->name().contains(text, Qt::CaseInsensitive)) {

            // Go to person in diagram.
            view->centerOn(diagramItem);

            // Highlight the person.
            scene->highlightForSearch(diagramItem);

            // Update dialog.
            dialogFind->setStatus("Person found.");
            dialogFind->onFound();

            // Save index for next search.
            m_searchFoundIndex = index;

            // Return.
            return true;
        }
    }

    // Return.
    return false;
}

/// Export the diagram as an image.
void MainForm::exportImage()
{
    // Previously used path.
//    static QString m_lastUsedExportPath;

    // Check that diagram is not empty.
    if (scene->isEmpty()) {
        QMessageBox::warning(this, "Problem Exporting Image", "The diagram is empty, so nothing was exported.");
        return;
    }

    // Get the filename.

    // Set default folder.
    QString defaultDir;

//    if (!m_lastUsedExportPath.isEmpty()) {
//        // Use previous export folder.
//        QFileInfo info(m_lastUsedExportPath);
//        defaultDir = info.dir().path();
//    }
//    else {
        // Use the save files directory.
//        defaultDir = saveFileDir();
//    }

    // Add default file-name.
//    if (!defaultDir.isEmpty()) {
//        defaultDir += "/export.png";
//    }

    // Just specify the filename.
    defaultDir = "export.png";

    // Show file chooser dialog.
    QString title = tr("Export Image");
    QString filter = tr("PNG Image Files (*.png)");
    QString fileName = QFileDialog::getSaveFileName(this, title, defaultDir, filter);

    // Check if user cancelled.
    if (fileName.isEmpty()) {
        return;
    }

    // Ensure file has correct suffix.
    if (!fileName.toLower().endsWith(".png")) {
        fileName += ".png";
    }

    // Get the render area.
    QRectF targetRect;
    QRectF sourceRect = scene->itemsBoundingRect();

    // Create the image.
    QSize imageSize = sourceRect.size().toSize();
    QImage image(imageSize, QImage::Format_ARGB32_Premultiplied);

    // Paint to the image.
    QPainter painter(&image);
    scene->render(&painter, targetRect, sourceRect);

    // Save the image.
    QImageWriter writer(fileName);
    bool exportOK = writer.write(image);

    // Show a message.
    if (exportOK) {
        QMessageBox::information(this, "Image Exported", "Image exported successfully.");
    }
    else {
        QMessageBox::warning(this, "Problem Exporting Image",
                                 tr("Cannot write %1: %2")
                             .arg(QDir::toNativeSeparators(fileName), writer.errorString()));
    }

    // Store path.
//    m_lastUsedExportPath = fileName;
}

void MainForm::createPersonListReport()
{
    ReportWindow *window = new ReportWindow(this);
    window->createReportFor(scene);
    window->show();
}

void MainForm::createTimelineReport()
{
    TimelineReportWindow *window = new TimelineReportWindow(this);
    window->createReportFor(scene);
    window->show();
}

void MainForm::showFileProperties()
{
    if (!dialogFileProperties) {
        dialogFileProperties = new DialogFileProperties(this);
    }
    dialogFileProperties->setFile(m_saveFileName);
    dialogFileProperties->setDiagram(scene);
    dialogFileProperties->show();
}

void MainForm::autoLayoutDiagram()
{
    // Create undo object.
    MoveItemsUndo *undo = new MoveItemsUndo(scene, scene->items());

    // Layout the scene.
    scene->autoLayout();

    // Store undo item.
    undo->storeAfterState();
    undo->setText("auto layout");
    undo->setMoveView(true);
    undoStack->push(undo);

    // Go to first item.
    if (!scene->isEmpty()) {
        view->centerOn(scene->firstItem());
    }
}

void MainForm::openExampleDiagram()
{
    // Show examples folder.
    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Open Example Genealogy"),
                                         exampleFileDir(),
                                         tr("Genealogy XML Files (*.xml)"));

    // Open the file if once was chosen.
    if (fileName.isEmpty())
        return;

    // Try to open the file.
    open(fileName);
}

QString MainForm::exampleFileDir() const
{
    return QDir("examples").path();
}

bool MainForm::shouldRemoveInvalidFiles() const
{
    QSettings settings;
    return settings.value("interface/removeInvalidFiles", false).toBool();
}

void MainForm::setSceneScale(double scale)
{
    QMatrix oldMatrix = view->matrix();
    view->resetMatrix();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(scale, scale);
}

///
/// \brief MainForm::selectCurrentItemDescendants Select descendants of the selected person.
///
void MainForm::selectCurrentItemDescendants()
{
    auto selectedItems = scene->selectedItems();
    if (selectedItems.isEmpty())
        return;

    auto item = selectedItems.first();
    if (item->type() == DiagramItem::Type) {
        auto person = qgraphicsitem_cast<DiagramItem *>(item);
        person->selectDescendants();
    }
}

void MainForm::viewSelectedItemDetails()
{
    auto selectedItems = scene->selectedItems();
    if (selectedItems.isEmpty())
        return;

    auto item = selectedItems.first();
    if (item->type() == DiagramItem::Type) {
        auto person = qgraphicsitem_cast<DiagramItem *>(item);
        viewPersonDetails(person);
    }
}

void MainForm::onPeopleMarried(DiagramItem *person1, DiagramItem *person2)
{
    undoStack->push(new MarriageUndo(scene, person1, person2));
}

void MainForm::removeMarriage()
{
    auto marriage = MarriageItem::getSelectedMarriage();
    marriage->personLeft()->removeMarriage();
    undoStack->push(new RemoveMarriageUndo(scene, marriage));
}

void MainForm::viewMarriageDetails()
{
    if (!dialogMarriageDetails) {
        dialogMarriageDetails = new DialogMarriageDetails(this);
    }
    dialogMarriageDetails->setMarriage(MarriageItem::getSelectedMarriage());
    dialogMarriageDetails->show();
}

void MainForm::onSceneCleared()
{
    treeItems.clear();
    tree->clear();
}

void MainForm::updateWindowTitle()
{
    // Application title.
    QString title = m_appName + " - ";

    // File name.
    if (saveFileExists())
    {
        QFileInfo info(m_saveFileName);
        title += info.fileName();
    }
    else
    {
        title += "New Diagram";

        if (m_gedcomWasImported)
        {
            title += " (Imported from GEDCOM)";
        }
    }

    // "Unsaved changes" indicator.
    if (!undoStack->isClean())
    {
        title += "*";
    }

//    title += " - ";
//    title += m_appName;

    setWindowTitle(title);
}

void MainForm::onUndoStackCleanChanged(bool clean)
{
    Q_UNUSED(clean);
    updateWindowTitle();
}

void MainForm::onPersonDoubleClicked(DiagramItem *person)
{
    viewPersonDetails(person);
}

void MainForm::onItemDragDropFinished()
{
    // Return to "pointer" mode.
    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));

    // Deselect "add item" buttons.
    buttonGroup->button(int(DiagramItem::Person))->setChecked(false);
}

void MainForm::updateGuiFromPreferences()
{
    // Load settings.
    QSettings settings;

    // Update sidebar collapse/expand button.
    bool value = settings.value("interface/showSidebarCollapseButton", false).toBool();
    collapseButton->setVisible(value);

    // Update "Find" window.
    if (dialogFind) {
        dialogFind->updateGuiFromPreferences();
    }
}

void MainForm::onPreferencesChanged()
{
    // Update diagram.
    scene->loadPreferences();

    // Update interface.
    updateGuiFromPreferences();
}

void MainForm::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainForm::moveToCenter()
{
    //
    // Center the window on the screen.
    //
    auto geometry = QApplication::desktop()->screenGeometry();
    auto halfSize = size() / 2.0;
    QPoint offset(halfSize.width(), halfSize.height());
    move(geometry.center() - offset);
}

void MainForm::onItemTextEdited(QGraphicsItem *item)
{
    if (m_beingDestroyed) return;

    auto parent = item->parentItem();

    if (parent) {
        auto person = qgraphicsitem_cast<DiagramItem *>(parent);
        if (person) {
            auto treeItem = treeItems[person->id()];
            if (treeItem) {
                treeItem->setText(0, person->name());
            }
        }
    }
}

void MainForm::createToolBox()
{
    //
    // "Add Item" tab.
    //
    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(false);
    connect(buttonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(buttonGroupClicked(int)));
    QGridLayout *layout = new QGridLayout;

    //
    // "Person" button
    //
    layout->addWidget(createCellWidget(tr("Person"), DiagramItem::Person, tr("P")), 0, 0);

    //
    // "Relationship" button
    //
    arrowButton = new QToolButton;
    arrowButton->setCheckable(true);
    buttonGroup->addButton(arrowButton, InsertArrowButton);
    arrowButton->setIcon(QIcon(QPixmap(":/images/linepointer.png")));
    arrowButton->setIconSize(QSize(50, 50));
    arrowButton->setToolTip("Add Parent/Child Relationship (R)");
    arrowButton->setShortcut(tr("R"));
    styleToolButton(arrowButton);
    QGridLayout *textLayout = new QGridLayout;
    textLayout->addWidget(arrowButton, 0, 0, Qt::AlignHCenter);
    textLayout->addWidget(new QLabel(tr("Parent/Child\nRelationship")), 1, 0, Qt::AlignCenter);
    QWidget *textWidget = new QWidget;
    textWidget->setLayout(textLayout);
    layout->addWidget(textWidget, 0, 1);

    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);

    //
    // Background tab.
    //
    backgroundButtonGroup = new QButtonGroup(this);
    connect(backgroundButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(backgroundButtonGroupClicked(QAbstractButton*)));

    QGridLayout *backgroundLayout = new QGridLayout;
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Blue Grid"),
                                                           ":/images/background1.png"), 0, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("White Grid"),
                                                           ":/images/background2.png"), 0, 1);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Gray Grid"),
                                                           ":/images/background3.png"), 1, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("No Grid"),
                                                           ":/images/background4.png"), 1, 1);

    backgroundLayout->setRowStretch(2, 10);
    backgroundLayout->setColumnStretch(2, 10);

    QWidget *backgroundWidget = new QWidget;
    backgroundWidget->setLayout(backgroundLayout);

    //
    // Tree view.
    //
    QGridLayout *treeViewLayout = new QGridLayout;
    tree = new QTreeWidget(this);
    tree->setHeaderLabel("Person");
    tree->setSortingEnabled(true);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTreeViewContextMenuRequested(QPoint)));
    connect(tree, &QTreeWidget::itemDoubleClicked, this, &MainForm::onTreeItemDoubleClicked);
    treeViewLayout->addWidget(tree);
    QWidget *treeViewWidget = new QWidget;
    treeViewWidget->setLayout(treeViewLayout);
    tree->setObjectName("treeViewPersons");

    toolBox = new QToolBox;
    toolBox->setObjectName("toolBox");;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox->addItem(itemWidget, tr("Add Items"));
    toolBox->addItem(backgroundWidget, tr("Backgrounds"));
    toolBox->addItem(treeViewWidget, tr("Tree View"));
}

void MainForm::createActions()
{
//    findAction = new QAction(tr("Find..."), this);
//    findAction->setShortcut(QKeySequence::Find);
//    findAction->setStatusTip(tr("Find person with name"));
    connect(ui->findAction, SIGNAL(triggered()), this, SLOT(onFind()));

//    toFrontAction = new QAction(QIcon(":/images/bringtofront.png"),
//                                tr("Bring to &Front"), this);
//    toFrontAction->setShortcut(tr("Ctrl+Shift+F"));
//    toFrontAction->setStatusTip(tr("Bring item to front"));
    connect(ui->toFrontAction, SIGNAL(triggered()), this, SLOT(bringToFront()));

//    sendBackAction = new QAction(QIcon(":/images/sendtoback.png"), tr("Send to &Back"), this);
//    sendBackAction->setShortcut(tr("Ctrl+T"));
//    sendBackAction->setStatusTip(tr("Send item to back"));
    connect(ui->sendBackAction, SIGNAL(triggered()), this, SLOT(sendToBack()));

//    deleteAction = new QAction(QIcon(":/images/delete.png"), tr("&Delete"), this);
//    deleteAction->setShortcut(tr("Delete"));
//    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(ui->deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

//    exitAction = new QAction(tr("E&xit"), this);
//    exitAction->setShortcuts(QKeySequence::Quit);
//    exitAction->setStatusTip(tr("Quit the application"));
    connect(ui->exitAction, SIGNAL(triggered()), this, SLOT(close()));

//    newAction = new QAction(tr("New"), this);
//    newAction->setShortcuts(QKeySequence::New);
//    newAction->setStatusTip(tr("Start a new diagram"));
    connect(ui->newAction, SIGNAL(triggered()), this, SLOT(newDiagram()));

//    openAction = new QAction(tr("Open"), this);
//    openAction->setShortcuts(QKeySequence::Open);
//    openAction->setStatusTip(tr("Open diagram"));
    connect(ui->openAction, SIGNAL(triggered()), this, SLOT(open()));

//    saveAction = new QAction(tr("Save"), this);
//    saveAction->setShortcuts(QKeySequence::Save);
//    saveAction->setStatusTip(tr("Save diagram"));
    connect(ui->saveAction, SIGNAL(triggered()), this, SLOT(save()));
//    saveAsAction = new QAction(tr("Save As..."), this);
//    saveAsAction->setShortcuts(QKeySequence::SaveAs);
//    saveAsAction->setStatusTip(tr("Save diagram as a new file"));
    connect(ui->saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

//    printAction = new QAction(tr("Print..."), this);
//    printAction->setShortcuts(QKeySequence::Print);
//    printAction->setStatusTip(tr("Print diagram"));
    connect(ui->printAction, SIGNAL(triggered()), this, SLOT(print()));

    undoAction = undoStack->createUndoAction(this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setObjectName("undoAction");
    redoAction = undoStack->createRedoAction(this);
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setObjectName("redoAction");

//    boldAction = new QAction(tr("Bold"), this);
//    boldAction->setCheckable(true);
//    QPixmap pixmap(":/images/bold.png");
//    boldAction->setIcon(QIcon(pixmap));
//    boldAction->setShortcut(tr("Ctrl+B"));
//    connect(boldAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

//    italicAction = new QAction(QIcon(":/images/italic.png"), tr("Italic"), this);
//    italicAction->setCheckable(true);
//    italicAction->setShortcut(tr("Ctrl+I"));
//    connect(italicAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

//    underlineAction = new QAction(QIcon(":/images/underline.png"), tr("Underline"), this);
//    underlineAction->setCheckable(true);
//    underlineAction->setShortcut(tr("Ctrl+U"));
//    connect(underlineAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

//    aboutAction = new QAction(tr("A&bout"), this);
//    aboutAction->setShortcut(tr("F1"));
    connect(ui->aboutAction, SIGNAL(triggered()), this, SLOT(about()));

//    selectAllAction = new QAction(tr("Select All"), this);
//    selectAllAction->setShortcut(tr("Ctrl+A"));
    connect(ui->selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));

//    selectNoneAction = new QAction(tr("Select None"), this);
//    selectNoneAction->setShortcut(tr("Ctrl+Shift+A"));
    connect(ui->selectNoneAction, SIGNAL(triggered()), this, SLOT(selectNone()));

//    alignHorizontallyAction = new QAction(tr("Align Horizontally"), this);
    connect(ui->alignHorizontallyAction, SIGNAL(triggered()), this, SLOT(alignItemsHorizontally()));

//    alignVerticallyAction = new QAction(tr("Align Vertically"), this);
    connect(ui->alignVerticallyAction, SIGNAL(triggered()), this, SLOT(alignItemsVertically()));

//    viewDetailsAction = new QAction(tr("Details..."), this);
//    viewDetailsAction->setShortcut(tr("Ctrl+D"));
    connect(ui->viewDetailsAction, SIGNAL(triggered()), this, SLOT(viewSelectedItemDetails()));

    removeMarriageAction = new QAction(QIcon(":/images/delete.png"), tr("Remove Marriage"), this);
    connect(removeMarriageAction, SIGNAL(triggered()), this, SLOT(removeMarriage()));

    marriageDetailsAction = new QAction(tr("Marriage Details..."), this);
    connect(marriageDetailsAction, SIGNAL(triggered()), this, SLOT(viewMarriageDetails()));

    connect(ui->helpContentsAction, SIGNAL(triggered()), this, SLOT(showHelpContents()));
}

void MainForm::createMenus()
{
//    fileMenu = menuBar()->addMenu(tr("&File"));
//    fileMenu->addAction(newAction);
//    fileMenu->addAction(openAction);
//    fileMenu->addAction(saveAction);
//    fileMenu->addAction(saveAsAction);
//    fileMenu->addSeparator();
//    fileMenu->addAction(printAction);
//    fileMenu->addSeparator();
//    fileMenu->addAction(exitAction);

//    editMenu = menuBar()->addMenu(tr("&Edit"));
//    editMenu->addAction(undoAction);
//    editMenu->addAction(redoAction);
//    editMenu->addSeparator();
//    editMenu->addAction(findAction);

    // Replace the placeholder undo items.
    undoAction->setIcon(ui->actionUndo->icon());
    redoAction->setIcon(ui->actionRedo->icon());
    ui->menuEdit->insertAction(ui->actionUndo, undoAction);
    ui->menuEdit->insertAction(ui->actionRedo, redoAction);
    ui->menuEdit->removeAction(ui->actionUndo);
    ui->menuEdit->removeAction(ui->actionRedo);

//    itemMenu = menuBar()->addMenu(tr("&Item"));
//    itemMenu->addAction(deleteAction);
//    itemMenu->addSeparator();
//    itemMenu->addAction(toFrontAction);
//    itemMenu->addAction(sendBackAction);
//    itemMenu->addSeparator();
//    itemMenu->addAction(viewDetailsAction);

//    selectMenu = menuBar()->addMenu(tr("&Select"));
//    selectMenu->addAction(selectAllAction);
//    selectMenu->addAction(selectNoneAction);

//    alignMenu = menuBar()->addMenu(tr("&Align"));
//    alignMenu->addAction(alignHorizontallyAction);
//    alignMenu->addAction(alignVerticallyAction);

//    aboutMenu = menuBar()->addMenu(tr("&Help"));
//    aboutMenu->addAction(aboutAction);

    marriageMenu = new QMenu("Marriage", this);
    marriageMenu->addAction(removeMarriageAction);
    marriageMenu->addAction(marriageDetailsAction);
    MarriageItem::setContextMenu(marriageMenu);

    // Create context menu for tree-view.
    QAction *viewDetailsAction = new QAction(tr("&Details..."), this);
    viewDetailsAction->setStatusTip(tr("View person details"));
    connect(viewDetailsAction, SIGNAL(triggered()), this, SLOT(onTreeViewDetailsAction()));

    treeViewContextMenu = new QMenu(this);
    treeViewContextMenu->addAction(viewDetailsAction);
}

void MainForm::createToolbars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(ui->newAction);
    fileToolBar->addAction(ui->openAction);
    fileToolBar->addAction(ui->saveAction);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(ui->deleteAction);
//    editToolBar->addAction(ui->toFrontAction);
//    editToolBar->addAction(ui->sendBackAction);

    alignmentToolBar = addToolBar(tr("Alignment"));
    alignmentToolBar->addAction(ui->alignHorizontallyAction);
    alignmentToolBar->addAction(ui->alignVerticallyAction);

//    fontCombo = new QFontComboBox();
//    connect(fontCombo, SIGNAL(currentFontChanged(QFont)),
//            this, SLOT(currentFontChanged(QFont)));

//    fontSizeCombo = new QComboBox;
//    fontSizeCombo->setEditable(true);
//    for (int i = 8; i < 30; i = i + 2)
//        fontSizeCombo->addItem(QString().setNum(i));
//    QIntValidator *validator = new QIntValidator(2, 64, this);
//    fontSizeCombo->setValidator(validator);
//    connect(fontSizeCombo, SIGNAL(currentIndexChanged(QString)),
//            this, SLOT(fontSizeChanged(QString)));

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
    textAction = fontColorToolButton->menu()->defaultAction();
    fontColorToolButton->setIcon(createColorToolButtonIcon(":/images/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    fontColorToolButton->setToolTip("Set text color");
    connect(fontColorToolButton, SIGNAL(clicked()),
            this, SLOT(textButtonTriggered()));

    fillColorToolButton = new QToolButton;
    fillColorToolButton->setObjectName("fillColorToolButton");
//    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
//    fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
//    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/floodfill.png", Qt::white));
    fillColorToolButton->setToolTip("Set fill color");
    connect(fillColorToolButton, SIGNAL(clicked()),
            this, SLOT(fillButtonTriggered()));

    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/linecolor.png", Qt::black));
    lineColorToolButton->setToolTip("Set line color");
    connect(lineColorToolButton, SIGNAL(clicked()),
            this, SLOT(lineButtonTriggered()));

//    textToolBar = addToolBar(tr("Font"));
//    textToolBar->addWidget(fontCombo);
//    textToolBar->addWidget(fontSizeCombo);
//    textToolBar->addAction(boldAction);
//    textToolBar->addAction(italicAction);
//    textToolBar->addAction(underlineAction);

    colorToolBar = addToolBar(tr("Color"));
    colorToolBar->addWidget(fontColorToolButton);
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);

    pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setShortcut(tr(" "));
    pointerButton->setIcon(QIcon(":/images/pointer.png"));
    linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/linepointer.png"));

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::MoveItem));
    pointerTypeGroup->addButton(linePointerButton, int(DiagramScene::InsertLine));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));

    // Define zoom limits.
    const int ZOOM_MIN = 10;
    const int ZOOM_MAX = 250;

    sceneScaleCombo = new QComboBox;
    sceneScaleCombo->setObjectName("zoomComboBox");
    QStringList scales;
    scales << tr("10%")
           << tr("25%")
           << tr("50%")
           << tr("75%")
           << tr("100%")
           << tr("150%")
           << tr("200%")
           << tr("250%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(sceneScaleCombo->findText(tr("100%")));
    connect(sceneScaleCombo, SIGNAL(activated(QString)),
            this, SLOT(sceneScaleActivated(QString)));
    sceneScaleCombo->setEditable(true);
    sceneScaleCombo->setValidator(new PercentValidator(ZOOM_MIN, ZOOM_MAX, this));
    sceneScaleCombo->setInsertPolicy(QComboBox::NoInsert);
    connect(sceneScaleCombo->lineEdit(), SIGNAL(editingFinished()),
            this, SLOT(sceneScaleEditingFinished()));
    connect(sceneScaleCombo->lineEdit(), SIGNAL(textEdited(QString)),
            this, SLOT(sceneScaleTextEdited(QString)));

    // Add slider for zoom.
    zoomSlider = new QSlider(Qt::Horizontal);
    zoomSlider->setObjectName("zoomSlider");
    zoomSlider->setMaximumWidth(100);
    zoomSlider->setRange(ZOOM_MIN, ZOOM_MAX);
    zoomSlider->setValue(100);
    connect(zoomSlider, SIGNAL(valueChanged(int)),
            this, SLOT(zoomSliderValueChanged(int)));

    // Allow pointer button to be deselected.
    pointerTypeGroup->setExclusive(false);

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
//    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);
    pointerToolbar->addWidget(zoomSlider);
    pointerToolbar->setStyleSheet("QToolBar { spacing: 8px; }");
}

QWidget *MainForm::createBackgroundCellWidget(const QString &text, const QString &image)
{
    QToolButton *button = new QToolButton;
    button->setText(text);
    button->setIcon(QIcon(image));
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    backgroundButtonGroup->addButton(button);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

void MainForm::styleToolButton(QToolButton *button) const
{
    QString styleSheet = button->styleSheet();
    QString extraStyle =
            "QToolButton:checked {"
                "border: 4px solid green;"
            "}";
    styleSheet += extraStyle;
    button->setStyleSheet(styleSheet);
}

QString MainForm::getPhotosFolderFor(const QString &fileName) const
{
    return FileUtils::getPhotosFolderFor(fileName);
}

void MainForm::open(const QString &fileName)
{
    // Ask whether to save unsaved changes.
    if (!maybeSave()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Genealogy Maker"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        if (m_openingRecentFile && shouldRemoveInvalidFiles()) {
            removeFromRecentFiles(fileName);
        }
        return;
    }

    bool openedOK = scene->open(&file, getPhotosFolderFor(fileName));

    // Exit if not opened OK.
    if (!openedOK) {
//        if (m_openingRecentFile) {
//            removeFromRecentFiles(fileName);
//        }
        return;
    }

    // Scroll to first item.
    if (!scene->isEmpty()) {
        view->centerOn(scene->firstItem());
    }

    // Store save file.
    m_saveFileName = fileName;

    // Update window title.
    updateWindowTitle();

    // Clear undo stack.
    undoStack->clear();
    undoStack->setClean();

    // Update "Recent Files" menu.
    prependToRecentFiles(fileName);
}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

void MainForm::updateRecentFileActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList recentFiles = readRecentFiles(settings);
    const int count = qMin(int(MaxRecentFiles), recentFiles.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
        recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
        recentFileActs[i]->setData(recentFiles.at(i));
        recentFileActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentFiles; ++i)
        recentFileActs[i]->setVisible(false);
}

void MainForm::openRecentFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
    {
        m_openingRecentFile = true;
        open(action->data().toString());
        m_openingRecentFile = false;
    }
}

void MainForm::clearRecentFilesMenu()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Confirm");
    msgBox.setText("Really clear the recent files menu?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes) {
        // Remove all entries.
        QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
        settings.beginWriteArray(recentFilesKey());
        settings.endArray();

        // Hide the menu.
        setRecentFilesVisible(false);
    }
}

bool MainForm::hasRecentFiles()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();
    return count > 0;
}

void MainForm::prependToRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainForm::setRecentFilesVisible(bool visible)
{
    recentFileSubMenuAct->setVisible(visible);
}

void MainForm::removeFromRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

QWidget *MainForm::createCellWidget(const QString &text, DiagramItem::DiagramType type, QKeySequence shortcut)
{
    DiagramItem item(type, ui->itemMenu);
    QIcon icon(item.image());

//    QToolButton *button = new QToolButton;
    DraggableButton *button = new DraggableButton(nullptr);
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    button->setToolTip(QString("Add ") + text + " (" + shortcut.toString() + ")");
    button->setShortcut(shortcut);
    buttonGroup->addButton(button, int(type));
    connect(button, SIGNAL(dragDropFinished()), this, SLOT(onItemDragDropFinished()));
    button->setObjectName(QString("buttonAdd") + text);

    // Set button style to indicate clearly when it is active.
    styleToolButton(button);

    // Add button to layout.
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

QMenu *MainForm::createColorMenu(const char *slot, QColor defaultColor)
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue")
          << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor)
            colorMenu->setDefaultAction(action);
    }
    return colorMenu;
}

QIcon MainForm::createColorToolButtonIcon(const QString &imageFile, QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    // Draw icon centred horizontally on button.
    QRect target(4, 0, 42, 43);
    QRect source(0, 0, 42, 43);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}

QIcon MainForm::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}

QString MainForm::saveFileDir()
{
    // Ensure directory exists.
    QString dirName = "save-files";
    QDir().mkpath(dirName);

    // Return path.
    return QDir(dirName).path();
}

bool MainForm::saveFileExists() const
{
    return !m_saveFileName.isEmpty();
}

bool MainForm::maybeSave()
{
    if (!undoStack->isClean()) {
        QString message = tr("The diagram has been modified.\n"
                             "Do you want to save your changes?");
        auto ret = QMessageBox::warning(this, m_appName, message,
                                        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        switch (ret) {
        case QMessageBox::Save:
            save();
            return true;
        case QMessageBox::Cancel:
            return false;
        default:
            break;
        }
    }
    return true;
}

void MainForm::viewPersonDetails(DiagramItem *person)
{
    if (!dialogPersonDetails) {
        dialogPersonDetails = new DialogPersonDetails(this);
    }
    dialogPersonDetails->setItem(person);
    dialogPersonDetails->setXmlFile(m_saveFileName);
    dialogPersonDetails->viewDefaultTab();
    dialogPersonDetails->show();
}

void MainForm::addPythonPath(QProcess *process) const
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    // Add Python library path if required.
    QString key = "PYTHONPATH";
    if (!env.contains(key)) {
        env.insert(key, getPythonPath());
    }

    process->setProcessEnvironment(env);
}

QString MainForm::getPythonPath() const
{
    // Default path.
    QString pythonPath = "C:\\Python27\\Lib";

    // Run "where" to find the Python path
    QString command("where");
    QStringList params = QStringList() << "python";

    // Create the process.
    QProcess *process = new QProcess();

    // Start the command.
    process->start(command, params);
    bool successful = process->waitForFinished(1000);

    if (successful) {
        // Read the path.
        QString output(process->readAllStandardOutput());
        QStringList lines = output.split("\n");
        if (!lines.isEmpty()) {
            pythonPath = lines.first().trimmed().replace("python.exe", "Lib");
        }
    }
    else {
        qDebug() << "Could not run 'where' to find Python path.";
    }

    // End the process.
    process->close();

    // Return the path.
    return pythonPath;
}

DiagramScene *MainForm::getScene() const
{
    return scene;
}

QGraphicsView *MainForm::getView() const
{
    return view;
}
//! [32]

void MainForm::showHelpContents()
{
//    if (!dialogHelp) {
//        dialogHelp = new DialogHelp(this);
//    }

//    dialogHelp->show();

    if (!helpWindow) {
        helpWindow = new HelpWindow(this);
    }

    helpWindow->show();
}

void MainForm::on_actionChangeSize_triggered()
{
    // Show dialog.
    auto dialog = new DialogChangeSize(this);
    dialog->setCurrentSize(scene->sceneRect().size());
    int result = dialog->exec();

    // Process result.
    if (result == QDialog::Accepted) {
        UndoManager::add(new ChangeDiagramSizeUndo(scene, dialog->getNewWidth(), dialog->getNewHeight()));
        scene->setSceneRect(0, 0, dialog->getNewWidth(), dialog->getNewHeight());
    }
}

void MainForm::on_actionPreferences_triggered()
{
    if (!preferencesWindow) {
        preferencesWindow = new PreferencesWindow(this);
        connect(preferencesWindow, SIGNAL(preferencesChanged()), this, SLOT(onPreferencesChanged()));
    }

    preferencesWindow->loadPreferences();
    preferencesWindow->show();
}

void MainForm::on_actionImportGedcomFile_triggered()
{
    // Ask whether to save unsaved changes.
    if (!maybeSave()) {
        return;
    }

    // Use last folder if possible.
    QString folderPath;
    if (!m_lastGedcomImportFolder.isEmpty())
    {
        folderPath = m_lastGedcomImportFolder;
    }
    else
    {
        folderPath = saveFileDir();
    }

    // Show the "open file" dialog.
    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Import GEDCOM File"),
                                         folderPath,
                                         tr("GEDCOM Files (*.ged)"));

    // Exit if no file was chosen.
    if (fileName.isEmpty())
        return;

    // Save the folder path.
    QFileInfo info(fileName);
    m_lastGedcomImportFolder = info.dir().path();

    // Open file.
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Genealogy Maker"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    // Create progress dialog.
    QProgressDialog progress("Importing GEDCOM File...", "Cancel", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setLabelText("Setting up command...");
    progress.show();
    qApp->processEvents();

    // Transform the file using Python.
//    QString path = "/home/martin/lib/python-gedcom-1.0.0/";
    QString path = "python-gedcom-1.0.0/";
    QString command("python");
    QString outputFileName("imported.xml");
    QFileInfo outuptFileInfo(outputFileName);
    QString outputFilePath = outuptFileInfo.absoluteFilePath();
    QStringList params = QStringList() << "import_gedcom.py" << fileName << outputFilePath;

    // Create the process.
    QProcess *process = new QProcess();
    process->setWorkingDirectory(path);

    // Hack to set Python library path.
#ifdef Q_OS_WIN
    progress.setValue(10);
    progress.setLabelText("Ensuring Python path exists...");
    qApp->processEvents();

    addPythonPath(process);
#endif

    // Start the command.
    progress.setValue(20);
    progress.setLabelText("Running conversion script...");
    qApp->processEvents();

    process->start(command, params);
    bool successful = process->waitForFinished(3000);

    if (!successful) {
        // Warn the user if the import failed.
        QString title = tr("Import Error");
        QString message = tr("<p>Could not import GEDCOM file, due to the following error:<br/>");
        message += tr("%1</p>").arg(process->errorString());

        // Advise about downloading Python.
//        if (process->error() == QProcess::FailedToStart) {
            message += tr("<p>The program requires Python to import GEDCOM files. "
                    "You can download this from "
                    "<a href=\"https://www.python.org/downloads/\">https://www.python.org/downloads/</a>.</p>");
//        }

        // Show the warning box.
        QMessageBox msgBox(QMessageBox::Warning, title, message, QMessageBox::Ok, this);
        msgBox.setTextFormat(Qt::RichText); // Make the link clickable.
        msgBox.exec();

        // Close the process and exit.
        process->close();
        return;
    }

    process->waitForFinished();

    // Debug.
//    qDebug() << "Std Error:" << process->readAllStandardError();
//    qDebug() << "Std Output:" << process->readAllStandardOutput();

    process->close();

    // Load the transformed file.
    progress.setValue(50);
    progress.setLabelText("Opening transformed XML file...");
    qApp->processEvents();

    QFile outputXmlFile(outputFilePath);
    scene->open(&outputXmlFile, getPhotosFolderFor(outputFilePath));
    scene->autoLayout();

    // Scroll to first item.
    if (!scene->isEmpty()) {
        view->centerOn(scene->firstItem());
    }

    // Reset save file.
    m_saveFileName = "";

    // Set flag.
    m_gedcomWasImported = true;

    // Update window title.
    updateWindowTitle();

    // Clear undo stack.
    undoStack->clear();
    undoStack->setClean();
}

void MainForm::on_actionExportGedcomFile_triggered()
{
    // Select output file.
    QString outputFileName = "exported.ged";

    QString title = tr("Choose Output File");
    QString dir = saveFileDir();
    if (saveFileExists()) {
        // Set default file name.
        QFileInfo info(m_saveFileName);
        dir = info.absoluteDir().filePath(info.baseName());
    }
    QString filter = tr("GEDCOM Files (*.ged)");
    outputFileName = QFileDialog::getSaveFileName(this, title, dir, filter);

    if (outputFileName.isEmpty())
        return;

    if (!outputFileName.endsWith(".ged")) {
        outputFileName += ".ged";
    }

    // Open file for writing.
    QFile file(outputFileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QString message = tr("Cannot write file %1:\n%2.").arg(m_saveFileName).arg(file.errorString());
        QMessageBox::warning(this, tr("Genealogy Maker"), message);
        return;
    }

    // Save a copy of the diagram.
    //if (hasUnsavedChanges()) {
    QTemporaryFile inputXmlFile;
    if (inputXmlFile.open()) {
        scene->save(&inputXmlFile, "");
        inputXmlFile.close();
    }
    else {
        QString message = tr("Cannot save temporary XML file %1:\n%2.")
                .arg(inputXmlFile.fileName()).arg(inputXmlFile.errorString());
        QMessageBox::warning(this, tr("Genealogy Maker"), message);
        return;
    }
    //}

    // Create progress dialog.
    QProgressDialog progress("Exporting GEDCOM File...", "Cancel", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setLabelText("Setting up command...");
    progress.show();
    qApp->processEvents();

    // Transform the file using Python.
    QString path = "python-gedcom-1.0.0/";
    QString command("python");
    QFileInfo outuptFileInfo(outputFileName);
    QString outputFilePath = outuptFileInfo.absoluteFilePath();
    QStringList params = QStringList() << "export_gedcom.py" << inputXmlFile.fileName() << outputFilePath;

    // Create the process.
    QProcess *process = new QProcess();
    process->setWorkingDirectory(path);

    // Hack to set Python library path.
#ifdef Q_OS_WIN
    progress.setValue(10);
    progress.setLabelText("Ensuring Python path exists...");
    qApp->processEvents();

    addPythonPath(process);
#endif

    // Start the command.
    progress.setValue(20);
    progress.setLabelText("Running conversion script...");
    qApp->processEvents();

    process->start(command, params);
    bool successful = process->waitForFinished(3000);

    if (!successful) {
        // Warn the user if the import failed.
        QString title = tr("Export Error");
        QString message = tr("<p>Could not export GEDCOM file, due to the following error:<br/>");
        message += tr("%1</p>").arg(process->errorString());

        // Advise about downloading Python.
//        if (process->error() == QProcess::FailedToStart) {
            message += tr("<p>The program requires Python to export GEDCOM files. "
                    "You can download this from "
                    "<a href=\"https://www.python.org/downloads/\">https://www.python.org/downloads/</a>.</p>");
//        }

        // Show the warning box.
        QMessageBox msgBox(QMessageBox::Warning, title, message, QMessageBox::Ok, this);
        msgBox.setTextFormat(Qt::RichText); // Make the link clickable.
        msgBox.exec();

        // Close the process and exit.
        process->close();
        return;
    }

    process->waitForFinished();

    // Store error output.
    QString errorOutput(process->readAllStandardError());

    process->close();

    // Check exit code.
    int exitCode = process->exitCode();
    if (exitCode != 0) {
        // There was an error.
        QString title = tr("Problem Exporting GEDCOM");
        QString message = tr("There was a problem exporting the GEDCOM file:\n\n");
        message += errorOutput;
        QMessageBox msgBox(QMessageBox::Warning, title, message, QMessageBox::Ok, this);
        msgBox.exec();
        return;
    }

    // Finish.
    progress.setValue(100);
    progress.setLabelText("Done.");
    qApp->processEvents();
}

void MainForm::onCollapseButtonClicked(bool checked)
{
    Q_UNUSED(checked);

    // Toggle show/hide the toolbox.
    toolBox->setVisible(!toolBox->isVisible());

    // Update button text.
    if (toolBox->isVisible()) {
        collapseButton->setText("<<");
    }
    else {
        collapseButton->setText(">>");
    }

    // Update action.
    ui->showSideBarAction->setChecked(checked);
}

void MainForm::onTreeViewContextMenuRequested(const QPoint &pos)
{
    // Get item under mouse.
    QTreeWidgetItem *item = tree->itemAt(pos);

    // Do not show menu if no item under mouse.
    if (!item)
    {
        return;
    }

    // Store item.
    treeFocusedItem = item;

    // Show menu.
    treeViewContextMenu->exec( tree->mapToGlobal(pos) );
}

void MainForm::onTreeViewDetailsAction()
{
    // Show person details.
    QUuid id = treeFocusedItem->data(0, Qt::UserRole).toUuid();
    auto diagramItem = scene->itemWithId(id);
    if (diagramItem) {
        viewPersonDetails(diagramItem);
    }
    else {
        qDebug() << "Could not find person with id:" << id;
    }
}

void MainForm::on_actionExportImage_triggered()
{
    exportImage();
}

void MainForm::on_actionPersonListReport_triggered()
{
    createPersonListReport();
}

void MainForm::on_actionTimelineReport_triggered()
{
    createTimelineReport();
}

void MainForm::on_actionFileProperties_triggered()
{
    showFileProperties();
}

void MainForm::on_actionAutoLayout_triggered()
{
    autoLayoutDiagram();
}

void MainForm::on_actionOpenExample_triggered()
{
    openExampleDiagram();
}

void MainForm::on_actionSelectDescendants_triggered()
{
    selectCurrentItemDescendants();
}

void MainForm::on_showSideBarAction_triggered()
{
    onCollapseButtonClicked(ui->showSideBarAction->isChecked());
}

void MainForm::on_actionFileExportImage_triggered()
{
    exportImage();
}
