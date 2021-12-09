#include <QtWidgets>
#include <QtTest/QtTest>

#include "diagramitem.h"
#include "diagramscene.h"
#include "marriageitem.h"
#include "gui/dialogpersondetails.h"
#include "gui/mainform.h"

#include <QDebug>
#include <QObject>

// =============================================================================
// DetailsWindowHelper
// =============================================================================

class DetailsWindowHelper : public QObject
{
    Q_OBJECT

public:
    DetailsWindowHelper();
    virtual ~DetailsWindowHelper();

    bool isFinished() const;

public slots:
    void closeWindow();
    void setDisplayNameTest();
    void setGenderTest();

private:
    bool m_finished;
};

DetailsWindowHelper::DetailsWindowHelper() :
    m_finished(false)
{
    // Avoid compiler error.
}

DetailsWindowHelper::~DetailsWindowHelper()
{
    // Avoid compiler error.
}

bool DetailsWindowHelper::isFinished() const
{
    return m_finished;
}

void DetailsWindowHelper::closeWindow()
{
    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    DialogPersonDetails *dialog = dynamic_cast<DialogPersonDetails *>(widget);

    if (dialog)
    {
        QTimer::singleShot(0, dialog, SLOT(accept()));
    }

    // Set flag.
    m_finished = true;
}

void DetailsWindowHelper::setDisplayNameTest()
{
    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    DialogPersonDetails *dialog = dynamic_cast<DialogPersonDetails *>(widget);
    QVERIFY(dialog);

    // Get the edit box.
    QLineEdit *lineEditName = dialog->findChild<QLineEdit*>("lineEditName");
    QVERIFY(lineEditName);

    // Set the display name.
    lineEditName->setText("New Display Name");

    // Save changes.
    QPushButton *pushButtonSave = dialog->findChild<QPushButton*>("pushButtonSave");
    QVERIFY(pushButtonSave);
    pushButtonSave->click();

    // Set flag.
    m_finished = true;
}

void DetailsWindowHelper::setGenderTest()
{
    // Hack. Copied.
    const static int CBOX_GENDER_MALE = 0;
    const static int CBOX_GENDER_FEMALE = 1;
    const static int CBOX_GENDER_UNKNOWN = 2;

    Q_UNUSED(CBOX_GENDER_FEMALE);

    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    DialogPersonDetails *dialog = dynamic_cast<DialogPersonDetails *>(widget);
    QVERIFY(dialog);

    // Get the box.
    QComboBox *comboBoxGender = dialog->findChild<QComboBox*>("comboBoxGender");
    QVERIFY(comboBoxGender);

    // Check the position.
    QCOMPARE(comboBoxGender->currentIndex(), CBOX_GENDER_UNKNOWN);

    // Set the gender.
    comboBoxGender->setCurrentIndex(CBOX_GENDER_MALE);

    // Save changes.
    QPushButton *pushButtonSave = dialog->findChild<QPushButton*>("pushButtonSave");
    QVERIFY(pushButtonSave);
    pushButtonSave->click();

    // Close the window.
    //QTimer::singleShot(0, dialog, SLOT(accept()));

    // Set flag.
    m_finished = true;
}

// =============================================================================
// TestCaseHelper
// =============================================================================

class TestCaseHelper : public QObject
{
    Q_OBJECT

public:
    TestCaseHelper();
    virtual ~TestCaseHelper();
    void setOpenFileName(const QString &fileName);
    void setSaveFileName(const QString &saveFileName);
    bool isFinished() const;

private slots:
    void handleOpenDialog();
    void handleSaveDialog();

private:
    QString m_openFileName;
    QString m_saveFileName;
    bool m_finished;
};

TestCaseHelper::TestCaseHelper() :
    m_finished(false)
{

}

TestCaseHelper::~TestCaseHelper()
{
    // Avoid compiler error.
}

void TestCaseHelper::setOpenFileName(const QString &fileName)
{
    m_openFileName = fileName;
}

void TestCaseHelper::handleOpenDialog()
{
    QWidget *activeModalWidget = QApplication::activeModalWidget();
    QFileDialog *dialog = dynamic_cast<QFileDialog *>(activeModalWidget);

    if (dialog)
    {
        dialog->selectFile(m_openFileName);
        QTimer::singleShot(0, dialog, SLOT(accept()));
    }

    m_finished = true;
}


