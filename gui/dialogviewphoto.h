#ifndef DIALOGVIEWPHOTO_H
#define DIALOGVIEWPHOTO_H

#include <QDialog>

namespace Ui {
class DialogViewPhoto;
}

class DialogViewPhoto : public QDialog
{
    Q_OBJECT

public:
    explicit DialogViewPhoto(QWidget *parent = 0);
    ~DialogViewPhoto();

    bool photoLoadedOK() const;
    void setPhoto(const QString &fileName);
    void setPhotoIndex(int index);
    void setPhotoList(const QStringList &list);

private slots:
    void on_pushButtonPrevious_clicked();

    void on_pushButtonNext_clicked();

private:
    Ui::DialogViewPhoto *ui;

    void goToNext();
    void goToPrevious();

    bool m_photoLoadedOK;
    QStringList m_photoList;
    int m_currentPhotoIndex;
};

#endif // DIALOGVIEWPHOTO_H
