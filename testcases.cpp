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

#include <QtWidgets>
#include <QtTest/QtTest>

#include "gui/mainform.h"

#include <QObject>

class TestCases: public QObject
{
    Q_OBJECT

public:
    virtual ~TestCases();

private slots:
    void testGui();
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
    QApplication::processEvents();

    mainWindow.activateWindow();
    QApplication::processEvents();
    QTest::qWait(2000);

    QTest::keyClicks(&mainWindow, "N", Qt::ControlModifier);
    //QApplication::processEvents();
    //QTest::qWait(2000);

    QCOMPARE(mainWindow.windowTitle(), QString("New Diagram - Genealogy Maker Qt"));
}

//int main(int argc, char* argv[])
//{
////#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
////    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
////    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
////#endif
////    Application app(argc, argv);
////    app.setApplicationName("KeePassXC");
////    app.setApplicationVersion(KEEPASSXC_VERSION);
////    app.setQuitOnLastWindowClosed(false);
////    app.setAttribute(Qt::AA_Use96Dpi, true);
////    app.applyTheme();
////    QTEST_DISABLE_KEYPAD_NAVIGATION
////    TestGui tc;
////    QTEST_SET_MAIN_SOURCE_PATH
//    QApplication app(argc, argv);
//    TestCases cases;
//    return QTest::qExec(&cases, argc, argv);
//}

QTEST_MAIN(TestCases)
#include "testcases.moc"
