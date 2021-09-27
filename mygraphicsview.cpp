#include "mygraphicsview.h"

// -----------------------------------------------------------------------------
// Based on https://gist.github.com/Legor/a00760b6d7af32c01357fb7ff76ad86a
// -----------------------------------------------------------------------------

#include <QEvent>
#include <QMouseEvent>

MyGraphicsView::MyGraphicsView(QGraphicsScene *scene, QWidget *owner) :
    QGraphicsView(scene, owner)
{
    installEventFilter(this);
}

void MyGraphicsView::onMouseReleased()
{
    // This is a workaround, since the event filter does not catch
    // mouse-release events. Instead we use a signal from the
    // scene object.
    setDragMode(QGraphicsView::DragMode::NoDrag);
}

bool MyGraphicsView::eventFilter(QObject *object, QEvent *event) {

    Q_UNUSED(object);

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

        // Use the middle mouse button to scroll
        if (mouse_event->button() == Qt::MiddleButton)
        {
            // Temporarily enable dragging mode
            setDragMode(QGraphicsView::DragMode::ScrollHandDrag);

            // Emit a left mouse click (the default button for the drag mode)
            QMouseEvent* pressEvent = new QMouseEvent(QEvent::GraphicsSceneMousePress,
                                                      mouse_event->pos(), Qt::MouseButton::LeftButton,
                                                      Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier);
            mousePressEvent(pressEvent);
        }
    }

    // Let the default handler take effect.
    return false;
}
