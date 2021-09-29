#ifndef DIALOGPERSONDETAILS_H
#define DIALOGPERSONDETAILS_H

#include <QDialog>

class DiagramItem;

namespace Ui {
class DialogPersonDetails;
}

class DialogPersonDetails : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPersonDetails(QWidget *parent = 0);
    ~DialogPersonDetails();

    void setItem(DiagramItem *item);

private slots:
    void on_pushButtonClose_clicked();

    void on_pushButtonSave_clicked();

    void on_pushButtonAddPhoto_clicked();

    void on_pushButtonRemovePhoto_clicked();

private:
    void save();

    Ui::DialogPersonDetails *ui;
};

#endif // DIALOGPERSONDETAILS_H