void TestCaseHelper::handleSaveDialog()
{
    QWidget *activeModalWidget = QApplication::activeModalWidget();
    QFileDialog *dialog = dynamic_cast<QFileDialog *>(activeModalWidget);

    if (dialog)
    {
        dialog->selectFile(m_saveFileName);
        QTimer::singleShot(0, dialog, SLOT(accept()));
    }

    m_finished = true;
}

void TestCaseHelper::setSaveFileName(const QString &saveFileName)
{
    m_saveFileName = saveFileName;
}

bool TestCaseHelper::isFinished() const
{
    return m_finished;
}

// =============================================================================
// TestCases
// =============================================================================

class TestCases: public QObject
{
    Q_OBJECT

public:
    virtual ~TestCases();

private slots:
    void cleanup();
    void init();

//private:
    void testNew();
    void testOpen();
    void testSave();
    void testExit();
    void testBug1();
    void volumeTest();
    void setGenderTest();
    void thumbnailTest();
    void defaultFillColorTest();
    void exportGedcomTest();
    void setDisplayNameTest();
    void importGedcomTest();
    void saveFillColorTest();
//    void doubleClickToViewDetailsTest();
    void deletePersonTest();
    void treeViewTest();
    void importGedcomThenSaveTest();
//    void importGedcomTreeViewTest();
//    void dragAndDropNewPersonTest();
    void windowTitleTest();

private slots:
    void testLongName();
    void testLongNameMarriage();
    void testRenameWhileMarried();

private:
    TestCaseHelper *m_helper;
    MainForm *m_mainWindow;

    DiagramItem *clickToAddPerson();
    void importGedcomFile(const QString &fileName);
    void openTestFile(const QString &fileName);
};

TestCases::~TestCases()
{
    // Avoid compiler bug.
}

void TestCases::cleanup()
{
    // Performed after each test case.
    m_mainWindow->disconnect();
    m_mainWindow->deleteLater();
    m_mainWindow = nullptr;

    // Wait for window to be destroyed.
    QTest::qWait(100);
}

void TestCases::init()
{
    // Performed before each test case.
    Q_INIT_RESOURCE(genealogymaker);

    m_mainWindow = new MainForm();
    m_mainWindow->moveToCenter();
    m_mainWindow->show();
    m_mainWindow->activateWindow();

    QTest::qWaitForWindowActive(m_mainWindow);
}

void TestCases::testNew()
{
    QTest::keyClicks(m_mainWindow, "N", Qt::ControlModifier);

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram"));
}

void TestCases::testOpen()
{
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName("van-zijl-new.xml");

    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    QTest::keyClicks(m_mainWindow, "O", Qt::ControlModifier);

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - van-zijl-new.xml"));
}

void TestCases::testSave()
{
    m_helper = new TestCaseHelper();
    m_helper->setSaveFileName("saved-diagram.xml");
    QTimer::singleShot(1000, m_helper, SLOT(handleSaveDialog()));
    QTest::keyClicks(m_mainWindow, "S", Qt::ControlModifier);

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - saved-diagram.xml"));
}

void TestCases::testExit()
{
    QTest::keyClicks(m_mainWindow, "Q", Qt::ControlModifier);
    QTest::qWait(100);

    QCOMPARE(m_mainWindow->isVisible(), false);
}

void TestCases::testBug1()
{
    // Open the test file.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName("bug-1-test.xml");

    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    QTest::keyClicks(m_mainWindow, "O", Qt::ControlModifier);

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - bug-1-test.xml"));

    // Select all.
    QTest::keyClicks(m_mainWindow, "A", Qt::ControlModifier);

    // TODO: Check that all are selected. For this I must add a getter for the "scene" field.

    // Delete all.
    QTest::keyClick(m_mainWindow, Qt::Key_Delete);

    // TODO: Check that all are deleted. For this I must add a getter for the "scene" field.
}

void TestCases::volumeTest()
{
    // Create parameters.
    const int personCount = 100;
    const int leftOffset = 200;
    const int topOffset = 100;

    // Create the diagram.
    for (int i = 0; i < personCount; ++i)
    {
        auto person = new DiagramItem(DiagramItem::Person, nullptr);
        person->setBrush(Qt::white);
        person->setPos(leftOffset, topOffset + i * 100);
        person->setName(QString("Person #%1").arg(i));
        m_mainWindow->getScene()->addItem(person);
    }

    // Save the diagram.
    m_helper = new TestCaseHelper();
    m_helper->setSaveFileName("volume-test.xml");
    QTimer::singleShot(1000, m_helper, SLOT(handleSaveDialog()));
    QTest::keyClicks(m_mainWindow, "S", Qt::ControlModifier);
}

