#include "mainform.h"
#include "ui_mainform.h"

#include <QDesktopWidget>

MainForm::MainForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);
}

MainForm::~MainForm()
{
    delete ui;
}

void MainForm::moveToCenter()
{
    //
    // Center the window on the screen.
    //
    auto geometry = QApplication::desktop()->screenGeometry();
    auto halfSize = size() / 2.0;
    QPoint offset(halfSize.width(), halfSize.height());
    move(geometry.center() - offset);
}
