#include <QtWidgets>
#include <QtTest/QtTest>

#include "diagramitem.h"
#include "diagramscene.h"
#include "fileutils.h"
#include "marriageitem.h"
#include "gui/dialogchangesize.h"
#include "gui/dialogfileproperties.h"
#include "gui/dialogfind.h"
#include "gui/dialogpersondetails.h"
#include "gui/mainform.h"
#include "gui/preferenceswindow.h"
#include "gui/reportwindow.h"
#include "viewphotowindow.h"

#include <QDebug>
#include <QObject>
#include <QTableWidget>

//
// Functions.
//
static QString getTestInputFilePathFor(const QString &fileName)
{
    QDir dir("../genealogy-maker/test-files/");
    return dir.absoluteFilePath(fileName);
}

//
// Class declarations.
//
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
    void addPhotoTest();
    void addTwoPhotosWithSameNameTest();
    void closeWindow();
    void setDisplayNameTest();
    void setGenderTest();
    void undoEditPersonDetailsTest();
    void viewPhotosTest();

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

void DetailsWindowHelper::addPhotoTest()
{
    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    DialogPersonDetails *dialog = dynamic_cast<DialogPersonDetails *>(widget);
    QVERIFY(dialog);

    // Get the button.
    QPushButton *pushButtonAddPhoto = dialog->findChild<QPushButton*>("pushButtonAddPhoto");
    QVERIFY(pushButtonAddPhoto);

    // Create the helper.
    auto helper = new TestCaseHelper();
    helper->setOpenFileName(getTestInputFilePathFor("Oupa/Photo.png"));

    // Add the photo.
    QTimer::singleShot(1000, helper, SLOT(handleOpenDialog()));
    pushButtonAddPhoto->click();

    // Save changes.
    QPushButton *pushButtonSave = dialog->findChild<QPushButton*>("pushButtonSave");
    QVERIFY(pushButtonSave);
    pushButtonSave->click();

    // Set flag.
    m_finished = true;
}

