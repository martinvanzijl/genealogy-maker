#ifndef FORMDATEPICKER_H
#define FORMDATEPICKER_H

#include <QWidget>

namespace Ui {
class FormDatePicker;
}

class QCalendarWidget;

class FormDatePicker : public QWidget
{
    Q_OBJECT

public:
    explicit FormDatePicker(QWidget *parent = 0);
    ~FormDatePicker();

    QDate getDate() const;

private slots:
    void on_pushButtonCalendar_clicked();

    void onCalendarDatePicked(const QDate &date);

    void on_pushButtonClear_clicked();

private:
    Ui::FormDatePicker *ui;

    QCalendarWidget *m_calendarWidget;
};

#endif // FORMDATEPICKER_H
