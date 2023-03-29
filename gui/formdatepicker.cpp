#include "formdatepicker.h"
#include "ui_formdatepicker.h"

#include <QCalendarWidget>

FormDatePicker::FormDatePicker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDatePicker)
{
    ui->setupUi(this);

    // Create calendar widget.
    m_calendarWidget = new QCalendarWidget();
    m_calendarWidget->setWindowFlags(Qt::Popup);
    connect(m_calendarWidget, SIGNAL(activated(QDate)), this, SLOT(onCalendarDatePicked(QDate)));
    connect(m_calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(onCalendarDatePicked(QDate)));
}

FormDatePicker::~FormDatePicker()
{
    delete ui;
}

QDate FormDatePicker::getDate() const
{
    return m_calendarWidget->selectedDate();
}

void FormDatePicker::setDate(const QDate &date)
{
    ui->lineEditDate->setText(date.toString(Qt::ISODate));
}

void FormDatePicker::on_pushButtonCalendar_clicked()
{
    // Show the calendar as a popup.
    auto calendarPos = ui->lineEditDate->mapToGlobal(QPoint(0, ui->pushButtonCalendar->height()));
    m_calendarWidget->move(calendarPos);
    m_calendarWidget->show();
}

void FormDatePicker::onCalendarDatePicked(const QDate &date)
{
    ui->lineEditDate->setText(date.toString(Qt::ISODate));

    m_calendarWidget->hide();
}

void FormDatePicker::on_pushButtonClear_clicked()
{
    ui->lineEditDate->clear();
}
