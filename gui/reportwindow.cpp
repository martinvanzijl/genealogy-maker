#include "reportwindow.h"
#include "ui_reportwindow.h"

#include "diagramitem.h"
#include "diagramscene.h"

ReportWindow::ReportWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReportWindow)
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

ReportWindow::~ReportWindow()
{
    delete ui;
}

void ReportWindow::createReportFor(DiagramScene *scene)
{
    int row = 0;

    for (auto item: scene->items()) {

        // Skip if not a person.
        if (item->type() != DiagramItem::Type) {
            continue;
        }

        // Cast.
        DiagramItem *person = dynamic_cast<DiagramItem*> (item);

        // Create row.
        ui->tableWidgetReport->setRowCount(row + 1);

        // Add cells.
        int column = 0;

        addCell(row, column++, person->getFirstName());
        addCell(row, column++, person->getLastName());
        addCell(row, column++, person->name());
        if (person->getDateOfBirth() == DiagramItem::defaultDateOfBirth()) {
            addCell(row, column++, "");
        }
        else {
            addCell(row, column++, person->getDateOfBirth());
        }
        addCell(row, column++, person->getPlaceOfBirth());
        addCell(row, column++, person->getCountryOfBirth());
        if (person->getDateOfDeath() == DiagramItem::defaultDateOfDeath()) {
            addCell(row, column++, "");
        }
        else {
            addCell(row, column++, person->getDateOfDeath());
        }
        addCell(row, column++, person->getPlaceOfDeath());
        addCell(row, column++, person->getGender());

        // Go to next row.
        ++row;
    }

    // Resize columns.
    ui->tableWidgetReport->resizeColumnsToContents();
}

void ReportWindow::addCell(int row, int column, const QString &value)
{
    QTableWidgetItem *cell = new QTableWidgetItem(value);
    ui->tableWidgetReport->setItem(row, column, cell);
}

void ReportWindow::addCell(int row, int column, const QDate &value)
{
    addCell(row, column, value.toString());
}

void ReportWindow::on_pushButtonClose_clicked()
{
    close();
}
