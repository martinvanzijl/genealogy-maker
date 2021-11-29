#include <QtWidgets>
#include <QtTest/QtTest>

#include "diagramitem.h"
#include "diagramscene.h"
#include "gui/mainform.h"

#include <QDebug>
#include <QObject>

// =============================================================================
// TestCaseHelper
// =============================================================================

class TestCaseHelper : public QObject
{
    Q_OBJECT

public:
    virtual ~TestCaseHelper();
    void setOpenFileName(const QString &fileName);
    void setSaveFileName(const QString &saveFileName);

private slots:
    void handleOpenDialog();
    void handleSaveDialog();

private:
    QString m_openFileName;
    QString m_saveFileName;
};

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
}

void TestCaseHelper::setSaveFileName(const QString &saveFileName)
{
m_saveFileName = saveFileName;
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
    void testNew();
    void testOpen();
    void testSave();
    void testExit();
    void testBug1();
    void volumeTest();

private:
    TestCaseHelper *m_helper;
    MainForm *m_mainWindow;
};

TestCases::~TestCases()
{
    // Avoid compiler bug.
}

void TestCases::cleanup()
{
    // Performed after each test case.
    delete m_mainWindow;
    m_mainWindow = nullptr;
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

QTEST_MAIN(TestCases)
#include "testcases.moc"
