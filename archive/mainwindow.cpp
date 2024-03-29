/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"

#include "arrow.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
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

#include <QtWidgets>
#include <QPrinter>
#include <QPrintDialog>
#include <QUndoStack>

const int InsertArrowButton = 11;

#include <QDesktopWidget>

MainWindow::MainWindow()
{
    undoStack = new QUndoStack(this);
    connect(undoStack, SIGNAL(cleanChanged(bool)), this, SLOT(onUndoStackCleanChanged(bool)));
    m_appName = "Genealogy Maker Qt";

    createActions();
    createToolBox();
    createMenus();

    scene = new DiagramScene(itemMenu, this);
    scene->setSceneRect(QRectF(0, 0, 5000, 5000));
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
    view = new MyGraphicsView(scene);
    connect(scene, SIGNAL(mouseReleased()), view, SLOT(onMouseReleased()));
    connect(scene, SIGNAL(arrowAdded(Arrow*)), this, SLOT(onArrowAdded(Arrow*)));
    connect(scene, SIGNAL(itemsAboutToMove()), this, SLOT(onItemsAboutToMove()));
    connect(scene, SIGNAL(itemsFinishedMoving()), this, SLOT(onItemsFinishedMoving()));
    connect(scene, SIGNAL(cleared()), this, SLOT(onSceneCleared()));
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
}

void MainWindow::backgroundButtonGroupClicked(QAbstractButton *button)
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

void MainWindow::buttonGroupClicked(int id)
{
    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    foreach (QAbstractButton *button, buttons) {
        button->setChecked(buttonGroup->button(id) == button);
    }
    if (id == InsertArrowButton)  {
        linePointerButton->click();
    }
    else {
        scene->setItemType(DiagramItem::DiagramType(id));
        scene->setMode(DiagramScene::InsertItem);
    }
}

void MainWindow::deleteItem()
{
    QList<QGraphicsItem *> itemsRemoved;

    foreach (QGraphicsItem *item, scene->selectedItems()) {
        if (item->type() == Arrow::Type) {
            scene->removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            arrow->startItem()->removeArrow(arrow);
            arrow->endItem()->removeArrow(arrow);
            itemsRemoved << arrow;
        }
    }

    foreach (QGraphicsItem *item, scene->selectedItems()) {
        if (item->type() == DiagramItem::Type) {
            DiagramItem *diagramItem = qgraphicsitem_cast<DiagramItem *> (item);
            diagramItem->removeArrows();
            delete treeItems[diagramItem->id()];
        }
        scene->removeItem(item);
        itemsRemoved << item;
    }

    undoStack->push(new DeleteItemsUndo(scene, itemsRemoved));
}

void MainWindow::pointerGroupClicked(int)
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));

    // Update button group.
    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    foreach (QAbstractButton *button, buttons) {
        button->setChecked(false);
    }
    if (pointerTypeGroup->checkedButton() == linePointerButton) {
        arrowButton->setChecked(true);
    }
}

void MainWindow::bringToFront()
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

void MainWindow::sendToBack()
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

void MainWindow::itemInserted(DiagramItem *item, bool fromLoad)
{
    // Add item to list view.
    auto id = item->id();
    QStringList list;
    list << item->name();
    auto treeItem = new QTreeWidgetItem(list);
    tree->addTopLevelItem(treeItem);
    treeItem->setData(0, Qt::UserRole, id);
    treeItems[id] = treeItem;
    connect(item->textItem(), &DiagramTextItem::textEdited, this, &MainWindow::onItemTextEdited);

    if (!fromLoad)
    {
        // Update widgets.
        pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
        scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
        buttonGroup->button(int(item->diagramType()))->setChecked(false);

        // Update undo stack.
        undoStack->push(new AddItemUndo(scene, item));
    }
}

void MainWindow::onItemRemoved(DiagramItem *item)
{
    delete treeItems[item->id()];
}

void MainWindow::currentFontChanged(const QFont &)
{
    handleFontChange();
}

void MainWindow::fontSizeChanged(const QString &)
{
    handleFontChange();
}

