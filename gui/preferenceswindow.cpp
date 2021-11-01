#include "preferenceswindow.h"
#include "ui_preferenceswindow.h"

#include <QSettings>

PreferencesWindow::PreferencesWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PreferencesWindow)
{
    ui->setupUi(this);

    //
    // Center the window on the parent.
    //
    if (parent)
    {
        auto geometry = parent->geometry();
        auto halfSize = size() / 2.0;
        QPoint offset(halfSize.width(), halfSize.height());
        move(geometry.center() - offset);
    }
}

PreferencesWindow::~PreferencesWindow()
{
    delete ui;
}

void PreferencesWindow::loadPreferences()
{
    // Load arrow line width.
    QSettings settings;
    int arrowLineWidth = settings.value("diagram/arrowLineWidth", 2).toInt();
    ui->spinBoxArrowLineThickness->setValue(arrowLineWidth);

    // Load thumbnail setting.
    bool showThumbnails = settings.value("diagram/showThumbnails", false).toBool();
    ui->checkBoxShowTumbnail->setChecked(showThumbnails);
}

void PreferencesWindow::on_pushButtonApply_clicked()
{
    apply();
//    close();
}

void PreferencesWindow::apply()
{
    // Save arrow line width.
    QSettings settings;
    int arrowLineWidth = ui->spinBoxArrowLineThickness->value();
    settings.setValue("diagram/arrowLineWidth", arrowLineWidth);

    // Store the thumbnail setting.
    bool showThumbnails =  ui->checkBoxShowTumbnail->isChecked();
    settings.setValue("diagram/showThumbnails", showThumbnails);

    // Send signal.
    emit preferencesChanged();
}

void PreferencesWindow::on_pushButtonClose_clicked()
{
    close();
}