void TestCases::setGenderTest()
{
    // Add person.
    auto person = new DiagramItem(DiagramItem::Person, nullptr);
    m_mainWindow->getScene()->addItem(person);

    // Select the person.
    m_mainWindow->getScene()->selectAll();

    // Set up helper to close the window.
    auto helper = new DetailsWindowHelper();
    QTimer::singleShot(1000, helper, SLOT(setGenderTest()));

    // Show the window.
    QTest::keyClicks(m_mainWindow, "D", Qt::ControlModifier);

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }

    // Check gender was set.
    QCOMPARE(person->getGender(), QString("M"));
}

void TestCases::thumbnailTest()
{
    // Open the test file.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName("thumbnail-test.xml");

    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    QTest::keyClicks(m_mainWindow, "O", Qt::ControlModifier);

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - thumbnail-test.xml"));

    // Get the person.
    QList<QGraphicsItem *> items = m_mainWindow->getScene()->items();
    QVERIFY(items.size() >= 1);

    QGraphicsItem *graphicsItem = items.last();
    DiagramItem *person = qgraphicsitem_cast<DiagramItem *>(graphicsItem);
    QVERIFY(person);

    // Check that the thumbnail is shown.
    bool thumbnailShown = false;

    QList<QGraphicsItem *> childItems = person->childItems();
    for (auto childItem: childItems)
    {
        if (childItem->type() == QGraphicsPixmapItem::Type)
        {
            thumbnailShown = childItem->isVisible();
            break;
        }
    }

    QVERIFY(thumbnailShown);
}

void TestCases::defaultFillColorTest()
{
    // Press the hotkey for adding a person.
    QTest::keyClicks(m_mainWindow, "P");

    // Allow hotkey to take effect.
    QTest::qWait(500);

    // Get the diagram widget.
    auto views = m_mainWindow->getScene()->views();
    QCOMPARE(views.size(), 1);
    auto widget = views.first();

    // Click on the diagram to create the person.
    QTest::mouseClick(widget, Qt::LeftButton);

    // Get the person.
    QList<QGraphicsItem *> items = m_mainWindow->getScene()->items();
    QVERIFY(items.size() >= 1);

    QGraphicsItem *graphicsItem = items.last();
    DiagramItem *person = qgraphicsitem_cast<DiagramItem *>(graphicsItem);
    QVERIFY(person);

    // Check the fill color.
    QVERIFY(person->brush() == QBrush(Qt::white));
}

void TestCases::exportGedcomTest()
{
    // Open the test file.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName("export-gedcom-test.xml");

    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    QTest::keyClicks(m_mainWindow, "O", Qt::ControlModifier);

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - export-gedcom-test.xml"));

    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionExportGedcomFile");
    QVERIFY(action);

    // Export to GEDCOM.
    const QString saveFileName = "export-gedcom-test.ged";
    m_helper->setSaveFileName(saveFileName);
    QTimer::singleShot(1000, m_helper, SLOT(handleSaveDialog()));
    action->trigger();

    // Check that the exported file exists.
    QFile file(QString("save-files/") + saveFileName);
    QVERIFY(file.exists());
}

void TestCases::setDisplayNameTest()
{
    // Add person.
    auto person = new DiagramItem(DiagramItem::Person, nullptr);
    m_mainWindow->getScene()->addItem(person);

    // Select the person.
    m_mainWindow->getScene()->selectAll();

    // Set up helper to close the window.
    auto helper = new DetailsWindowHelper();
    QTimer::singleShot(1000, helper, SLOT(setDisplayNameTest()));

    // Show the person details window.
    QTest::keyClicks(m_mainWindow, "D", Qt::ControlModifier);

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }

    // Check display name was set.
    QCOMPARE(person->textItem()->text(), QString("New Display Name"));
}

void TestCases::importGedcomTest()
{
    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionImportGedcomFile");
    QVERIFY(action);

    // Create the helper.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName("import-gedcom-test.ged");

    // Import the GEDCOM file.
    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    action->trigger();

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram (Imported from GEDCOM)"));

    // Get the person.
    QList<QGraphicsItem *> items = m_mainWindow->getScene()->items();
    QVERIFY(items.size() >= 1);

    QGraphicsItem *graphicsItem = items.last();
    DiagramItem *person = qgraphicsitem_cast<DiagramItem *>(graphicsItem);
    QVERIFY(person);

    // Check the name.
    QCOMPARE(person->getFirstName(), QString("Oupa"));

    // Debug.
    //    QTest::qWait(1000);
}