void DetailsWindowHelper::addTwoPhotosWithSameNameTest()
{
    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    DialogPersonDetails *dialog = dynamic_cast<DialogPersonDetails *>(widget);
    QVERIFY(dialog);

    // Get the button.
    QPushButton *pushButtonAddPhoto = dialog->findChild<QPushButton*>("pushButtonAddPhoto");
    QVERIFY(pushButtonAddPhoto);

    // Create the helper.
    auto helper = new TestCaseHelper();

    // Add the photos.
    helper->setOpenFileName(getTestInputFilePathFor("Ouma/Photo.png"));
    QTimer::singleShot(1000, helper, SLOT(handleOpenDialog()));
    pushButtonAddPhoto->click();

    helper->setOpenFileName(getTestInputFilePathFor("Oupa/Photo.png"));
    QTimer::singleShot(1000, helper, SLOT(handleOpenDialog()));
    pushButtonAddPhoto->click();

    // Save changes.
    QPushButton *pushButtonSave = dialog->findChild<QPushButton*>("pushButtonSave");
    QVERIFY(pushButtonSave);
    pushButtonSave->click();

    // Set flag.
    m_finished = true;
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

void DetailsWindowHelper::undoEditPersonDetailsTest()
{
    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    DialogPersonDetails *dialog = dynamic_cast<DialogPersonDetails *>(widget);
    QVERIFY(dialog);

    // Update display name.
    QLineEdit *lineEdit = dialog->findChild<QLineEdit*>("lineEditName");
    QVERIFY(lineEdit);
    lineEdit->setText("New Display Name");

    // Update first name.
    lineEdit = dialog->findChild<QLineEdit*>("lineEditFirstName");
    QVERIFY(lineEdit);
    lineEdit->setText("New First Name");

    // Update last name.
    lineEdit = dialog->findChild<QLineEdit*>("lineEditLastName");
    QVERIFY(lineEdit);
    lineEdit->setText("New Last Name");

    // Update date of birth.
    QDateEdit *dateEdit = dialog->findChild<QDateEdit*>("dateEditBirth");
    QVERIFY(dateEdit);
    dateEdit->setDate(QDateTime::currentDateTime().date());

    // Update place of birth.
    lineEdit = dialog->findChild<QLineEdit*>("lineEditPlaceOfBirth");
    QVERIFY(lineEdit);
    lineEdit->setText("New Place of Birth");

    // Update country of birth.
    lineEdit = dialog->findChild<QLineEdit*>("lineEditCountryOfBirth");
    QVERIFY(lineEdit);
    lineEdit->setText("New Country of Birth");

    // Update date of death.
    dateEdit = dialog->findChild<QDateEdit*>("dateEditDeath");
    QVERIFY(dateEdit);
    dateEdit->setDate(QDateTime::currentDateTime().date().addDays(1));

    // Update place of death.
    lineEdit = dialog->findChild<QLineEdit*>("lineEditPlaceOfDeath");
    QVERIFY(lineEdit);
    lineEdit->setText("New Place of Death");

    // Update place of death.
    QComboBox *comboBox = dialog->findChild<QComboBox*>("comboBoxGender");
    QVERIFY(comboBox);
    comboBox->setCurrentIndex(0);

    // Update bio.
    QPlainTextEdit *plainTextEdit = dialog->findChild<QPlainTextEdit*>("plainTextEditBio");
    QVERIFY(plainTextEdit);
    plainTextEdit->setPlainText("New Bio");

    // Update photos.
    QString photoFileName = getTestInputFilePathFor("Oupa/Photo.png");

    TestCaseHelper *helper = new TestCaseHelper();
    helper->setOpenFileName(photoFileName);
    QTimer::singleShot(1000, helper, SLOT(handleOpenDialog()));

    QPushButton *pushButton = dialog->findChild<QPushButton*>("pushButtonAddPhoto");
    QVERIFY(pushButton);
    pushButton->click();

    // TODO: Edit the rest of the fields.

    // Save changes.
    QPushButton *pushButtonSave = dialog->findChild<QPushButton*>("pushButtonSave");
    QVERIFY(pushButtonSave);
    pushButtonSave->click();

    // Set flag.
    m_finished = true;
}

void DetailsWindowHelper::viewPhotosTest()
{
    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    DialogPersonDetails *dialog = dynamic_cast<DialogPersonDetails *>(widget);
    QVERIFY(dialog);

    // Get the list.
    QListWidget *listWidgetPhotos = dialog->findChild<QListWidget*>("listWidgetPhotos");
    QVERIFY(listWidgetPhotos);

    // Check the entry count.
    int photoCount = listWidgetPhotos->count();
    QVERIFY(photoCount > 0);

    // Select the first item.
    listWidgetPhotos->setCurrentRow(0);

    // Press Enter.
    QTest::keyClick(listWidgetPhotos, Qt::Key_Return);

    // Wait for window to show.
    QTest::qWait(1000);

    // Check that window is shown.
    QWidget *newWidget = QApplication::activeWindow();
    ViewPhotoWindow *window = dynamic_cast<ViewPhotoWindow *>(newWidget);
    QVERIFY(window);

    // Set flag.
    m_finished = true;
}

// =============================================================================
// DiagramSizeDialogHelper
// =============================================================================

class DiagramSizeDialogHelper : public QObject
{
    Q_OBJECT

public:
    DiagramSizeDialogHelper();
    virtual ~DiagramSizeDialogHelper();

    bool isFinished() const;

public slots:
    void changeDiagramSize();

private:
    bool m_finished;
};

DiagramSizeDialogHelper::DiagramSizeDialogHelper() :
    m_finished(false)
{

}

DiagramSizeDialogHelper::~DiagramSizeDialogHelper()
{
    // Avoid compiler error.
}

bool DiagramSizeDialogHelper::isFinished() const
{
    return m_finished;
}

void DiagramSizeDialogHelper::changeDiagramSize()
{
    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    DialogChangeSize *dialog = dynamic_cast<DialogChangeSize *>(widget);
    QVERIFY(dialog);

    // Set the size.
    QSpinBox *spinBoxHorizontal = dialog->findChild<QSpinBox*>("spinBoxHorizontal");
    QSpinBox *spinBoxVertical = dialog->findChild<QSpinBox*>("spinBoxVertical");

    QVERIFY(spinBoxHorizontal);
    QVERIFY(spinBoxVertical);

    spinBoxHorizontal->setValue(2500);
    spinBoxVertical->setValue(2500);

    // Apply the changes.
    QPushButton *pushButtonOK = dialog->findChild<QPushButton*>("pushButtonOK");
    QVERIFY(pushButtonOK);
    pushButtonOK->click();

    // Set flag.
    m_finished = true;
}

// =============================================================================
// FilePropertiesDialogHelper
// =============================================================================

class FilePropertiesDialogHelper : public QObject
{
    Q_OBJECT

public:
    FilePropertiesDialogHelper();
    virtual ~FilePropertiesDialogHelper();

    bool isFinished() const;

public slots:
    void checkLabelsCorrect();

private:
    bool m_finished;
};

FilePropertiesDialogHelper::FilePropertiesDialogHelper() :
    m_finished(false)
{

}

FilePropertiesDialogHelper::~FilePropertiesDialogHelper()
{
    // Avoid compiler error.
}

bool FilePropertiesDialogHelper::isFinished() const
{
    return m_finished;
}

void FilePropertiesDialogHelper::checkLabelsCorrect()
{
    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    DialogFileProperties *dialog = dynamic_cast<DialogFileProperties *>(widget);
    QVERIFY(dialog);

    // Get the components.
    QLabel *marriageCountLabel = dialog->findChild<QLabel*>("labelNumberOfMarriagesValue");
    QLabel *personCountLabel = dialog->findChild<QLabel*>("labelNumberOfPeopleValue");
    QLabel *relationshipCountLabel = dialog->findChild<QLabel*>("labelNumberOfRelationshipsValue");

    QCOMPARE(marriageCountLabel->text().toInt(), 1);
    QCOMPARE(personCountLabel->text().toInt(), 7);
    QCOMPARE(relationshipCountLabel->text().toInt(), 3);

    // Close the dialog.
    QPushButton *pushButtonClose = dialog->findChild<QPushButton*>("pushButtonClose");
    QVERIFY(pushButtonClose);
    pushButtonClose->click();

    // Set flag.
    m_finished = true;
}

// =============================================================================
// FindDialogHelper
// =============================================================================

class FindDialogHelper : public QObject
{
    Q_OBJECT

public:
    FindDialogHelper();
    virtual ~FindDialogHelper();

    bool isFinished() const;

public slots:
    void testLabel();

private:
    bool m_finished;
};

FindDialogHelper::FindDialogHelper() :
    m_finished(false)
{

}

FindDialogHelper::~FindDialogHelper()
{
    // Avoid compiler error.
}

bool FindDialogHelper::isFinished() const
{
    return m_finished;
}

void FindDialogHelper::testLabel()
{
    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    DialogFind *dialog = dynamic_cast<DialogFind *>(widget);
    QVERIFY(dialog);

    // Get the components.
    QLabel *statusLabel = dialog->findChild<QLabel*>("labelStatus");

    QCOMPARE(statusLabel->text(), QString("Enter the person's name and click \"Find\"."));

    // Enter a non-existent name.
    QLineEdit *lineEditText = dialog->findChild<QLineEdit*>("lineEditText");
    QVERIFY(lineEditText);
    QTest::keyClicks(lineEditText, "XYZ");

    // Push the "Find" button.
    QPushButton *pushButtonFind = dialog->findChild<QPushButton*>("pushButtonFind");
    QVERIFY(pushButtonFind);
    pushButtonFind->click();

    QCOMPARE(statusLabel->text(), QString("Person not found."));

    // Close the dialog.
    QPushButton *pushButtonClose = dialog->findChild<QPushButton*>("pushButtonClose");
    QVERIFY(pushButtonClose);
    pushButtonClose->click();

    // Set flag.
    m_finished = true;
}

// =============================================================================
// ReportWindowHelper
// =============================================================================

class ReportWindowHelper : public QObject
{
    Q_OBJECT

public:
    ReportWindowHelper();
    virtual ~ReportWindowHelper();

    bool isFinished() const;

public slots:
    void testDefaultDates();

private:
    bool m_finished;
};

ReportWindowHelper::ReportWindowHelper() :
    m_finished(false)
{

}

ReportWindowHelper::~ReportWindowHelper()
{
    // Avoid compiler error.
}

bool ReportWindowHelper::isFinished() const
{
    return m_finished;
}

void ReportWindowHelper::testDefaultDates()
{
    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    ReportWindow *window = dynamic_cast<ReportWindow *>(widget);
    QVERIFY(window);

    // Get the components.
    QTableWidget *tableWidgetReport = window->findChild<QTableWidget*>("tableWidgetReport");
    QVERIFY(tableWidgetReport);

    // Check values.

    // Date of birth populated.
    auto toDate = QDate::fromString(tableWidgetReport->item(0, 3)->data(Qt::DisplayRole).toString());
    QCOMPARE(toDate, QDate(1900, 1, 31));

    // Date of death empty.
    QCOMPARE(tableWidgetReport->item(0, 6)->data(Qt::DisplayRole).toString(), QString(""));

    // Date of death populated.
    toDate = QDate::fromString(tableWidgetReport->item(1, 6)->data(Qt::DisplayRole).toString());
    QCOMPARE(toDate, QDate(2020, 12, 1));

    // Date of birth empty.
    QCOMPARE(tableWidgetReport->item(1, 3)->data(Qt::DisplayRole).toString(), QString(""));

    // Close the dialog.
    QPushButton *pushButtonClose = window->findChild<QPushButton*>("pushButtonClose");
    QVERIFY(pushButtonClose);
    pushButtonClose->click();

    // Set flag.
    m_finished = true;
}

// =============================================================================
// PreferencesWindowHelper
// =============================================================================

class PreferencesWindowHelper : public QObject
{
    Q_OBJECT

public:
    PreferencesWindowHelper();
    virtual ~PreferencesWindowHelper();

    bool isFinished() const;

public slots:
    void testSetDiagramFont();

private:
    bool m_finished;
};

PreferencesWindowHelper::PreferencesWindowHelper() :
    m_finished(false)
{

}

PreferencesWindowHelper::~PreferencesWindowHelper()
{
    // Avoid compiler error.
}

bool PreferencesWindowHelper::isFinished() const
{
    return m_finished;
}

void PreferencesWindowHelper::testSetDiagramFont()
{
    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    PreferencesWindow *window = dynamic_cast<PreferencesWindow *>(widget);
    QVERIFY(window);

    // Get the components.
    QFontComboBox *fontComboBoxDiagramFont = window->findChild<QFontComboBox*>("fontComboBoxDiagramFont");
    QVERIFY(fontComboBoxDiagramFont);

    QComboBox *comboBoxDiagramFontSize = window->findChild<QComboBox*>("comboBoxDiagramFontSize");
    QVERIFY(comboBoxDiagramFontSize);

    // Set values.
    fontComboBoxDiagramFont->setCurrentText("Serif");
    comboBoxDiagramFontSize->setCurrentText("10");

    // Apply the changes.
    QPushButton *pushButtonApply = window->findChild<QPushButton*>("pushButtonApply");
    QVERIFY(pushButtonApply);
    pushButtonApply->click();

    // Close the dialog.
    QPushButton *pushButtonClose = window->findChild<QPushButton*>("pushButtonClose");
    QVERIFY(pushButtonClose);
    pushButtonClose->click();

    // Set flag.
    m_finished = true;
}

// =============================================================================
// TestCaseHelper
// =============================================================================

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
#ifdef Q_OS_WIN
        // Does not work. Puts path into file name edit box.
        QFileInfo info(m_openFileName);
        dialog->setDirectory(info.absoluteDir());
        dialog->selectFile(info.fileName());
#else
        dialog->selectFile(m_openFileName);
#endif

        // Add delay in case the file is in a different directory.
        QTest::qWait(100);

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
        QTest::qWait(100); // Hack to ensure file is selected.
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
// FillColorDialogHelper
// =============================================================================

class FillColorDialogHelper : public QObject
{
    Q_OBJECT

public:
    FillColorDialogHelper();
    virtual ~FillColorDialogHelper();

    bool isFinished() const;

public slots:
    void setFillColor();

private:
    bool m_finished;
};

FillColorDialogHelper::FillColorDialogHelper() :
    m_finished(false)
{

}

FillColorDialogHelper::~FillColorDialogHelper()
{
    // Avoid compiler error.
}

bool FillColorDialogHelper::isFinished() const
{
    return m_finished;
}

void FillColorDialogHelper::setFillColor()
{
    // Get the window.
    QWidget *widget = QApplication::activeWindow();
    QColorDialog *dialog = dynamic_cast<QColorDialog *>(widget);
    QVERIFY(dialog);

    // Set the color.
    dialog->setCurrentColor(Qt::blue);

    // Apply the changes.
    dialog->accept();
    dialog->close();

    // Set flag.
    m_finished = true;
}

// =============================================================================
// TestCases
// =============================================================================

class TestCases: public QObject
{
    Q_OBJECT

public:
    TestCases();
    virtual ~TestCases();

private:
    void openTestFile(const QString &fileName);
    void saveTestFileAs(const QString &fileName);
    DiagramItem *getFirstPerson() const;

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
    void testLongName();
    void testLongNameMarriage();
    void testRenameWhileMarried();
    void savePhotosTest();
    void addPhotoTest();
    void saveInOtherFolderTest();
    void testUndoEditPersonDetails();
    void saveTwoPhotosWithSameNameTest();
    void saveMoveThenOpenAgainTest();
    void recentFilesMenuTest();
    void importLatestMarriagesTest();
    void exportThenImportGedcomTest();
    void photoWindowTest();
    void personListReportTest();
    void timelineReportTest();
    void filePropertiesWindowTest();
    void autoLayoutTest();
    void zoomComboBoxTest();
    void zoomSliderTest();
    void testSelectDescendants();
    void testUndoChangeDiagramSize();
    void testShowAndHideSidebar();
    void testFindDialogLabel();
    void exportImageTest();
    void openExampleTest();
    void personListReportDefaultDateTest();
    void setDiagramFontTest();
    void setFillColorTest();

private slots:
    void testFontWarning();

private:
    TestCaseHelper *m_helper;
    MainForm *m_mainWindow;

    void addPhotoToSelectedPerson();
    DiagramItem *clickToAddPerson();
    DiagramItem *getPersonWithName(const QString &name);
    int getSceneScalePercent();
    void importGedcomFile(const QString &fileName);
};

TestCases::TestCases()
{

}

TestCases::~TestCases()
{
    // Avoid compiler bug.
}

void TestCases::saveTestFileAs(const QString &fileName)
{
    // Create helper.
    if (!m_helper)
    {
        m_helper = new TestCaseHelper();
    }

    // Save the file.
    m_helper->setSaveFileName(fileName);
    QTimer::singleShot(1000, m_helper, SLOT(handleSaveDialog()));

//    QTest::keyClicks(m_mainWindow, "S", Qt::ControlModifier | Qt::ShiftModifier);
    QAction *action = m_mainWindow->findChild<QAction*>("saveAsAction");
    if (!action)
    {
        qDebug() << "Action does not exist.";
        return;
    }
    action->trigger();

    QFileInfo info(fileName);
    QString baseName = info.fileName();
    QString expectedTtle = QString("Genealogy Maker Qt - ") + baseName;
    QString windowTitle = m_mainWindow->windowTitle();
    if (windowTitle != expectedTtle)
    {
        qDebug() << "File not saved OK.";
        qDebug() << " - Actual:" << windowTitle;
        qDebug() << " - Expected:" << expectedTtle;
    }
}

DiagramItem *TestCases::getFirstPerson() const
{
    // Get the list.
    QList<QGraphicsItem *> items = m_mainWindow->getScene()->items();

//    QVERIFY(items.size() >= 1); // Can only use inside test function.
    if (items.empty())
    {
        qDebug() << "Items is empty.";
        return nullptr;
    }

    // Get the person.
    for (QGraphicsItem *item: items) {
        DiagramItem *person = qgraphicsitem_cast<DiagramItem *>(item);
        if (person) {
            return person;
        }
    }

    // Return.
    qDebug() << "No person found.";
    return nullptr;
}

void TestCases::cleanup()
{
    // Performed after each test case.
    m_mainWindow->disconnect();
    m_mainWindow->deleteLater();
    m_mainWindow = nullptr;

    // Delete helper.
    if (m_helper)
    {
        m_helper->deleteLater();
        m_helper = nullptr;
    }

    // Wait for window to be destroyed.
    QTest::qWait(100);
}

void TestCases::init()
{
    // Performed before each test case.
    Q_INIT_RESOURCE(genealogymaker);

    m_helper = nullptr;

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
    openTestFile(getTestInputFilePathFor("smith-new.xml"));

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - smith-new.xml"));
}

void TestCases::testSave()
{
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

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
    openTestFile(getTestInputFilePathFor("bug-1-test.xml"));

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
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

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
    openTestFile(getTestInputFilePathFor("thumbnail-test.xml"));

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
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Open the test file.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName(getTestInputFilePathFor("export-gedcom-test.xml"));

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
    QFile file(getTestInputFilePathFor(saveFileName));
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
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionImportGedcomFile");
    QVERIFY(action);

    // Create the helper.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName(getTestInputFilePathFor("import-gedcom-test.ged"));

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
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

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
    openTestFile(getTestInputFilePathFor("delete-person-test.xml"));

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

#ifndef Q_OS_WIN
    QCOMPARE(oupa->scene(), nullptr); // Causes compilation error on Windows.
#endif

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
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionImportGedcomFile");
    QVERIFY(action);

    // Create the helper.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName(getTestInputFilePathFor("import-gedcom-test.ged"));

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
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Check initial title.
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram"));

    // Import GEDCOM.
    importGedcomFile(getTestInputFilePathFor("import-gedcom-test.ged"));

    // Check title.
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram (Imported from GEDCOM)"));

    // Create new diagram.
    QAction *action = m_mainWindow->findChild<QAction*>("newAction");
    QVERIFY(action);
    action->trigger();

    // Check title.
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram"));

    // Import GEDCOM.
    importGedcomFile(getTestInputFilePathFor("import-gedcom-test.ged"));

    // Check title.
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram (Imported from GEDCOM)"));

    // Open another diagram.
    openTestFile(getTestInputFilePathFor("smith-new.xml"));

    // Check title.
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - smith-new.xml"));
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

void TestCases::testUndoEditPersonDetails()
{
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Add person.
    auto person = clickToAddPerson();
    QString originalDisplayName = person->name();
    QString originalFirstName = person->getFirstName();
    QString originalLastName = person->getLastName();
    QDate originalDateOfBirth = person->getDateOfBirth();
    QString originalPlaceOfBirth = person->getPlaceOfBirth();
    QString originalCountryOfBirth = person->getCountryOfBirth();
    QDate originalDateOfDeath = person->getDateOfDeath();
    QString originalPlaceOfDeath = person->getPlaceOfDeath();
    QString originalGender = person->getGender();
    QString originalBio = person->bio();
    QStringList originalPhotos = person->photos();

    // Select the person.
    m_mainWindow->getScene()->selectAll();

    // Set up helper to close the window.
    auto helper = new DetailsWindowHelper();
    QTimer::singleShot(1000, helper, SLOT(undoEditPersonDetailsTest()));

    // Edit person details.
    QAction *action = m_mainWindow->findChild<QAction*>("viewDetailsAction");
    QVERIFY(action);
    action->trigger();

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }

    // Get new details.
    QString newDisplayName = person->name();
    QString newFirstName = person->getFirstName();
    QString newLastName = person->getLastName();
    QDate newDateOfBirth = person->getDateOfBirth();
    QString newPlaceOfBirth = person->getPlaceOfBirth();
    QString newCountryOfBirth = person->getCountryOfBirth();
    QDate newDateOfDeath = person->getDateOfDeath();
    QString newPlaceOfDeath = person->getPlaceOfDeath();
    QString newGender = person->getGender();
    QString newBio = person->bio();
    QStringList newPhotos = person->photos();

    // Undo.
    action = m_mainWindow->findChild<QAction*>("undoAction");
    QVERIFY(action);
    QVERIFY(action->isEnabled());
    action->trigger();

    // Check details were reverted.
    QCOMPARE(person->name(), originalDisplayName);
    QCOMPARE(person->getFirstName(), originalFirstName);
    QCOMPARE(person->getLastName(), originalLastName);
    QCOMPARE(person->getDateOfBirth(), originalDateOfBirth);
    QCOMPARE(person->getPlaceOfBirth(), originalPlaceOfBirth);
    QCOMPARE(person->getCountryOfBirth(), originalCountryOfBirth);
    QCOMPARE(person->getDateOfDeath(), originalDateOfDeath);
    QCOMPARE(person->getPlaceOfDeath(), originalPlaceOfDeath);
    QCOMPARE(person->getGender(), originalGender);
    QCOMPARE(person->bio(), originalBio);
    QCOMPARE(person->photos(), originalPhotos);

    // Redo.
    action = m_mainWindow->findChild<QAction*>("redoAction");
    QVERIFY(action);
    QVERIFY(action->isEnabled());
    action->trigger();

    // Check details were updated.
    QCOMPARE(person->name(), newDisplayName);
    QCOMPARE(person->getFirstName(), newFirstName);
    QCOMPARE(person->getLastName(), newLastName);
    QCOMPARE(person->getDateOfBirth(), newDateOfBirth);
    QCOMPARE(person->getPlaceOfBirth(), newPlaceOfBirth);
    QCOMPARE(person->getCountryOfBirth(), newCountryOfBirth);
    QCOMPARE(person->getDateOfDeath(), newDateOfDeath);
    QCOMPARE(person->getPlaceOfDeath(), newPlaceOfDeath);
    QCOMPARE(person->getGender(), newGender);
    QCOMPARE(person->bio(), newBio);
    QCOMPARE(person->photos(), newPhotos);
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

DiagramItem *TestCases::getPersonWithName(const QString &name)
{
    DiagramScene *scene = m_mainWindow->getScene();
    QList<QGraphicsItem *> items = scene->items();

    for (auto item: items)
    {
        if (item->type() == DiagramItem::Type)
        {
            DiagramItem *person = qgraphicsitem_cast<DiagramItem *>(item);
            if (person->name() == name)
            {
                return person;
            }
        }
    }

    return nullptr;
}

int TestCases::getSceneScalePercent()
{
    double scale = m_mainWindow->getView()->matrix().m11(); // Assume X and Y scale are the same.
    int percent = scale * 100;

    return percent;
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
#ifdef Q_OS_WIN
    // Workaround for Windows. The file dialog will not populate the name correctly.
    m_mainWindow->open(fileName);
#else
    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("openAction");
//    QVERIFY(action);

    // Create the helper.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName(fileName);

    // Open the file.
    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    action->trigger();
#endif
}

void TestCases::savePhotosTest()
{
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Open the test file.
    openTestFile(getTestInputFilePathFor("save-photos-test.xml"));

    // Get the first person.
    DiagramItem *person = getFirstPerson();
    QVERIFY(person);

    // Save the diagram.
    saveTestFileAs("save-photos-test-updated.xml");

    // Check that directory exists.
    QDir photosDir(getTestInputFilePathFor("save-photos-test-updated-photos"));
    QVERIFY(photosDir.exists());

    // Check that directory for person exists.
    QString personPhotosDirName = person->id().toString();
    QDir personPhotosDir(photosDir.filePath(personPhotosDirName));
    QVERIFY(personPhotosDir.exists());

    // Check that photo is in folder.
    QVERIFY(personPhotosDir.entryList().contains("Photo.png"));
}

void TestCases::addPhotoTest()
{
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Add person.
    auto person = new DiagramItem(DiagramItem::Person, nullptr);
    m_mainWindow->getScene()->addItem(person);

    // Select the person.
    m_mainWindow->getScene()->selectAll();

    // Set up helper to close the window.
    auto helper = new DetailsWindowHelper();
    QTimer::singleShot(1000, helper, SLOT(addPhotoTest()));

    // Show the person details window.
    QTest::keyClicks(m_mainWindow, "D", Qt::ControlModifier);

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }

    // Check that photo was added.
    QCOMPARE(person->photos().size(), 1);
    QVERIFY(person->photos().first().endsWith("Photo.png"));

    // Save the diagram.
    saveTestFileAs("add-photos-test.xml");

    // Check that directory exists.
    QDir photosDir("save-files/add-photos-test-photos");
    QVERIFY(photosDir.exists());

    // Check that directory for person exists.
    QString personPhotosDirName = person->id().toString();
    QDir personPhotosDir(photosDir.filePath(personPhotosDirName));
    QVERIFY(personPhotosDir.exists());

    // Check that photo is in folder.
    QVERIFY(personPhotosDir.entryList().contains("Photo.png"));
}

void TestCases::saveInOtherFolderTest()
{
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Add person.
    auto person = new DiagramItem(DiagramItem::Person, nullptr);
    m_mainWindow->getScene()->addItem(person);

    // Select the person.
    m_mainWindow->getScene()->selectAll();

    // Set up helper to close the window.
    auto helper = new DetailsWindowHelper();
    QTimer::singleShot(1000, helper, SLOT(addPhotoTest()));

    // Show the person details window.
    QTest::keyClicks(m_mainWindow, "D", Qt::ControlModifier);

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }

    // Check that photo was added.
    QCOMPARE(person->photos().size(), 1);
    QVERIFY(person->photos().first().endsWith("Photo.png"));

    // Create folder if required.
    QDir dir("save-files/other-folder");
    dir.mkpath(".");

    // Save the diagram.
    saveTestFileAs("other-folder/save-in-other-folder-test.xml");

    // Check that directory exists.
    QDir photosDir("save-files/other-folder/save-in-other-folder-test-photos");
    QVERIFY(photosDir.exists());

    // Check that directory for person exists.
    QString personPhotosDirName = person->id().toString();
    QDir personPhotosDir(photosDir.filePath(personPhotosDirName));
    QVERIFY(personPhotosDir.exists());

    // Check that photo is in folder.
    QVERIFY(personPhotosDir.entryList().contains("Photo.png"));
}