void MainWindow::sceneScaleActivated(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QMatrix oldMatrix = view->matrix();
    view->resetMatrix();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(newScale, newScale);
}

void MainWindow::sceneScaleEditingFinished()
{
    if (scaleTextEditedByUser) {
        sceneScaleActivated(sceneScaleCombo->currentText());
    }
    scaleTextEditedByUser = false;
}

void MainWindow::sceneScaleTextEdited(const QString &scale)
{
    Q_UNUSED(scale);

    scaleTextEditedByUser = true;
}

void MainWindow::textColorChanged()
{
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/textpointer.png",
                                     qvariant_cast<QColor>(textAction->data())));
    textButtonTriggered();
}

void MainWindow::itemColorChanged()
{
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/floodfill.png",
                                     qvariant_cast<QColor>(fillAction->data())));
    fillButtonTriggered();
}

void MainWindow::lineColorChanged()
{
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/linecolor.png",
                                     qvariant_cast<QColor>(lineAction->data())));
    lineButtonTriggered();
}

void MainWindow::textButtonTriggered()
{
    scene->setTextColor(qvariant_cast<QColor>(textAction->data()));
}

void MainWindow::fillButtonTriggered()
{
    scene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
}

void MainWindow::lineButtonTriggered()
{
    scene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
}

void MainWindow::handleFontChange()
{
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());
    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(italicAction->isChecked());
    font.setUnderline(underlineAction->isChecked());

    scene->setFont(font);
}

void MainWindow::itemSelected(QGraphicsItem *item)
{
    DiagramTextItem *textItem =
    qgraphicsitem_cast<DiagramTextItem *>(item);

    QFont font = textItem->font();
    fontCombo->setCurrentFont(font);
    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    boldAction->setChecked(font.weight() == QFont::Bold);
    italicAction->setChecked(font.italic());
    underlineAction->setChecked(font.underline());
}

void MainWindow::about()
{
    QString message = tr("This program is for creating genealogies. It is "
                         "written using the Qt Framework. The program is "
                         "open-source and you are welcome to contribute "
                         "to it."
                         "\n\n"
                         "Version: 1");
    QMessageBox::about(this, tr("About Genealogy Maker"), message);
}

void MainWindow::newDiagram()
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

    // Update window title.
    updateWindowTitle();
}

void MainWindow::open()
{
    // Ask whether to save unsaved changes.
    if (!maybeSave()) {
        return;
    }

    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Open Genealogy File"),
                                         saveFileDir(),
                                         tr("Genealogy XML Files (*.xml)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Genealogy Maker"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    scene->open(&file);

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
}

void MainWindow::save()
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
     scene->save(&file);

     // Update window title.
     updateWindowTitle();

     // Mark undo stack.
     undoStack->setClean();
}

void MainWindow::saveAs()
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
     scene->save(&file);

     // Update window title.
     updateWindowTitle();

     // Mark undo stack.
     undoStack->setClean();
}

void MainWindow::onTreeItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    QUuid id = item->data(0, Qt::UserRole).toUuid();
    auto diagramItem = scene->itemWithId(id);
    if (diagramItem) {
        view->centerOn(diagramItem);
    }
}

void MainWindow::selectAll()
{
    scene->selectAll();
}

void MainWindow::selectNone()
{
    scene->clearSelection();
}

void MainWindow::print()
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

void MainWindow::alignItemsHorizontally()
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

void MainWindow::alignItemsVertically()
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

void MainWindow::onMouseWheelZoomed()
{
    double scale = view->matrix().m11();
    double percent = scale * 100.0;
    QString text = QString::number((int)percent) + "%";
    sceneScaleCombo->setCurrentText(text);

}

void MainWindow::onArrowAdded(Arrow *arrow)
{
    undoStack->push(new AddArrowUndo(scene, arrow));
}

void MainWindow::onItemsAboutToMove()
{
    auto items = scene->selectedItems();
    if (!items.empty()) {
        moveItemsUndo = new MoveItemsUndo(scene, items);
    }
}

void MainWindow::onItemsFinishedMoving()
{
    if (moveItemsUndo) {
        moveItemsUndo->storeAfterState();
        undoStack->push(moveItemsUndo);
        moveItemsUndo = nullptr;
    }
}

