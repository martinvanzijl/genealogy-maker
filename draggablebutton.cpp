#include "draggablebutton.h"

#include <QByteArray>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QTimer>

#include <QDebug>

DraggableButton::DraggableButton(QWidget *parent) :
    QToolButton(parent)
{
    m_dragStartTimer = new QTimer(this);
    m_dragStartTimer->setSingleShot(true);
    m_dragStartTimer->setInterval(100);
    connect(m_dragStartTimer, SIGNAL(timeout()), this, SLOT(onDragStartTimerTimeout()));

    m_latestMousePressEvent = nullptr;
}

DraggableButton::~DraggableButton()
{
    // Avoid compilation bug.
}

void DraggableButton::mousePressEvent(QMouseEvent *event)
{
    QToolButton::mousePressEvent(event);

    m_dragStartTimer->start();

    delete m_latestMousePressEvent;
    m_latestMousePressEvent = new QMouseEvent(event->type(), event->localPos(), event->button(), event->buttons(), event->modifiers());
}

void DraggableButton::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragStartTimer->stop();

    QToolButton::mouseReleaseEvent(event);
}

void DraggableButton::onDragStartTimerTimeout()
{
    // Start the dragging operation.
    QPixmap pixmap = icon().pixmap(32, 32);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << QString("Add Person");

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);

    drag->exec();

    // Have to simulate release event.
    QMouseEvent *fakeMouseReleaseEvent = new QMouseEvent(QMouseEvent::MouseButtonRelease,
                                                         m_latestMousePressEvent->localPos(),
                                                         m_latestMousePressEvent->button(),
                                                         m_latestMousePressEvent->buttons(),
                                                         m_latestMousePressEvent->modifiers());
    mouseReleaseEvent(fakeMouseReleaseEvent);

    // Alert that drop has finished.
    emit dragDropFinished();
}