void TestCases::saveTwoPhotosWithSameNameTest()
{
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Add person.
    auto person = new DiagramItem(DiagramItem::Person, nullptr);
    m_mainWindow->getScene()->addItem(person);

    // Select the person.
    m_mainWindow->getScene()->selectAll();

    // Set up helper to close the window.
    auto helper = new DetailsWindowHelper();
    QTimer::singleShot(1000, helper, SLOT(addTwoPhotosWithSameNameTest()));

    // Show the person details window.
    QTest::keyClicks(m_mainWindow, "D", Qt::ControlModifier);

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }

    // Check that photos were added.
    QCOMPARE(person->photos().size(), 2);
    QVERIFY(person->photos().first().endsWith("Photo.png"));
    QVERIFY(person->photos().last().endsWith("Photo.png"));

    // TODO: Clear the existing photos directory, if it exists.

    // Save the diagram.
    saveTestFileAs("add-two-photos-with-same-name-test.xml");

    // Check that directory exists.
    QDir photosDir("save-files/add-two-photos-with-same-name-test-photos");
    QVERIFY(photosDir.exists());

    // Check that directory for person exists.
    QString personPhotosDirName = person->id().toString();
    QDir personPhotosDir(photosDir.filePath(personPhotosDirName));
    QVERIFY(personPhotosDir.exists());

    // Check that photo is in folder.
    QStringList nameFilters;
    nameFilters << "*.png";
    QCOMPARE(personPhotosDir.entryList(nameFilters).size(), 2);
    QVERIFY(personPhotosDir.entryList(nameFilters).size() >= 2);
}