void MainWindow::onFind()
{
    if (!dialogFind) {
        dialogFind = new DialogFind(this);
        connect(dialogFind, SIGNAL(search(QString)), this, SLOT(onSearch(QString)));
    }
    dialogFind->show();
}

void MainWindow::onSearch(const QString &text)
{
    for (auto item: scene->items()) {
        if (item->type() == DiagramItem::Type) {
            auto diagramItem = qgraphicsitem_cast<DiagramItem *> (item);
            if (diagramItem->name().contains(text)) {
                view->centerOn(diagramItem);
                return;
            }
        }
    }

    qDebug() << "Item not found.";
}

void MainWindow::viewItemDetails()
{
    auto selectedItems = scene->selectedItems();
    if (selectedItems.isEmpty())
        return;

    auto item = selectedItems.first();
    if (item->type() == DiagramItem::Type) {
        auto person = qgraphicsitem_cast<DiagramItem *>(item);

        if (!dialogPersonDetails) {
            dialogPersonDetails = new DialogPersonDetails(this);
        }
        dialogPersonDetails->setItem(person);
        dialogPersonDetails->show();
    }
}

void MainWindow::onPeopleMarried(DiagramItem *person1, DiagramItem *person2)
{
    undoStack->push(new MarriageUndo(scene, person1, person2));
}

void MainWindow::removeMarriage()
{
    auto marriage = MarriageItem::getSelectedMarriage();
    marriage->personLeft()->removeMarriage();
    undoStack->push(new RemoveMarriageUndo(scene, marriage));
}

void MainWindow::viewMarriageDetails()
{
    if (!dialogMarriageDetails) {
        dialogMarriageDetails = new DialogMarriageDetails(this);
    }
    dialogMarriageDetails->setMarriage(MarriageItem::getSelectedMarriage());
    dialogMarriageDetails->show();
}

void MainWindow::onSceneCleared()
{
    treeItems.clear();
    tree->clear();
}

void MainWindow::updateWindowTitle()
{
    QString title;

    if (saveFileExists())
    {
        QFileInfo info(m_saveFileName);
        title = info.fileName();
    }
    else
    {
        title = "New Diagram";
    }

    if (!undoStack->isClean())
    {
        title += "*";
    }

    title += " - ";
    title += m_appName;

    setWindowTitle(title);
}

