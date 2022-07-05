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

    // Load sidebar button setting.
    bool showSidebarCollapseButton = settings.value("interface/showSidebarCollapseButton", false).toBool();
    ui->checkBoxShowCollapseButton->setChecked(showSidebarCollapseButton);

    // Load "Find" window transparency setting.
    bool findWindowTransparent = settings.value("interface/findWindowTransparent", false).toBool();
    ui->checkBoxFindWindowTransparent->setChecked(findWindowTransparent);

    // Load "remove invalid files" setting.
    bool removeInvalidFiles = settings.value("interface/removeInvalidFiles", false).toBool();
    ui->checkBoxRemoveInvalidFiles->setChecked(removeInvalidFiles);
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

    // Store the sidebar button setting.
    bool showSidebarCollapseButton =  ui->checkBoxShowCollapseButton->isChecked();
    settings.setValue("interface/showSidebarCollapseButton", showSidebarCollapseButton);

    // Store the "Find" window transparency setting.
    bool findWindowTransparent =  ui->checkBoxFindWindowTransparent->isChecked();
    settings.setValue("interface/findWindowTransparent", findWindowTransparent);

    // Store the "remove invalid files" setting.
    bool removeInvalidFiles =  ui->checkBoxRemoveInvalidFiles->isChecked();
    settings.setValue("interface/removeInvalidFiles", removeInvalidFiles);

    // Send signal.
    emit preferencesChanged();
}

void PreferencesWindow::on_pushButtonClose_clicked()
{
    close();
}
