#include "dialogfileproperties.h"
#include "ui_dialogfileproperties.h"

#include <diagramscene.h>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>

DialogFileProperties::DialogFileProperties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFileProperties)
{
    ui->setupUi(this);
}

DialogFileProperties::~DialogFileProperties()
{
    delete ui;
}

void DialogFileProperties::setDiagram(DiagramScene *&scene)
{
    // Populate.
    QString personCount = QString::number(scene->personCount());
    ui->labelNumberOfPeopleValue->setText(personCount);

    QString relationshipCount = QString::number(scene->relationshipCount());
    ui->labelNumberOfRelationshipsValue->setText(relationshipCount);

    QString marriageCount = QString::number(scene->marriageCount());
    ui->labelNumberOfMarriagesValue->setText(marriageCount);
}

void DialogFileProperties::setFile(const QString &filePath)
{
    // Clear existing.
    clear();

    // Exit if path is empty.
    if (filePath.isEmpty()) {
        return;
    }

    // Get file information.
    QFileInfo info(filePath);

    // Check file exists.
    if (!info.exists()) {
        return;
    }

    // Populate.
    ui->labelNameValue->setText(info.fileName());
    ui->labelFolderValue->setText(info.absoluteDir().absolutePath());
    ui->labelTimestampValue->setText(info.lastModified().toString(Qt::ISODate));
}

void DialogFileProperties::on_pushButtonClose_clicked()
{
    close();
}

void DialogFileProperties::clear()
{
    ui->labelNameValue->clear();
    ui->labelFolderValue->clear();
    ui->labelTimestampValue->clear();
}
