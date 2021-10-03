#ifndef DIALOGMARRIAGEDETAILS_H
#define DIALOGMARRIAGEDETAILS_H

#include <QDialog>

namespace Ui {
class DialogMarriageDetails;
}

class MarriageItem;

class DialogMarriageDetails : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMarriageDetails(QWidget *parent = 0);
    ~DialogMarriageDetails();

    void setMarriage(MarriageItem *marriage);
    void save();

private slots:
    void on_pushButtonSave_clicked();

    void on_pushButtonClose_clicked();

    void on_checkBoxDateUnknown_stateChanged(int state);

    void on_dateEditDate_dateChanged(const QDate &date);

private:
    void setTextGrayedOut(QWidget *widget, bool grayedOut);

    Ui::DialogMarriageDetails *ui;
    MarriageItem *m_marriage;
};

#endif // DIALOGMARRIAGEDETAILS_H