void TestCases::saveFillColorTest()
{
    // Create person.
    auto person = new DiagramItem(DiagramItem::Person, nullptr);

    // Set the fill color.
    QBrush newBrush(Qt::blue);
    person->setBrush(newBrush);

    // Add person to scene.
    m_mainWindow->getScene()->addItem(person);

    // Save diagram.
    QString diagramFileName = "save-fill-color-test.xml";
    m_helper = new TestCaseHelper();
    m_helper->setSaveFileName(diagramFileName);
    QTimer::singleShot(1000, m_helper, SLOT(handleSaveDialog()));
    QTest::keyClicks(m_mainWindow, "S", Qt::ControlModifier);

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - save-fill-color-test.xml"));

    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("newAction");
    QVERIFY(action);

    // Create a new diagram.
    action->trigger();
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram"));

    // Open the saved file.
    m_helper->setOpenFileName(diagramFileName);

    action = m_mainWindow->findChild<QAction*>("openAction");
    QVERIFY(action);

    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    action->trigger();

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - ") + diagramFileName);

    // Get the person.
    QList<QGraphicsItem *> items = m_mainWindow->getScene()->items();
    QVERIFY(items.size() >= 1);

    QGraphicsItem *graphicsItem = items.last();
    person = qgraphicsitem_cast<DiagramItem *>(graphicsItem);
    QVERIFY(person);

    // Check the fill color.
    QVERIFY(person->brush() == newBrush);
}

//void TestCases::doubleClickToViewDetailsTest()
//{
//    // Get the diagram widget.
//    auto views = m_mainWindow->getScene()->views();
//    QCOMPARE(views.size(), 1);
//    auto widget = views.first();

//    // Press the hotkey for adding a person.
//    QTest::keyClicks(m_mainWindow, "P");

//    // Allow hotkey to take effect.
//    QTest::qWait(500);

//    // Click on the diagram to create the person.
//    QTest::mouseClick(widget, Qt::LeftButton);

//    // Get the person.
//    QList<QGraphicsItem *> items = m_mainWindow->getScene()->items();
//    QVERIFY(items.size() >= 1);

//    QGraphicsItem *graphicsItem = items.last();
//    DiagramItem *person = qgraphicsitem_cast<DiagramItem *>(graphicsItem);
//    QVERIFY(person);

//    // Press Enter key to finish editing name.
//    QTest::keyClick(widget, Qt::Key_Return);

//    // Wait for this to take effect.
//    QTest::qWait(500);

//    // Set up helper to close the window.
//    auto helper = new DetailsWindowHelper();
//    QTimer::singleShot(1000, helper, SLOT(closeWindow()));

//    // Double-click on the diagram to view the person's details.
//    qDebug() << "Trying to double click.";

//    // Does not work.
//    QTest::mouseDClick(widget, Qt::LeftButton);

//    // Check that the window was shown.
//    QVERIFY(helper->isFinished());
//}

void TestCases::deletePersonTest()
{
    // Open the test file.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName("delete-person-test.xml");

    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    QTest::keyClicks(m_mainWindow, "O", Qt::ControlModifier);

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - delete-person-test.xml"));

    // Get the persons.
    DiagramItem *pa = nullptr;
    DiagramItem *oupa = nullptr;

    DiagramScene *scene = m_mainWindow->getScene();
    QList<QGraphicsItem *> items = scene->items();

    for (auto item: items)
    {
        if (item->type() == DiagramItem::Type)
        {
            DiagramItem *person = qgraphicsitem_cast<DiagramItem *>(item);
            if (person->name() == "Pa")
            {
                pa = person;
            }
            else if (person->name() == "Oupa")
            {
                oupa = person;
            }
        }
    }

    QVERIFY(pa);
    QVERIFY(oupa);

    // Select the person.
    oupa->setSelected(true);

    // Get the action.
    QAction *deleteAction = m_mainWindow->findChild<QAction*>("deleteAction");
    QVERIFY(deleteAction);

    // Delete the person.
    QCOMPARE(oupa->scene(), scene);
    deleteAction->trigger();
    QCOMPARE(oupa->scene(), nullptr);

    // Check the relationship was deleted, too.
    QVERIFY(pa->getArrows().empty());

    // Wait before using the hotkey.
    QTest::qWait(500);

    // Undo the deletion.
    QTest::keyClicks(m_mainWindow, "Z", Qt::ControlModifier);

    // Allow the hotkey to take effect.
    QTest::qWait(500);

    // Check the person was restored.
    QCOMPARE(oupa->scene(), scene);

    // Check the relationship was restored, too.
    QVERIFY(!pa->getArrows().empty());
}

