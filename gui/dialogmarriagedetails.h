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

private:
    Ui::DialogMarriageDetails *ui;
};

#endif // DIALOGMARRIAGEDETAILS_H
