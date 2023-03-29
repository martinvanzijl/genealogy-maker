#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QMainWindow>

namespace Ui {
class HelpWindow;
}

class HelpWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HelpWindow(QWidget *parent = 0);
    ~HelpWindow();

private slots:
    void on_pushButtonClose_clicked();

private:
    Ui::HelpWindow *ui;
    void loadHelpFile();
};

#endif // HELPWINDOW_H
