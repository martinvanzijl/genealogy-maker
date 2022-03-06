#include "timelinereportwindow.h"
#include "ui_timelinereportwindow.h"

#include <QDebug>

#include "diagramitem.h"
#include "diagramscene.h"
#include "marriageitem.h"

TimelineReportWindow::TimelineReportWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TimelineReportWindow)
{
    ui->setupUi(this);

    // Center the window on the parent.
    if (parent)
    {
        auto geometry = parent->geometry();
        auto halfSize = size() / 2.0;
        QPoint offset(halfSize.width(), halfSize.height());
        move(geometry.center() - offset);
    }
}

TimelineReportWindow::~TimelineReportWindow()
{
    delete ui;
}

void TimelineReportWindow::createReportFor(DiagramScene *scene)
{
    for (auto item: scene->items()) {
        if (item->type() == MarriageItem::Type) {
            // Cast.
            MarriageItem *marriage = dynamic_cast<MarriageItem*> (item);

            // Add event.
            QString description = marriage->personLeft()->name() + " and "
                    + marriage->personRight()->name() + " married.";
            addRow(marriage->getDate(), "Marriage", description);
        }
        else if (item->type() == DiagramItem::Type) {
            // Cast.
            DiagramItem *person = dynamic_cast<DiagramItem*> (item);

            // Get events.
            person->getDateOfBirth();
            person->getDateOfDeath();

            // Create row.
            addRow(person->getDateOfBirth(), "Birth", person->name() + " born.");
            addRow(person->getDateOfDeath(), "Death", person->name() + " died.");
        }
    }

    // Resize columns.
    ui->tableWidgetReport->resizeColumnsToContents();

    // Order by time.
    ui->tableWidgetReport->sortByColumn(0, Qt::AscendingOrder);
}

void TimelineReportWindow::addCell(int row, int column, const QString &value)
{
    QTableWidgetItem *cell = new QTableWidgetItem(value);
    ui->tableWidgetReport->setItem(row, column, cell);
}

void TimelineReportWindow::addCell(int row, int column, const QDate &value)
{
    // Use ISO date to make order correct.
    addCell(row, column, value.toString(Qt::ISODate));
}

void TimelineReportWindow::addRow(const QDate &date, const QString &type, const QString &description)
{
    // Create row.
    int row = ui->tableWidgetReport->rowCount();
    ui->tableWidgetReport->setRowCount(row + 1);

    // Add cells.
    int column = 0;

    addCell(row, column++, date);
    addCell(row, column++, type);
    addCell(row, column++, description);
}

void TimelineReportWindow::on_pushButtonClose_clicked()
{
    close();
}