void TestCases::saveMoveThenOpenAgainTest()
{
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Add person.
    DiagramItem *person = clickToAddPerson();

    // Select the person.
    m_mainWindow->getScene()->selectAll();

    // Add a photo.
    addPhotoToSelectedPerson();

    // Save the diagram.
    const QString originalSaveFileName = "save-move-then-open-again-test.xml";
    saveTestFileAs(originalSaveFileName);

    const QString photosDirName = "save-move-then-open-again-test-photos";

    // Move the diagram files.
    QDir originalSaveFileDir("save-files/");

    // Create new folder.
    QDir newSaveFileDir(originalSaveFileDir.filePath("other-folder"));
    newSaveFileDir.mkpath(".");

    QVERIFY(newSaveFileDir.exists());

    // Move the XML file.
    QString newSaveFileName = newSaveFileDir.absoluteFilePath(originalSaveFileName);
    FileUtils::copyAndReplace(originalSaveFileDir.absoluteFilePath(originalSaveFileName), newSaveFileName);

    QVERIFY(QFile(newSaveFileName).exists());

    // Move the photos directory.
    QString originalPhotosDirName = originalSaveFileDir.absoluteFilePath(photosDirName);
    QString newPhotosDir = newSaveFileDir.absoluteFilePath(photosDirName);
    bool renameOK = FileUtils::moveFolder(originalPhotosDirName, newPhotosDir);

    QVERIFY(renameOK);
    QVERIFY(QFile(newPhotosDir).exists());

    // Open the test file in the new location.
    openTestFile(newSaveFileName);

    // Get the person.
    person = getFirstPerson();

    // Check photos exist.
    for (QString photo: person->photos()) {
        QVERIFY(!photo.isEmpty());
        QVERIFY(QFile(photo).exists());
    }
}

