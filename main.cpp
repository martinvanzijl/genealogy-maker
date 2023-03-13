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

//#include "mainwindow.h"
#include "gui/mainform.h"

#include <QApplication>

int main(int argv, char *args[])
{
    Q_INIT_RESOURCE(genealogymaker);

    QApplication app(argv, args);
//    MainWindow mainWindow;
    MainForm mainWindow;
//    mainWindow.setGeometry(100, 100, 800, 500);
    mainWindow.moveToCenter();
    mainWindow.show();

    // Load diagram if passed as parameter.
    auto argList = app.arguments();
    if (argList.size() >= 2)
    {
        QString fileName = argList[1];
        mainWindow.open(fileName);
    }

    return app.exec();
}
