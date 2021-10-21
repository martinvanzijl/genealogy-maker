#ifndef PREFERENCESWINDOW_H
#define PREFERENCESWINDOW_H

#include <QMainWindow>

namespace Ui {
class PreferencesWindow;
}

class PreferencesWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PreferencesWindow(QWidget *parent = 0);
    ~PreferencesWindow();

    void loadPreferences();

private slots:
    void on_pushButtonApply_clicked();

    void on_pushButtonClose_clicked();

private:
    Ui::PreferencesWindow *ui;

    void apply();

signals:
    void preferencesChanged();
};

#endif // PREFERENCESWINDOW_H
