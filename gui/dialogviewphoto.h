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

//    bool photoLoadedOK() const;
    void setPhoto(const QString &fileName);

private:
    Ui::DialogViewPhoto *ui;

//    bool m_photoLoadedOK;
};

#endif // DIALOGVIEWPHOTO_H
