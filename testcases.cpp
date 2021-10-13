#include <QtWidgets>
#include <QtTest/QtTest>

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

private slots:
    void handleOpenDialog();
    void handleSaveDialog();

private:
    QString m_openFileName;
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
        dialog->selectFile("saved-diagram.xml");
        QTimer::singleShot(0, dialog, SLOT(accept()));
    }
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

    QCOMPARE(m_mainWindow->windowTitle(), QString("New Diagram - Genealogy Maker Qt"));
}

void TestCases::testOpen()
{
    m_helper = new TestCaseHelper();
    m_helper->setOpenFileName("van-zijl-new.xml");

    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    QTest::keyClicks(m_mainWindow, "O", Qt::ControlModifier);

    QCOMPARE(m_mainWindow->windowTitle(), QString("van-zijl-new.xml - Genealogy Maker Qt"));
}

void TestCases::testSave()
{
    m_helper = new TestCaseHelper();
    QTimer::singleShot(1000, m_helper, SLOT(handleSaveDialog()));
    QTest::keyClicks(m_mainWindow, "S", Qt::ControlModifier);

    QCOMPARE(m_mainWindow->windowTitle(), QString("saved-diagram.xml - Genealogy Maker Qt"));
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

    QCOMPARE(m_mainWindow->windowTitle(), QString("bug-1-test.xml - Genealogy Maker Qt"));

    // Select all.
    QTest::keyClicks(m_mainWindow, "A", Qt::ControlModifier);

    // TODO: Check that all are selected. For this I must add a getter for the "scene" field.

    // Delete all.
    QTest::keyClick(m_mainWindow, Qt::Key_Delete);

    // TODO: Check that all are deleted. For this I must add a getter for the "scene" field.
}

QTEST_MAIN(TestCases)
#include "testcases.moc"
