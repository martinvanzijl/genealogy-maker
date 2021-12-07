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

#ifndef MAINFORM_H
#define MAINFORM_H

#include "diagramitem.h"

#include <QMainWindow>

namespace Ui {
class MainForm;
}

class Arrow;
class DiagramScene;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
class QTreeWidget;
class QTreeWidgetItem;
class QUndoStack;
class MoveItemsUndo;
class DialogFind;
class DialogPersonDetails;
class DialogMarriageDetails;
class DialogHelp;
class PreferencesWindow;
class QProcess;
QT_END_NAMESPACE

//! [0]
class MainForm : public QMainWindow
{
    Q_OBJECT

public:
   explicit MainForm(QWidget *parent = 0);
    ~MainForm();
   void moveToCenter();

   DiagramScene *getScene() const;

public slots:
   void onItemTextEdited(QGraphicsItem *item);

private slots:
    void backgroundButtonGroupClicked(QAbstractButton *button);
    void buttonGroupClicked(int id);
    void deleteItem();
    void pointerGroupClicked(int id);
    void bringToFront();
    void sendToBack();
    void itemInserted(DiagramItem *item, bool fromLoad);
    void onItemRemoved(DiagramItem *item);
    void currentFontChanged(const QFont &font);
    void fontSizeChanged(const QString &size);
    void sceneScaleActivated(const QString &scale);
    void sceneScaleEditingFinished();
    void sceneScaleTextEdited(const QString &scale);
    void textColorChanged();
    void itemColorChanged();
    void lineColorChanged();
    void textButtonTriggered();
    void fillButtonTriggered();
    void lineButtonTriggered();
//    void handleFontChange();
    void itemSelected(QGraphicsItem *item);
    void about();
    void newDiagram();
    void open();
    void save();
    void saveAs();
    void onTreeItemDoubleClicked(QTreeWidgetItem *item, int column);
    void selectAll();
    void selectNone();
    void print();
    void alignItemsHorizontally();
    void alignItemsVertically();
    void onMouseWheelZoomed();
    void onArrowAdded(Arrow *arrow);
    void onItemsAboutToMove();
    void onItemsFinishedMoving();
    void onFind();
    void onSearch(const QString &text);
    void viewSelectedItemDetails();
    void onPeopleMarried(DiagramItem *person1, DiagramItem *person2);
    void removeMarriage();
    void viewMarriageDetails();
    void onSceneCleared();
    void updateWindowTitle();
    void onUndoStackCleanChanged(bool clean);
    void onPersonDoubleClicked(DiagramItem *person);
    void onItemDragDropFinished();
    void onPreferencesChanged();
    void updateRecentFileActions();
    void openRecentFile();

    void showHelpContents();

    void on_actionChangeSize_triggered();

    void on_actionPreferences_triggered();

    void on_actionImportGedcomFile_triggered();

    void on_actionExportGedcomFile_triggered();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainForm *ui;

    void createToolBox();
    void createActions();
    void createMenus();
    void createToolbars();
    QWidget *createBackgroundCellWidget(const QString &text,
                                        const QString &image);
    QWidget *createCellWidget(const QString &text,
                              DiagramItem::DiagramType type, QKeySequence shortcut);
    QMenu *createColorMenu(const char *slot, QColor defaultColor);
    QIcon createColorToolButtonIcon(const QString &image, QColor color);
    QIcon createColorIcon(QColor color);
    QString saveFileDir();
    bool saveFileExists() const;
    bool maybeSave();
    void viewPersonDetails(DiagramItem *person);
    void addPythonPath(QProcess *process) const;
    QString getPythonPath() const;
    void styleToolButton(QToolButton *button) const;
    void open(const QString &fileName);

    // "Recent Files" menu.
    static bool hasRecentFiles();
    void prependToRecentFiles(const QString &fileName);
    void setRecentFilesVisible(bool visible);

    enum { MaxRecentFiles = 5 };

    QAction *recentFileActs[MaxRecentFiles];
    QAction *recentFileSubMenuAct;

    // Fields.
    DiagramScene *scene;
    QGraphicsView *view;

//    QAction *exitAction;
//    QAction *addAction;
//    QAction *deleteAction;
//    QAction *newAction;
//    QAction *openAction;
//    QAction *saveAction;
//    QAction *saveAsAction;

//    QAction *toFrontAction;
//    QAction *sendBackAction;
//    QAction *aboutAction;
//    QAction *selectAllAction;
//    QAction *selectNoneAction;
//    QAction *alignHorizontallyAction;
//    QAction *alignVerticallyAction;
//    QAction *viewDetailsAction;

//    QAction *printAction;

    QAction *removeMarriageAction;
    QAction *marriageDetailsAction;

//    QMenu *fileMenu;
//    QMenu *editMenu;
//    QMenu *itemMenu;
//    QMenu *selectMenu;
//    QMenu *alignMenu;
//    QMenu *aboutMenu;
    QMenu *marriageMenu;

    QToolBar *textToolBar;
    QToolBar *editToolBar;
    QToolBar *colorToolBar;
    QToolBar *pointerToolbar;

    QComboBox *sceneScaleCombo;
    QComboBox *itemColorCombo;
//    QComboBox *textColorCombo;
//    QComboBox *fontSizeCombo;
//    QFontComboBox *fontCombo;

    QToolBox *toolBox;
    QButtonGroup *buttonGroup;
    QButtonGroup *pointerTypeGroup;
    QButtonGroup *backgroundButtonGroup;
    QToolButton *fontColorToolButton;
    QToolButton *fillColorToolButton;
    QToolButton *lineColorToolButton;
    QToolButton *linePointerButton;
    QToolButton *arrowButton;
//    QAction *boldAction;
//    QAction *underlineAction;
//    QAction *italicAction;
    QAction *textAction;
    QAction *fillAction;
    QAction *lineAction;

    QTreeWidget *tree;
    QMap<QUuid, QTreeWidgetItem *> treeItems;

    bool scaleTextEditedByUser;

    QUndoStack *undoStack;
    QAction *undoAction;
    QAction *redoAction;
    MoveItemsUndo *moveItemsUndo;

//    QAction *findAction;
    DialogFind *dialogFind;
    DialogPersonDetails *dialogPersonDetails;
    DialogMarriageDetails *dialogMarriageDetails;
    DialogHelp *dialogHelp;

    QString m_saveFileName;
    QString m_appName;

    PreferencesWindow *preferencesWindow;

    // Hack to avoid seg-faults.
    bool m_beingDestroyed;

    // Hack to flag that a GEDCOM file was just imported.
    bool m_gedcomWasImported;
};

#endif // MAINFORM_H
