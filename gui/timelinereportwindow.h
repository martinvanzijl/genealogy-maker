#ifndef TIMELINEREPORTWINDOW_H
#define TIMELINEREPORTWINDOW_H

#include <QMainWindow>

class DiagramScene;

namespace Ui {
class TimelineReportWindow;
}

class TimelineReportWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TimelineReportWindow(QWidget *parent = 0);
    ~TimelineReportWindow();

    void createReportFor(DiagramScene *scene);

private slots:
    void on_pushButtonClose_clicked();

private:
    Ui::TimelineReportWindow *ui;

    void addCell(int row, int column, const QString &value);
    void addCell(int row, int column, const QDate &value);
    void addRow(const QDate &date, const QString &type, const QString &description);
};

#endif // TIMELINEREPORTWINDOW_H
