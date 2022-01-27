#ifndef DIALOGPERSONDETAILS_H
#define DIALOGPERSONDETAILS_H

#include <QDialog>

class DiagramItem;

class QListWidgetItem;

namespace Ui {
class DialogPersonDetails;
}

class DialogViewPhoto;

class DialogPersonDetails : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPersonDetails(QWidget *parent = 0);
    ~DialogPersonDetails();

    void setItem(DiagramItem *item);
    void viewDefaultTab();
    void setXmlFile(const QString &xmlFile);

private slots:
    void on_pushButtonClose_clicked();

    void on_pushButtonSave_clicked();

    void on_pushButtonAddPhoto_clicked();

    void on_pushButtonRemovePhoto_clicked();

    void on_listWidgetPhotos_itemDoubleClicked(QListWidgetItem *item);

//    void on_dateEditBirth_dateChanged(const QDate &date);

//    void on_dateEditDeath_dateChanged(const QDate &date);

//    void on_checkBoxDateOfBirthUnknown_stateChanged(int state);

//    void on_checkBoxDateOfDeathUnknown_stateChanged(int state);

private:
    void save();
    void addPhoto(const QString &fileName);
//    void setTextGrayedOut(QWidget *widget, bool grayedOut);
    void viewPhoto(const QString &fileName);
    QString getPhotosFolder() const;

    Ui::DialogPersonDetails *ui;
    DiagramItem *m_item;
    DialogViewPhoto *m_viewPhotoDialog;
    QString m_xmlFile;
    QString m_lastPhotoFolder;
};

#endif // DIALOGPERSONDETAILS_H