void TestCases::recentFilesMenuTest()
{
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Add person.
    clickToAddPerson();

    // Save.
    saveTestFileAs("recent-files-menu-test.xml");

    // Check that saved file is in "Recent Files" menu.

    // Get the "File" menu.
    QMenu *fileMenu = m_mainWindow->findChild<QMenu*>("menuFile");
    QVERIFY(fileMenu);

    // Get the "Recent Files" menu.
    QMenu *recentFilesMenu = m_mainWindow->findChild<QMenu*>("recentFilesMenu");
    QVERIFY(recentFilesMenu);

    // Show the menu. This is required to load the file list.
//    QTest::mouseClick(fileMenu, Qt::LeftButton); // Does not work!
//    QTest::keyClicks(m_mainWindow, "F", Qt::AltModifier); // Does not work!

    // TODO: Find a way to trigger the "show" event.

    // Get the first action.
    auto actions = recentFilesMenu->actions();
    QVERIFY(actions.size() >= 1);

    QAction *firstAction = actions.first();
    qDebug() << "Action: " << firstAction->data().toString();
}

void TestCases::importLatestMarriagesTest()
{
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionImportGedcomFile");
    QVERIFY(action);

    // Create the helper.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName(getTestInputFilePathFor("Musterstammbaum.ged"));

    // Import the GEDCOM file.
    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    action->trigger();

    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - New Diagram (Imported from GEDCOM)"));

    // Get the items.
    QList<QGraphicsItem *> items = m_mainWindow->getScene()->items();
    QVERIFY(items.size() >= 1);

    // Debug.
//    QTest::qWait(10000);

    // Check the marriage.
    for (auto item: items)
    {
        if (item->type() == DiagramItem::Type)
        {
            // Cast to person.
            DiagramItem *person = qgraphicsitem_cast<DiagramItem *>(item);
            QVERIFY(person);

            // Check if the right person.
            if (person->name() == "Anneliese Mustereisen")
            {
                // Check person is married.
                QVERIFY(person->isMarried());

                // Check spouse exists.
                DiagramItem *spouse = person->getSpouse();
                QVERIFY(spouse);

                // Check spouse is correct.
                QCOMPARE(spouse->name(), QString("Theodor Mustermann"));

                // Exit loop.
                break;
            }
        }
    }
}

