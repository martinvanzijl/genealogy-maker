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

#ifndef VIEWPHOTOWINDOW_H
#define VIEWPHOTOWINDOW_H

#include <QMainWindow>
#include <QImage>

class QAction;
class QFrame;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class ViewPhotoWindow : public QMainWindow
{
    Q_OBJECT

public:
    ViewPhotoWindow(QWidget *parent = nullptr);
    bool loadFile(const QString &);

    bool photoLoadedOK() const;
    void setPhoto(const QString &fileName);
    void setPhotoIndex(int index);
    void setPhotoList(const QStringList &list);

protected:
    void resizeEvent(QResizeEvent *ev) override;

private slots:
    void saveAs();
    void copy();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();

    void goToNext();
    void goToPrevious();
    void toggleNavigationPanel();

private:
    void createActions();
    void createMenus();
    void updateActions();
    bool saveFile(const QString &fileName);
    void setImage(const QImage &newImage);
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    QImage image;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    QFrame *navigationPanel;
    double scaleFactor;

    QAction *saveAsAct;
    QAction *copyAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    QAction *goToNextAct;
    QAction *goToPreviousAct;
    QAction *showNavigationPanelAct;

    bool m_photoLoadedOK;
    QStringList m_photoList;
    int m_currentPhotoIndex;
};

#endif // VIEWPHOTOWINDOW_H
