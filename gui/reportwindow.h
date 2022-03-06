#ifndef REPORTWINDOW_H
#define REPORTWINDOW_H

#include <QMainWindow>

class DiagramScene;

namespace Ui {
class ReportWindow;
}

class ReportWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReportWindow(QWidget *parent = 0);
    ~ReportWindow();

    void createReportFor(DiagramScene *scene);

private slots:
    void on_pushButtonClose_clicked();

private:
    Ui::ReportWindow *ui;

    void addCell(int row, int column, const QString &value);
    void addCell(int row, int column, const QDate &value);
};

#endif // REPORTWINDOW_H