void TestCases::exportThenImportGedcomTest()
{
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

    // Import original GEDCOM file.
    importGedcomFile(getTestInputFilePathFor("Musterstammbaum.ged"));

    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionExportGedcomFile");
    QVERIFY(action);

    // Export to GEDCOM.
    const QString saveFileName = "Musterstammbaum-exported.ged";
    m_helper->setSaveFileName(saveFileName);
    QTimer::singleShot(1000, m_helper, SLOT(handleSaveDialog()));
    action->trigger();

    // Check that the exported file exists.
    QFileInfo fileInfo(QString("save-files/") + saveFileName);
    QVERIFY(fileInfo.exists());

    // Import the exported GEDCOM file.
    importGedcomFile(fileInfo.absoluteFilePath());
}

void TestCases::photoWindowTest()
{
    // Open test file.
    openTestFile(getTestInputFilePathFor("photo-window-test.xml"));

    // Get the first person.
    DiagramItem *person = getFirstPerson();
    QVERIFY(person);

    // Select the person.
    person->setSelected(true);

    // Create helper.
    auto helper = new DetailsWindowHelper();
    QTimer::singleShot(1000, helper, SLOT(viewPhotosTest()));

    // Show the details window.
    QAction *action = m_mainWindow->findChild<QAction*>("viewDetailsAction");
    QVERIFY(action);
    action->trigger();

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }
}

