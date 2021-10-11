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

private slots:
    void handleOpenDialog();
    void handleSaveDialog();
};

TestCaseHelper::~TestCaseHelper()
{
    // Avoid compiler error.
}

void TestCaseHelper::handleOpenDialog()
{
    QWidget *activeModalWidget = QApplication::activeModalWidget();
    QFileDialog *dialog = dynamic_cast<QFileDialog *>(activeModalWidget);

    if (dialog)
    {
        dialog->selectFile("van-zijl-new.xml");
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
    void testGui();
    void testOpen();
    void testSave();

private:
    TestCaseHelper *m_helper;
};

TestCases::~TestCases()
{
    // Avoid compiler bug.
}

void TestCases::testGui()
{
    Q_INIT_RESOURCE(genealogymaker);

    MainForm mainWindow;
    mainWindow.moveToCenter();
    mainWindow.show();
    mainWindow.activateWindow();

    QTest::qWaitForWindowActive(&mainWindow);

    QTest::keyClicks(&mainWindow, "N", Qt::ControlModifier);

    QCOMPARE(mainWindow.windowTitle(), QString("New Diagram - Genealogy Maker Qt"));
}

void TestCases::testOpen()
{
    Q_INIT_RESOURCE(genealogymaker);

    MainForm mainWindow;
    mainWindow.moveToCenter();
    mainWindow.show();
    mainWindow.activateWindow();

    QTest::qWaitForWindowActive(&mainWindow);

    m_helper = new TestCaseHelper();
    QTimer::singleShot(1000, m_helper, SLOT(handleOpenDialog()));
    QTest::keyClicks(&mainWindow, "O", Qt::ControlModifier);

    QCOMPARE(mainWindow.windowTitle(), QString("van-zijl-new.xml - Genealogy Maker Qt"));
}

void TestCases::testSave()
{
    Q_INIT_RESOURCE(genealogymaker);

    MainForm mainWindow;
    mainWindow.moveToCenter();
    mainWindow.show();
    mainWindow.activateWindow();

    QTest::qWaitForWindowActive(&mainWindow);

    m_helper = new TestCaseHelper();
    QTimer::singleShot(1000, m_helper, SLOT(handleSaveDialog()));
    QTest::keyClicks(&mainWindow, "S", Qt::ControlModifier);

    QCOMPARE(mainWindow.windowTitle(), QString("saved-diagram.xml - Genealogy Maker Qt"));
}

QTEST_MAIN(TestCases)
#include "testcases.moc"