void MainWindow::onUndoStackCleanChanged(bool clean)
{
    Q_UNUSED(clean);
    updateWindowTitle();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::moveToCenter()
{
    //
    // Center the window on the screen.
    //
    auto geometry = QApplication::desktop()->screenGeometry();
    auto halfSize = size() / 2.0;
    QPoint offset(halfSize.width(), halfSize.height());
    move(geometry.center() - offset);
}

void MainWindow::onItemTextEdited(QGraphicsItem *item)
{
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

void MainWindow::createToolBox()
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
    layout->addWidget(createCellWidget(tr("Person"), DiagramItem::Step, tr("P")), 0, 0);

    //
    // "Relationship" button
    //
    arrowButton = new QToolButton;
    arrowButton->setCheckable(true);
    buttonGroup->addButton(arrowButton, InsertArrowButton);
    arrowButton->setIcon(QIcon(QPixmap(":/images/linepointer.png")));
    arrowButton->setIconSize(QSize(50, 50));
    arrowButton->setToolTip("Add Relationship");
    arrowButton->setShortcut(tr("R"));
    QGridLayout *textLayout = new QGridLayout;
    textLayout->addWidget(arrowButton, 0, 0, Qt::AlignHCenter);
    textLayout->addWidget(new QLabel(tr("Relationship")), 1, 0, Qt::AlignCenter);
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
    connect(tree, &QTreeWidget::itemDoubleClicked, this, &MainWindow::onTreeItemDoubleClicked);
    treeViewLayout->addWidget(tree);
    QWidget *treeViewWidget = new QWidget;
    treeViewWidget->setLayout(treeViewLayout);

    toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox->addItem(itemWidget, tr("Add Items"));
    toolBox->addItem(backgroundWidget, tr("Backgrounds"));
    toolBox->addItem(treeViewWidget, tr("Tree View"));
}

void MainWindow::createActions()
{
    findAction = new QAction(tr("Find..."), this);
    findAction->setShortcut(QKeySequence::Find);
    findAction->setStatusTip(tr("Find person with name"));
    connect(findAction, SIGNAL(triggered()), this, SLOT(onFind()));

    toFrontAction = new QAction(QIcon(":/images/bringtofront.png"),
                                tr("Bring to &Front"), this);
    toFrontAction->setShortcut(tr("Ctrl+Shift+F"));
    toFrontAction->setStatusTip(tr("Bring item to front"));
    connect(toFrontAction, SIGNAL(triggered()), this, SLOT(bringToFront()));

    sendBackAction = new QAction(QIcon(":/images/sendtoback.png"), tr("Send to &Back"), this);
    sendBackAction->setShortcut(tr("Ctrl+T"));
    sendBackAction->setStatusTip(tr("Send item to back"));
    connect(sendBackAction, SIGNAL(triggered()), this, SLOT(sendToBack()));

    deleteAction = new QAction(QIcon(":/images/delete.png"), tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    newAction = new QAction(tr("New"), this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Start a new diagram"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newDiagram()));

    openAction = new QAction(tr("Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open diagram"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    saveAction = new QAction(tr("Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save diagram"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    saveAsAction = new QAction(tr("Save As..."), this);
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Save diagram as a new file"));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    printAction = new QAction(tr("Print..."), this);
    printAction->setShortcuts(QKeySequence::Print);
    printAction->setStatusTip(tr("Print diagram"));
    connect(printAction, SIGNAL(triggered()), this, SLOT(print()));

    undoAction = undoStack->createUndoAction(this);
    undoAction->setShortcut(QKeySequence::Undo);
    redoAction = undoStack->createRedoAction(this);
    redoAction->setShortcut(QKeySequence::Redo);

    boldAction = new QAction(tr("Bold"), this);
    boldAction->setCheckable(true);
    QPixmap pixmap(":/images/bold.png");
    boldAction->setIcon(QIcon(pixmap));
    boldAction->setShortcut(tr("Ctrl+B"));
    connect(boldAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    italicAction = new QAction(QIcon(":/images/italic.png"), tr("Italic"), this);
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));
    connect(italicAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    underlineAction = new QAction(QIcon(":/images/underline.png"), tr("Underline"), this);
    underlineAction->setCheckable(true);
    underlineAction->setShortcut(tr("Ctrl+U"));
    connect(underlineAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("F1"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    selectAllAction = new QAction(tr("Select All"), this);
    selectAllAction->setShortcut(tr("Ctrl+A"));
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));

    selectNoneAction = new QAction(tr("Select None"), this);
    selectNoneAction->setShortcut(tr("Ctrl+Shift+A"));
    connect(selectNoneAction, SIGNAL(triggered()), this, SLOT(selectNone()));

    alignHorizontallyAction = new QAction(tr("Align Horizontally"), this);
    connect(alignHorizontallyAction, SIGNAL(triggered()), this, SLOT(alignItemsHorizontally()));

    alignVerticallyAction = new QAction(tr("Align Vertically"), this);
    connect(alignVerticallyAction, SIGNAL(triggered()), this, SLOT(alignItemsVertically()));

    viewDetailsAction = new QAction(tr("Details..."), this);
    viewDetailsAction->setShortcut(tr("Ctrl+D"));
    connect(viewDetailsAction, SIGNAL(triggered()), this, SLOT(viewItemDetails()));

    removeMarriageAction = new QAction(QIcon(":/images/delete.png"), tr("Remove Marriage"), this);
    connect(removeMarriageAction, SIGNAL(triggered()), this, SLOT(removeMarriage()));

    marriageDetailsAction = new QAction(tr("Marriage Details..."), this);
    connect(marriageDetailsAction, SIGNAL(triggered()), this, SLOT(viewMarriageDetails()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(printAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(findAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();
    itemMenu->addAction(toFrontAction);
    itemMenu->addAction(sendBackAction);
    itemMenu->addSeparator();
    itemMenu->addAction(viewDetailsAction);

    selectMenu = menuBar()->addMenu(tr("&Select"));
    selectMenu->addAction(selectAllAction);
    selectMenu->addAction(selectNoneAction);

    alignMenu = menuBar()->addMenu(tr("&Align"));
    alignMenu->addAction(alignHorizontallyAction);
    alignMenu->addAction(alignVerticallyAction);

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);

    marriageMenu = new QMenu("Marriage", this);
    marriageMenu->addAction(removeMarriageAction);
    marriageMenu->addAction(marriageDetailsAction);
    MarriageItem::setContextMenu(marriageMenu);
}

void MainWindow::createToolbars()
{
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(deleteAction);
    editToolBar->addAction(toFrontAction);
    editToolBar->addAction(sendBackAction);

    fontCombo = new QFontComboBox();
    connect(fontCombo, SIGNAL(currentFontChanged(QFont)),
            this, SLOT(currentFontChanged(QFont)));

    fontSizeCombo = new QComboBox;
    fontSizeCombo->setEditable(true);
    for (int i = 8; i < 30; i = i + 2)
        fontSizeCombo->addItem(QString().setNum(i));
    QIntValidator *validator = new QIntValidator(2, 64, this);
    fontSizeCombo->setValidator(validator);
    connect(fontSizeCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(fontSizeChanged(QString)));

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
    textAction = fontColorToolButton->menu()->defaultAction();
    fontColorToolButton->setIcon(createColorToolButtonIcon(":/images/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    connect(fontColorToolButton, SIGNAL(clicked()),
            this, SLOT(textButtonTriggered()));

    fillColorToolButton = new QToolButton;
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/floodfill.png", Qt::white));
    connect(fillColorToolButton, SIGNAL(clicked()),
            this, SLOT(fillButtonTriggered()));

    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/linecolor.png", Qt::black));
    connect(lineColorToolButton, SIGNAL(clicked()),
            this, SLOT(lineButtonTriggered()));

    textToolBar = addToolBar(tr("Font"));
    textToolBar->addWidget(fontCombo);
    textToolBar->addWidget(fontSizeCombo);
    textToolBar->addAction(boldAction);
    textToolBar->addAction(italicAction);
    textToolBar->addAction(underlineAction);

    colorToolBar = addToolBar(tr("Color"));
    colorToolBar->addWidget(fontColorToolButton);
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);

    QToolButton *pointerButton = new QToolButton;
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

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, SIGNAL(activated(QString)),
            this, SLOT(sceneScaleActivated(QString)));
    sceneScaleCombo->setEditable(true);
    sceneScaleCombo->setValidator(new PercentValidator(25, 200, this));
    sceneScaleCombo->setInsertPolicy(QComboBox::NoInsert);
    connect(sceneScaleCombo->lineEdit(), SIGNAL(editingFinished()),
            this, SLOT(sceneScaleEditingFinished()));
    connect(sceneScaleCombo->lineEdit(), SIGNAL(textEdited(QString)),
            this, SLOT(sceneScaleTextEdited(QString)));

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);
}

QWidget *MainWindow::createBackgroundCellWidget(const QString &text, const QString &image)
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

QWidget *MainWindow::createCellWidget(const QString &text, DiagramItem::DiagramType type, QKeySequence shortcut)
{
    DiagramItem item(type, itemMenu);
    QIcon icon(item.image());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    button->setToolTip(QString("Add ") + text);
    button->setShortcut(shortcut);
    buttonGroup->addButton(button, int(type));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

QMenu *MainWindow::createColorMenu(const char *slot, QColor defaultColor)
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

QIcon MainWindow::createColorToolButtonIcon(const QString &imageFile, QColor color)
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

QIcon MainWindow::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}

QString MainWindow::saveFileDir()
{
    // Ensure directory exists.
    QString dirName = "save-files";
    QDir().mkpath(dirName);

    // Return path.
    return QDir(dirName).path();
}

bool MainWindow::saveFileExists() const
{
    return !m_saveFileName.isEmpty();
}

bool MainWindow::maybeSave()
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
//! [32]