void TestCases::personListReportTest()
{
    // Open test file.
    openTestFile(getTestInputFilePathFor("smith-new.xml"));

    // Create the report.
    QAction *action = m_mainWindow->findChild<QAction*>("actionPersonListReport");
    QVERIFY(action);
    action->trigger();

    // Debug.
    QTest::qWait(1000);
}

void TestCases::timelineReportTest()
{
    // Open test file.
    openTestFile(getTestInputFilePathFor("smith-new.xml"));

    // Create the report.
    QAction *action = m_mainWindow->findChild<QAction*>("actionTimelineReport");
    QVERIFY(action);
    action->trigger();

    // Debug.
    QTest::qWait(1000);
}

void TestCases::filePropertiesWindowTest()
{
    // Open test file.
    openTestFile(getTestInputFilePathFor("smith-new.xml"));

    // Set up helper to check the window labels.
    auto helper = new FilePropertiesDialogHelper();
    QTimer::singleShot(1000, helper, SLOT(checkLabelsCorrect()));

    // Show the window.
    QAction *action = m_mainWindow->findChild<QAction*>("actionFileProperties");
    QVERIFY(action);
    action->trigger();

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }
}

void TestCases::autoLayoutTest()
{
    // Open test file.
    openTestFile(getTestInputFilePathFor("smith-new.xml"));

    // Trigger the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionAutoLayout");
    QVERIFY(action);
    action->trigger();

    // Undo.
    action = m_mainWindow->findChild<QAction*>("undoAction");
    QVERIFY(action);
    QVERIFY(action->isEnabled());
    action->trigger();

    // Redo.
    action = m_mainWindow->findChild<QAction*>("redoAction");
    QVERIFY(action);
    QVERIFY(action->isEnabled());
    action->trigger();
}

void TestCases::openExampleTest()
{
#ifdef Q_OS_WIN
    QSKIP("This test does not work on Windows yet.");
#endif

//    QSKIP("There is an outstanding issue with this test. Skipping...");

    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionOpenExample");
    QVERIFY(action);

    // Create the helper.
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName("example-genealogy.xml");

    // Open the file.
    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    action->trigger();

    // Check that the file opened OK.
    QCOMPARE(m_mainWindow->windowTitle(), QString("Genealogy Maker Qt - example-genealogy.xml"));
}

void TestCases::personListReportDefaultDateTest()
{
    // Open test file.
    openTestFile(getTestInputFilePathFor("report-date-test.xml"));

    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionPersonListReport");
    QVERIFY(action);

    // Create the helper.
    auto helper = new ReportWindowHelper();
    QTimer::singleShot(1000, helper, SLOT(testDefaultDates()));

    // Run the report.
    action->trigger();
}

void TestCases::setDiagramFontTest()
{
    QSKIP("This does not correctly set the font preference. Skipping...");

    // Open example file.
    openTestFile(getTestInputFilePathFor("smith-new.xml"));

    // Get the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionPreferences");
    QVERIFY(action);

    // Create the helper.
    auto helper = new PreferencesWindowHelper();
    QTimer::singleShot(1000, helper, SLOT(testSetDiagramFont()));

    // Show the dialog.
    action->trigger();

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }

    // Check values.
    QFont font = getFirstPerson()->textItem()->font();
    QCOMPARE(font.family(), QString("Serif"));
    QCOMPARE(font.pointSize(), 10);
}

void TestCases::setFillColorTest()
{
    // Skip this test for now.
    QSKIP("Cannot seem to access the color dialog popup.");

    // Add a person to the diagram.
    DiagramItem *person = clickToAddPerson();

    // Create the helper.
    auto helper = new FillColorDialogHelper();
    QTimer::singleShot(2000, helper, SLOT(setFillColor()));

    // Person should be selected. Click fill color button.
    QToolButton *fillColorToolButton = m_mainWindow->findChild<QToolButton*>("fillColorToolButton");
    fillColorToolButton->click();

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }

    // Check fill color is set.
    QCOMPARE(person->brush().color(), QColor(Qt::blue));

    // Debug.
    QTest::qWait(3000);
}

