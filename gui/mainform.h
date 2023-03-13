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
class QPushButton;
class DialogFileProperties;
class QSlider;
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
   QGraphicsView *getView() const; // For unit test cases.

   void updateGuiFromPreferences();
   void open(const QString &fileName);

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
    void zoomSliderValueChanged(int value);
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
    void clearRecentFilesMenu();

    void showHelpContents();

    void on_actionChangeSize_triggered();

    void on_actionPreferences_triggered();

    void on_actionImportGedcomFile_triggered();

    void on_actionExportGedcomFile_triggered();

    void onCollapseButtonClicked(bool checked);

    void onTreeViewContextMenuRequested(const QPoint &pos);
    void onTreeViewDetailsAction();

    void on_actionExportImage_triggered();

    void on_actionPersonListReport_triggered();

    void on_actionTimelineReport_triggered();

    void on_actionFileProperties_triggered();

    void on_actionAutoLayout_triggered();

    void on_actionOpenExample_triggered();

    void on_actionSelectDescendants_triggered();

    void on_showSideBarAction_triggered();

    void on_actionFileExportImage_triggered();

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
    QString getPhotosFolderFor(const QString &fileName) const;

    // "Recent Files" menu.
    static bool hasRecentFiles();
    void prependToRecentFiles(const QString &fileName);
    void setRecentFilesVisible(bool visible);
    void removeFromRecentFiles(const QString &fileName);

    // Search.
    bool searchCheckPerson(QList<QGraphicsItem *> &items, int index, const QString &text);

    void exportImage();

    void createPersonListReport();
    void createTimelineReport();

    void showFileProperties();

    void autoLayoutDiagram();

    void openExampleDiagram();
    QString exampleFileDir() const;

    bool shouldRemoveInvalidFiles() const;

    void setSceneScale(double scale);

    void selectCurrentItemDescendants();

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
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *alignmentToolBar;
    QToolBar *colorToolBar;
    QToolBar *pointerToolbar;

    QComboBox *sceneScaleCombo;
    QComboBox *itemColorCombo;
//    QComboBox *textColorCombo;
//    QComboBox *fontSizeCombo;
//    QFontComboBox *fontCombo;

    QSlider *zoomSlider;

    QToolBox *toolBox;
    QButtonGroup *buttonGroup;
    QButtonGroup *pointerTypeGroup;
    QButtonGroup *backgroundButtonGroup;
    QToolButton *fontColorToolButton;
    QToolButton *fillColorToolButton;
    QToolButton *lineColorToolButton;
    QToolButton *linePointerButton;
    QToolButton *pointerButton;
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
    DialogFileProperties *dialogFileProperties;

    QString m_saveFileName;
    QString m_appName;

    PreferencesWindow *preferencesWindow;

    // Hack to avoid seg-faults.
    bool m_beingDestroyed;

    // Hack to flag that a GEDCOM file was just imported.
    bool m_gedcomWasImported;

    // Button to expand/collapse sidebar.
    QPushButton *collapseButton;

    // Item focused in tree-view.
    QTreeWidgetItem *treeFocusedItem;
    QMenu *treeViewContextMenu;

    // Search index.
    int m_searchFoundIndex;

    QString m_lastDiagramOpenFolder;
    QString m_lastGedcomImportFolder;

    // Flag for opening recent file.
    bool m_openingRecentFile;

    // Flag for updating slider from combo-box.
    bool m_disableZoomSliderSignal;
};

#endif // MAINFORM_H