void TestCases::treeViewTest()
{
    // Add person.
    DiagramItem *person = clickToAddPerson();

    // Get the tree-view.
    QTreeWidget *treeView = m_mainWindow->findChild<QTreeWidget*>("treeViewPersons");
    QVERIFY(treeView);

    // Check that tree-view is updated.
    QCOMPARE(treeView->model()->rowCount(), 1);

    // Check that entry is named correctly.
    auto treeViewItem = treeView->topLevelItem(0);
    QCOMPARE(treeViewItem->text(0), QString("New Person"));

    // Select the person.
    m_mainWindow->getScene()->selectAll();

    // Set up helper to close the window.
    auto helper = new DetailsWindowHelper();
    QTimer::singleShot(1000, helper, SLOT(setDisplayNameTest()));

    // Show the person details window.
    QAction *action = m_mainWindow->findChild<QAction*>("viewDetailsAction");
    QVERIFY(action);
    action->trigger();

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }

    // Check display name was set.
    QCOMPARE(person->textItem()->text(), QString("New Display Name"));

    // Check that the entry is renamed.
    QCOMPARE(treeViewItem->text(0), QString("New Display Name"));
}

void TestCases::importGedcomThenSaveTest()
{
    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionImportGedcomFile");
    QVERIFY(action);

    // Create the helper.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName("import-gedcom-test.ged");

    // Import the GEDCOM file.
    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    action->trigger();

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram (Imported from GEDCOM)"));

    // Save the diagram.
    m_helper = new TestCaseHelper();
    m_helper->setSaveFileName("saved-diagram.xml");
    QTimer::singleShot(1000, m_helper, SLOT(handleSaveDialog()));

    action = m_mainWindow->findChild<QAction*>("saveAction");
    action->trigger();

    // Check that diagram was saved correctly.
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - saved-diagram.xml"));
}

// Does not work yet, since I cannot seem to make a double-click event work.
//void TestCases::importGedcomTreeViewTest()
//{
//    // Import GEDCOM file.
//    importGedcomFile("tree-view-test.ged");

//    // Get the tree-view.
//    QTreeWidget *treeView = m_mainWindow->findChild<QTreeWidget*>("treeViewPersons");
//    QVERIFY(treeView);

//    // Double-click the first person.
//    auto item = treeView->topLevelItem(0);
//    auto pos = treeView->visualItemRect(item).center();
//    QTest::mouseDClick(treeView, Qt::LeftButton, Qt::KeyboardModifiers(), pos);

//    // Get the diagram view.
//    auto views = m_mainWindow->getScene()->views();
//    auto diagramWidget = views.first();

//    // Check that the first person is in view.
//    auto transformFirstPerson = diagramWidget->viewportTransform();
//    qDebug() << transformFirstPerson.dx() << transformFirstPerson.dy();

//    // Double-click the second person.
//    item = treeView->topLevelItem(1);
//    pos = treeView->visualItemRect(item).center();
//    QTest::mouseDClick(treeView, Qt::LeftButton, Qt::KeyboardModifiers(), pos);

//    // Check that the second person is in view.
//    auto transformSecondPerson= diagramWidget->viewportTransform();
//    qDebug() << transformSecondPerson.dx() << transformSecondPerson.dy();

//    // Check that the viewport moved.
//    QVERIFY(transformFirstPerson != transformSecondPerson);
//}

// Does not work yet. I cannot seem to simulate a drag and drop action.
//void TestCases::dragAndDropNewPersonTest()
//{
//    // Get the button.
//    QToolButton *button = m_mainWindow->findChild<QToolButton*>("buttonAddPerson");
//    QVERIFY(button);

//    // Get the diagram widget.
//    auto views = m_mainWindow->getScene()->views();
//    auto diagramWidget = views.first();

//    // Press the button.
//    QTest::mousePress(button, Qt::LeftButton);

//    // Wait for the drag to start.
//    QTest::qWait(500);

//    // Move the mouse.
//    //QTest::mouseMove(m_mainWindow, QPoint(300, 300));

//    // Press the button.
////    QTest::mouseRelease(button, Qt::LeftButton); // Does not work.
////    QTest::mouseClick(diagramWidget, Qt::LeftButton); // Does not work.
//}