void TestCases::testFontWarning()
{
    // Test GitHub issue #72.
    QSKIP("Skipping to avoid changing preferences.");

    // Clear the default font settings.
    QSettings settings;
    settings.remove("diagram/fontFamily");
    settings.remove("diagram/fontSize");
    m_mainWindow->getScene()->loadPreferences();

    // There should be no warning here.
    clickToAddPerson();
}

void TestCases::zoomComboBoxTest()
{
    // Check default zoom.
    QCOMPARE(getSceneScalePercent(), 100);

    // Change the combo-box value.
    QComboBox *zoomComboBox = m_mainWindow->findChild<QComboBox*>("zoomComboBox");
    QVERIFY(zoomComboBox);

    int itemIndex = zoomComboBox->findText("150%");
    QVERIFY(itemIndex != -1);

    zoomComboBox->setCurrentIndex(itemIndex);

    // Debug.
//    QTest::qWait(1000);

    // TODO: Force the "editing finished" signal to fire.

    // Press Enter in the combo-box. This does not work!
    QTest::keyClick(zoomComboBox, Qt::Key_Return);

    // Set the focus elsewhere. This does not work!
    m_mainWindow->getView()->setFocus();

    // Debug.
//    QTest::qWait(1000);

    // Check the scale has changed. Does not work yet!
//    QCOMPARE(getSceneScalePercent(), 150);
}

void TestCases::zoomSliderTest()
{
    // Check default zoom.
    QCOMPARE(getSceneScalePercent(), 100);

    // Change the slider value.
    QSlider *zoomSlider = m_mainWindow->findChild<QSlider*>("zoomSlider");
    QVERIFY(zoomSlider);

    int newValue = 150;
    zoomSlider->setValue(newValue);

    // Check the scale has changed.
    QCOMPARE(getSceneScalePercent(), newValue);

    // Check the combo-box value has also changed.
    QComboBox *zoomComboBox = m_mainWindow->findChild<QComboBox*>("zoomComboBox");
    QVERIFY(zoomComboBox);

    QCOMPARE(zoomComboBox->currentText(), tr("150%"));
}

void TestCases::testSelectDescendants()
{
    // Open test file.
    openTestFile(getTestInputFilePathFor("smith-new.xml"));

    // Find the person.
    DiagramItem *ouma = getPersonWithName("Ouma");
    QVERIFY(ouma);

    // Select the person.
    ouma->setSelected(true);

    // Select all descendants.
    QAction *action = m_mainWindow->findChild<QAction*>("actionSelectDescendants");
    QVERIFY(action);
    action->trigger();

    // Find the descendant.
    DiagramItem *walter = getPersonWithName("Walter Smith");
    QVERIFY(walter);

    // Check it is selected.
    QVERIFY(walter->isSelected());
}

void TestCases::testUndoChangeDiagramSize()
{
    // Store original size.
    auto sizeBefore = m_mainWindow->getScene()->sceneRect().size();

    // Create helper.
    auto helper = new DiagramSizeDialogHelper();
    QTimer::singleShot(1000, helper, SLOT(changeDiagramSize()));

    // Trigger the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionChangeSize");
    QVERIFY(action);
    action->trigger();

    // Store new size.
    auto sizeAfter = m_mainWindow->getScene()->sceneRect().size();

    // Undo.
    action = m_mainWindow->findChild<QAction*>("undoAction");
    QVERIFY(action);
    QVERIFY(action->isEnabled());
    action->trigger();

    QCOMPARE(m_mainWindow->getScene()->sceneRect().size(), sizeBefore);

    // Redo.
    action = m_mainWindow->findChild<QAction*>("redoAction");
    QVERIFY(action);
    QVERIFY(action->isEnabled());
    action->trigger();

    QCOMPARE(m_mainWindow->getScene()->sceneRect().size(), sizeAfter);
}

void TestCases::testShowAndHideSidebar()
{
    // Check sidebar is visible.
    QToolBox *toolBox = m_mainWindow->findChild<QToolBox*>("toolBox");
    QVERIFY(toolBox);
    QVERIFY(toolBox->isVisible());

    // Hide.
    QAction *action = m_mainWindow->findChild<QAction*>("showSideBarAction");
    QVERIFY(action);
    action->trigger();
    QVERIFY(!toolBox->isVisible());

    // Show.
    action->trigger();
    QVERIFY(toolBox->isVisible());
}

void TestCases::testFindDialogLabel()
{
    // Set up helper to check the window labels.
    auto helper = new FindDialogHelper();
    QTimer::singleShot(1000, helper, SLOT(testLabel()));

    // Show "Find" dialog.
    QAction *action = m_mainWindow->findChild<QAction*>("findAction");
    QVERIFY(action);
    action->trigger();

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }
}

void TestCases::exportImageTest()
{
    // Smoke test: just trigger the action.
    QAction *action = m_mainWindow->findChild<QAction*>("actionFileExportImage");
    QVERIFY(action);
//    action->trigger(); // TODO: The dialog does not close automatically. Must close manually.
}

void TestCases::addPhotoToSelectedPerson()
{
    auto helper = new DetailsWindowHelper();
    QTimer::singleShot(1000, helper, SLOT(addTwoPhotosWithSameNameTest()));

    // Show the person details window.
    QTest::keyClicks(m_mainWindow, "D", Qt::ControlModifier);

    // Wait till helper is done.
    while (!helper->isFinished())
    {
        QTest::qWait(1000);
    }

    // Check that photos were added.
//    QCOMPARE(person->photos().size(), 2);
//    QVERIFY(person->photos().first().endsWith("Photo.png"));
//    QVERIFY(person->photos().last().endsWith("Photo.png"));
}

QTEST_MAIN(TestCases)
#include "testcases.moc"
