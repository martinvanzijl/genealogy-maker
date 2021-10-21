#ifndef DRAGGABLEBUTTON_H
#define DRAGGABLEBUTTON_H

#include <QToolButton>

class QMouseEvent;
class QTimer;

class DraggableButton : public QToolButton
{
    Q_OBJECT

public:
    DraggableButton(QWidget *parent = nullptr);
    virtual ~DraggableButton();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void dragDropFinished();

private slots:
    void onDragStartTimerTimeout();

private:
    QTimer *m_dragStartTimer;
    QMouseEvent *m_latestMousePressEvent;
};

#endif // DRAGGABLEBUTTON_H