void TestCases::windowTitleTest()
{
    // Check initial title.
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram"));

    // Import GEDCOM.
    importGedcomFile("import-gedcom-test.ged");

    // Check title.
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram (Imported from GEDCOM)"));

    // Create new diagram.
    QAction *action = m_mainWindow->findChild<QAction*>("newAction");
    QVERIFY(action);
    action->trigger();

    // Check title.
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram"));

    // Import GEDCOM.
    importGedcomFile("import-gedcom-test.ged");

    // Check title.
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram (Imported from GEDCOM)"));

    // Open another diagram.
    openTestFile("van-zijl-new.xml");

    // Check title.
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - van-zijl-new.xml"));
}

void TestCases::testLongName()
{
    // Create person.
    DiagramItem *person = clickToAddPerson();

    // Get original size.
    QString originalName = person->name();
    auto originalRect = person->boundingRect();

    // Set name.
    person->setName("This Should Cause the Box to Expand");

    // Check that name fits in box.
    int boxWidth = person->boundingRect().width();
    int textWidth = person->textItem()->boundingRect().width();

    QVERIFY(boxWidth >= textWidth);

    // Undo. Check original box is restored.
    person->setName(originalName);

    QCOMPARE(person->boundingRect(), originalRect);
}

void TestCases::testLongNameMarriage()
{
    // Create people.
    DiagramItem *husband = clickToAddPerson();
    DiagramItem *wife = clickToAddPerson();

    // Set the names.
    husband->setName("HUSBAND: This Should Cause the Box to Expand");
    wife->setName("WIFE: This is Another Very Long Name");

    // Marry the people.
    husband->marryTo(wife);

    // Check the positions.
    MarriageItem *marriageItem = husband->getMarriageItem();
    QVERIFY(marriageItem);
    QCOMPARE(marriageItem->x(), husband->boundingRect().width() / 2 - marriageItem->boundingRect().width() / 2);
    QCOMPARE(wife->x() - wife->boundingRect().width() / 2, husband->x() + husband->boundingRect().width() / 2);
}

void TestCases::testRenameWhileMarried()
{
    // Create people.
    DiagramItem *husband = clickToAddPerson();
    DiagramItem *wife = clickToAddPerson();

    // Marry the people.
    husband->marryTo(wife);

    // Set the names.
    husband->setName("HUSBAND: This Should Cause the Box to Expand");
    wife->setName("WIFE: This is Another Very Long Name");

    // Check the positions.
    MarriageItem *marriageItem = husband->getMarriageItem();
    QVERIFY(marriageItem);
    QCOMPARE(marriageItem->x(), husband->boundingRect().width() / 2 - marriageItem->boundingRect().width() / 2);
    QCOMPARE(wife->x() - wife->boundingRect().width() / 2, husband->x() + husband->boundingRect().width() / 2);
}

DiagramItem *TestCases::clickToAddPerson()
{
    // Get the diagram widget.
    auto views = m_mainWindow->getScene()->views();
//    QCOMPARE(views.size(), 1);
    auto widget = views.first();

    // Press the hotkey for adding a person.
    QTest::keyClicks(m_mainWindow, "P");

    // Allow hotkey to take effect.
    QTest::qWait(500);

    // Click on the diagram to create the person.
    QTest::mouseClick(widget, Qt::LeftButton);

    // Get the person.
    QList<QGraphicsItem *> items = m_mainWindow->getScene()->items();
//    QVERIFY(items.size() >= 1);

//    QGraphicsItem *graphicsItem = items.last();
    QGraphicsItem *textItem = items.first();
    Q_ASSERT(textItem);
    QGraphicsItem *graphicsItem = textItem->parentItem();
    DiagramItem *person = qgraphicsitem_cast<DiagramItem *>(graphicsItem);
    Q_ASSERT(person);
//    QVERIFY(person);

    // Press Enter key to finish editing name.
    QTest::keyClick(widget, Qt::Key_Return);

    // Wait for this to take effect.
    QTest::qWait(500);

    // Return.
    return person;
}

void TestCases::importGedcomFile(const QString &fileName)
{
    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionImportGedcomFile");
//    QVERIFY(action);

    // Create the helper.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName(fileName);

    // Import the GEDCOM file.
    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    action->trigger();

//    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram (Imported from GEDCOM)"));
}

void TestCases::openTestFile(const QString &fileName)
{
    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("openAction");
//    QVERIFY(action);

    // Create the helper.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName(fileName);

    // Open the file.
    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    action->trigger();
}

QTEST_MAIN(TestCases)
#include "testcases.moc"
