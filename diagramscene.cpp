/*
 * This file is part of the Genealogy Maker program (https://github.com/martinvanzijl/genealogy-maker).
 * Copyright (c) 2023 Martin van Zijl.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "diagramscene.h"
#include "arrow.h"
#include "fileutils.h"
#include "marriageitem.h"
#include "undo/changebordercolorundo.h"
#include "undo/changefillcolorundo.h"
#include "undo/changelinecolorundo.h"
#include "undo/changetextcolorundo.h"
#include "undo/undoblock.h"
#include "undo/undomanager.h"

#include <QDebug>
#include <QDir>
#include <QDomDocument>
#include <QTextCursor>
#include <QTextStream>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QSettings>
#include <QTimer>

///
/// \brief parseXmlDate Parse the string from an XML file into a date.
/// \param string The attribute value.
/// \return The date object.
///
static QDate parseXmlDate(const QString &string)
{
    // Try parsing as a normal date.
    QDate date = QDate::fromString(string);

    if (!date.isValid())
    {
        // Try parsing as a GEDCOM date.
        date = QDate::fromString(string, "d MMM yyyy");
    }

//    if (!date.isValid())
//    {
//        // Try parsing as a year only.
//        date = QDate::fromString(string, "yyyy");
//    }

    if (!date.isValid())
    {
        qDebug() << "Invalid date in XML:" << string;
    }

    return date;
}

//! [0]
DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    myMode = MoveItem;
    myItemType = DiagramItem::Person;
    line = 0;
    textItem = 0;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
    m_highlightedItem = nullptr;
    m_nextId = 1;

    m_searchHighlightTimer = new QTimer(this);
    connect(m_searchHighlightTimer, SIGNAL(timeout()), this, SLOT(removeSearchHighlight()));

    m_window = nullptr;
}
//! [0]

//! [1]
void DiagramScene::setLineColor(const QColor &color)
{
    myLineColor = color;

    UndoBlock *block = new UndoBlock();
    for (auto item: selectedItems()) {

        if (item->type() == Arrow::Type) {
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            if (arrow->getColor() != myLineColor) {
                new ChangeLineColorUndo(arrow, myLineColor, block);
            }
            arrow->setColor(myLineColor);
        }
        else if (item->type() == DiagramItem::Type) {
            DiagramItem *diagramItem = dynamic_cast<DiagramItem*> (item);
            if (diagramItem->brush().color() != myLineColor) {
                new ChangeBorderColorUndo(diagramItem, myLineColor, block);
            }
            diagramItem->setBorderColor(myLineColor);
        }

    }
    UndoManager::addBlock(block);
}
//! [1]

//! [2]
void DiagramScene::setTextColor(const QColor &color)
{
    myTextColor = color;
//    if (isItemChange(DiagramTextItem::Type)) {
//        DiagramTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
//        item->setDefaultTextColor(myTextColor);
//    }
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        if (item->getTextColor() != color) {
            UndoManager::add(new ChangeTextColorUndo(item, color));
        }
        item->setTextColor(myTextColor);
    }
}
//! [2]

//! [3]
void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;

    UndoBlock *block = new UndoBlock();
    for (auto item: selectedItems()) {
        if (item->type() == DiagramItem::Type) {
            DiagramItem *diagramItem = dynamic_cast<DiagramItem*> (item);
            if (diagramItem->brush().color() != color) {
                new ChangeFillColorUndo(diagramItem, color, block);
            }
            diagramItem->setBrush(color);
        }
    }
    UndoManager::addBlock(block);
}
//! [3]

//! [4]
void DiagramScene::setFont(const QFont &font)
{
    myFont = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}

///
/// \brief DiagramScene::open Open the given file.
/// \param device The file to open.
/// \param photosFolderPath The path to photos for the project.
/// \return True if opened OK, false otherwise.
///
bool DiagramScene::open(QIODevice *device, const QString &photosFolderPath)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument domDocument;
    if (!domDocument.setContent(device, true, &errorStr, &errorLine,
                                &errorColumn)) {
        QMessageBox::information(window(), tr("Genealogy Maker"),
                                 tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
        return false;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "genealogy") {
        QMessageBox::information(window(), tr("Genealogy Maker"),
                                 tr("The file is not an genealogy file."));
        return false;
    }

    clear();
    emit cleared();

    // Load diagram size.
    int diagramWidth = root.attribute("height", "5000").toInt();
    int diagramHeight = root.attribute("width", "5000").toInt();
    setSceneRect(0, 0, diagramWidth, diagramHeight);

    // Load persons.
    QDomElement child = root.firstChildElement("item");
    while (!child.isNull()) {
        parseItemElement(child, photosFolderPath);
        child = child.nextSiblingElement("item");
    }

    // Load relationships.
    child = root.firstChildElement("relationship");
    while (!child.isNull()) {
        parseArrowElement(child);
        child = child.nextSiblingElement("relationship");
    }

    // Load marriages.
    child = root.firstChildElement("marriage");
    while (!child.isNull()) {
        parseMarriageElement(child);
        child = child.nextSiblingElement("marriage");
    }

    // Return.
    return true;
}

void DiagramScene::print()
{
    qDebug() << "Total items:" << items().count();
    for (auto item: items()) {
        qDebug() << "   " << item;
    }
}

void DiagramScene::save(QIODevice *device, const QString &photosFolderPath)
{
    const int indentSize = 4;

    QTextStream out(device);
    QDomDocument domDocument;
    QDomElement rootElement = domDocument.createElement("genealogy");
    QList<Arrow *> arrows;

    //
    // Save diagram information.
    //
    auto diagramRect = sceneRect();
    rootElement.setAttribute("width", diagramRect.width());
    rootElement.setAttribute("height", diagramRect.height());

    // Check if photos should be copied.
    bool copyPhotos = !photosFolderPath.isEmpty();

    //
    // Save persons.
    //
    for (auto item: items()) {
        DiagramItem *diagramItem = dynamic_cast<DiagramItem*> (item);

        if (diagramItem) {
            QDomElement itemElement = domDocument.createElement("item");
            itemElement.setAttribute("x", item->pos().x());
            itemElement.setAttribute("y", item->pos().y());
            itemElement.setAttribute("first_name", diagramItem->getFirstName());
            itemElement.setAttribute("last_name", diagramItem->getLastName());
            itemElement.setAttribute("name", diagramItem->name());
            itemElement.setAttribute("id", diagramItem->id().toString());
            itemElement.setAttribute("bio", diagramItem->bio());
//            if (diagramItem->isDateOfBirthKnown()) {
                itemElement.setAttribute("date_of_birth", diagramItem->getDateOfBirth().toString());
//            }
            itemElement.setAttribute("place_of_birth", diagramItem->getPlaceOfBirth());
            itemElement.setAttribute("country_of_birth", diagramItem->getCountryOfBirth());
//            if (diagramItem->isDateOfDeathKnown()) {
                itemElement.setAttribute("date_of_death", diagramItem->getDateOfDeath().toString());
//            }
            itemElement.setAttribute("place_of_death", diagramItem->getPlaceOfDeath());
            itemElement.setAttribute("fill_color", diagramItem->brush().color().name());
            itemElement.setAttribute("text_color", diagramItem->getTextColor().name());
            itemElement.setAttribute("border_color", diagramItem->getBorderColor().name());

            // Copy photos to project directory if required.
            if (copyPhotos) {
                copyPhotosForPerson(diagramItem, QDir(photosFolderPath));
            }

            // Save photo file names.
            for (QString photo: diagramItem->photos()) {
                QDomElement photoElement = domDocument.createElement("photo");

                // Hack: remove folder path to save relative file name.
                if (copyPhotos) {
                    QString prefix = photosFolderPath + "/";

                    if (photo.startsWith(prefix)) {
                        QString relativePath = photo;
                        relativePath.remove(prefix);
                        photoElement.setAttribute("project_dir_path", relativePath);
                    }
                }

                photoElement.setAttribute("path", photo);
                itemElement.appendChild(photoElement);
            }

            // Save gender.
            if (diagramItem->isGenderKnown()) {
                itemElement.setAttribute("gender", diagramItem->getGender());
            }

            rootElement.appendChild(itemElement);

            arrows << diagramItem->getArrows();
        }
    }

    //
    // Save relationships.
    //
    auto arrowSet = QSet<Arrow *>::fromList(arrows);
    for (auto arrow: arrowSet) {
        QDomElement element = domDocument.createElement("relationship");
        element.setAttribute("from", arrow->startItem()->id().toString());
        element.setAttribute("to", arrow->endItem()->id().toString());
        element.setAttribute("color", arrow->getColor().name());
        rootElement.appendChild(element);
    }

    //
    // Save marriages.
    //
    for (auto item: items()) {
        if (item->type() == MarriageItem::Type) {
            MarriageItem *marriage = dynamic_cast<MarriageItem*> (item);
            QDomElement element = domDocument.createElement("marriage");
            element.setAttribute("x", marriage->pos().x());
            element.setAttribute("y", marriage->pos().y());
            element.setAttribute("person_left", marriage->personLeft()->id().toString());
            element.setAttribute("person_right", marriage->personRight()->id().toString());
//            if (marriage->isDateKnown()) {
                element.setAttribute("date", marriage->getDate().toString());
//            }
            element.setAttribute("place", marriage->getPlace());

            rootElement.appendChild(element);
        }
    }

    domDocument.appendChild(rootElement);
    domDocument.save(out, indentSize);
}

DiagramItem *DiagramScene::itemWithId(const QUuid& id)
{
    return m_itemsDict[id];
}

bool DiagramScene::isEmpty() const
{
    return items().isEmpty();
}

QGraphicsItem *DiagramScene::firstItem() const
{
    return items().first();
}

void DiagramScene::selectAll()
{
    for (auto item: items()) {
        item->setSelected(true);
    }
}

void DiagramScene::addPersonFromUndo(DiagramItem *item)
{
    addItem(item);
    m_itemsDict[item->id()] = item;
    emit itemInserted(item, false);
}

void DiagramScene::removePersonFromUndo(DiagramItem *item)
{
    removeItem(item);
    m_itemsDict.remove(item->id());
    emit itemRemoved(item);
}

void DiagramScene::removeMarriage(DiagramItem *person1, DiagramItem *person2)
{
    Q_UNUSED(person2);

    person1->removeMarriage();
}

bool DiagramScene::isDrawingArrow() const
{
    return myMode == InsertLine;
}

void DiagramScene::loadPreferences()
{
    QSettings settings;

    int arrowLineWidth = settings.value("diagram/arrowLineWidth", 2).toInt();
    bool showThumbnails = settings.value("diagram/showThumbnails", true).toBool();

    QString fontFamily = settings.value("diagram/fontFamily", "").toString();
    qreal fontSize = settings.value("diagram/fontSize", 0).toReal();
    QFont font;
    bool fontIsValid = (!fontFamily.isEmpty() && fontSize > 0);

    if (fontIsValid) {
        font.setFamily(fontFamily);
        font.setPointSizeF(fontSize);
        DiagramTextItem::setDefaultFont(font);
    }

    for (auto item: items()) {
        if (item->type() == Arrow::Type) {
            Arrow *arrow = qgraphicsitem_cast<Arrow*> (item);
            arrow->setLineWidth(arrowLineWidth);
        }
        else if (item->type() == DiagramItem::Type) {
            DiagramItem *person= qgraphicsitem_cast<DiagramItem*> (item);
            person->setShowThumbnail(showThumbnails);

            if (fontIsValid) {
                person->textItem()->setFont(font);
                person->fitToText();
            }
        }
    }

    Arrow::setDefaultLineWidth(arrowLineWidth);
    DiagramItem::setShowThumbnailByDefault(showThumbnails);
}

int DiagramScene::autoLayoutRow(const QList<DiagramItem *> &items, int startY) {

    double startX = 128;

    double x = startX;
    double y = startY;

    double horizontalSpacing = 16;
    double verticalSpacing = 16;

    // Lay out each item in the row.
    for (auto item: items) {
        // Get the person.
        DiagramItem *person = qgraphicsitem_cast<DiagramItem*> (item);

        // Only place the *left* spouse of married couples. The right spouse will
        // move along with them.
        if (person->isMarried() && person->getSpousePosition() == DiagramItem::SpouseToLeft) {
            continue;
        }

        // Place the person.
        person->setPos(x, y);

        // Prepare the next position.
        x += person->getWidthIncludingSpouse() + horizontalSpacing;

        // Wrap to next line if required.
        if (x > sceneRect().right()) {
            y += person->boundingRect().height() + verticalSpacing;
            x = startX;
        }
    }

    return y;
}

/**
 * @brief DiagramScene::highlightForSearch Highlight the item as a search result.
 * @param item The item.
 */
void DiagramScene::highlightForSearch(DiagramItem *item)
{
    highlight(item);
    m_searchHighlightTimer->start(1000);
}

/**
 * @brief DiagramScene::marriageCount Count the marriages in the diagram.
 */
int DiagramScene::marriageCount() const
{
    int count = 0;

    for (auto item: items())
    {
        if (item->type() == MarriageItem::Type)
        {
            ++count;
        }
    }

    return count;
}

/**
 * @brief DiagramScene::personCount Count the persons in the diagram.
 */
int DiagramScene::personCount() const
{
    int count = 0;

    for (auto item: items())
    {
        if (item->type() == DiagramItem::Type)
        {
            ++count;
        }
    }

    return count;
}

/**
 * @brief DiagramScene::relationshipCount Count the parent/child relationships in the diagram.
 */
int DiagramScene::relationshipCount() const
{
    int count = 0;

    for (auto item: items())
    {
        if (item->type() == Arrow::Type)
        {
            ++count;
        }
    }

    return count;
}

// A temporary map to store the depth of each person.
static QMap<DiagramItem *, int> m_depthMap;

//
// Get the "depth" of the person, i.e. how many layers of ancestors they have.
//
//static int getAncestorDepth(DiagramItem *person) {

//    // Check if depth already calculated.
//    if (m_depthMap.contains(person)) {
//        return m_depthMap[person];
//    }

//    // Get maximum depth of parents.
//    QList<DiagramItem *> parents = person->getParents();

//    int maxParentDepth = -1;
//    for (DiagramItem *parent: parents) {
//        int parentDepth = getDepth(parent);
//        if (parentDepth > maxParentDepth) {
//            maxParentDepth = parentDepth;
//        }
//    }

//    // Add one.
//    int personDepth = maxParentDepth + 1;
//    m_depthMap[person] = personDepth;

//    // Return.
//    return personDepth;
//}

//
// Get the "depth" of the person, i.e. how many layers of descendants they have.
//
static int getDepth(DiagramItem *person) {

    // Check if depth already calculated.
    if (m_depthMap.contains(person)) {
        return m_depthMap[person];
    }

    // Get maximum depth of parents.
    QList<DiagramItem *> children = person->getChildren();

    int maxChildDepth = -1;
    for (DiagramItem *child: children) {
        int childDepth = getDepth(child);
        if (childDepth > maxChildDepth) {
            maxChildDepth = childDepth;
        }
    }

    // Add one.
    int personDepth = maxChildDepth + 1;

    // Check position of spouse if required.
    if (person->isMarried() && person->getSpousePosition() == DiagramItem::SpouseToRight) {
        int spouseDepth = getDepth(person->getSpouse());
        personDepth = qMax(personDepth, spouseDepth);
    }

    // Add to map.
    m_depthMap[person] = personDepth;

    // Return.
    return personDepth;
}

void DiagramScene::autoLayout()
{
    // Clear depth map.
    m_depthMap.clear();

    // Set up map of depth to persons.
    QMultiMap<int, DiagramItem* > depthMultiMap;

    // Find the right depth for each person.
    for (auto item: items()) {
        if (item->type() == DiagramItem::Type) {
            DiagramItem *person = qgraphicsitem_cast<DiagramItem*> (item);

            int depth = getDepth(person);
            depthMultiMap.insert(depth, person);
        }
    }

    // Get maximum depth.
    auto maxDepth = depthMultiMap.lastKey();

    // Place the persons.
    double y = 64;

    for (int depth = maxDepth; depth >= 0; --depth) {
        y = autoLayoutRow(depthMultiMap.values(depth), y);
        y += 256;
    }
}

//! [4]

void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}

void DiagramScene::setItemType(DiagramItem::DiagramType type)
{
    myItemType = type;
}

//! [5]
void DiagramScene::editorLostFocus(DiagramTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }
}

//void DiagramScene::onSelectionChanged()
//{
//    // Highlight selected items.
//    for (auto item: selectedItems()) {
//        if (item->type() == DiagramItem::Type) {
//            auto diagramItem = qgraphicsitem_cast<DiagramItem *> (item);
//            diagramItem->setHighlighted(true);
//        }
//    }
//}
//! [5]

//! [6]
DiagramItem *DiagramScene::createPerson(const QPointF &pos)
{
    // Create the item.
    DiagramItem *item = new DiagramItem(myItemType, myItemMenu);
//        item->setBrush(myItemColor);
    item->setBrush(Qt::white);
    addItem(item);
    item->setPos(pos);

    // Assign ID.
    auto id = QUuid::createUuid();
    item->setId(id);
    m_itemsDict[id] = item;
    emit itemInserted(item, false);

    // Return the item.
    return item;
}

void DiagramScene::copyPhotosForPerson(DiagramItem *diagramItem, const QDir &photosDir)
{
    // Create photos folder for person.
    QString personPhotosDirName = diagramItem->id().toString();
    QDir personPhotosDir(photosDir.filePath(personPhotosDirName));
    QString personPhotosFolderPath = personPhotosDir.absolutePath();

    // Get list of photos.
    auto photos = diagramItem->photos();

    // Ensure photos with the same name do not overwrite each other.
    QStringList destFileNames;
    for (QString path: photos) {

        // Get the file name.
        QFileInfo info(path);
        QString destFileName = info.fileName();

        // Choose a different name if it exists.
        if (destFileNames.contains(destFileName)) {
            QString baseName = info.baseName();
            QString ext = info.suffix();

            for (int suffix = 2; suffix < 10; ++suffix)
            {
                destFileName = baseName + "_" + QString::number(suffix) + "." + ext;

                if (!destFileNames.contains(destFileName)) {
                    break;
                }
            }
        }

        // Add to list.
        destFileNames << destFileName;
    }

    Q_ASSERT(destFileNames.size() == photos.size());

    // Save the photo details.
    QStringList photosUpdated;

    for (int i = 0; i < photos.size(); ++i) {

        // Get photo.
        QString photo = photos[i];

        // Ensure photo is saved in project directory.
        if (!photo.startsWith(personPhotosFolderPath))
        {
            // Create photos folder for person.
            if (!personPhotosDir.exists())
            {
                personPhotosDir.mkpath(".");
            }

            // Copy photo to folder.
            QString destFileName = destFileNames[i];
            QString destFilePath = personPhotosDir.absoluteFilePath(destFileName);
            bool copiedOK = FileUtils::copyAndReplace(photo, destFilePath);

            // Check for existing files.
            if (copiedOK) {
                photo = destFilePath;
            }
            else {
                qDebug() << "Could not copy photo" << photo << "to" << destFilePath;
            }
        }

        // Add to list.
        photosUpdated << photo;
    }

    // Update the list.
    diagramItem->setPhotos(photosUpdated);
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    // Unhighlight search item.
    removeSearchHighlight();

    DiagramItem *item = nullptr;
    switch (myMode) {
    case InsertItem:
    {
        // Create person.
        item = createPerson(mouseEvent->scenePos());

        break;
    }

        case InsertLine:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                        mouseEvent->scenePos()));
            line->setPen(QPen(myLineColor, 2));
            addItem(line);
            break;

//        case InsertText:
//            textItem = new DiagramTextItem();
//            textItem->setFont(myFont);
//            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
//            textItem->setZValue(1000.0);
//            connect(textItem, SIGNAL(lostFocus(DiagramTextItem*)),
//                    this, SLOT(editorLostFocus(DiagramTextItem*)));
//            connect(textItem, SIGNAL(selectedChange(QGraphicsItem*)),
//                    this, SIGNAL(itemSelected(QGraphicsItem*)));
//            addItem(textItem);
//            textItem->setDefaultTextColor(myTextColor);
//            textItem->setPos(mouseEvent->scenePos());
//            emit textInserted(textItem);

//    case MoveItem:
//    {
//        auto draggedItem = mouseGrabberItem();
//        if (draggedItem)
//        {
//            emit itemsAboutToMove();
//        }
//        break;
//    }

    default:
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);

    // If we added an item, then immediately edit the name.
    if (item)
    {
        item->editName();
    }
}
//! [9]

//! [10]
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == InsertLine && line != 0) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {

        // Handle undo.
        auto draggedItem = mouseGrabberItem();
        if (draggedItem && !m_busyMoving) {
            m_busyMoving = true;
            emit itemsAboutToMove();
        }

        // Normal event.
        QGraphicsScene::mouseMoveEvent(mouseEvent);

        // Check for moving text: move rectangle also.
        if (draggedItem && draggedItem->type() == DiagramTextItem::Type) {
            draggedItem->parentItem()->setSelected(true);
        }

        // Check for marriage.
        if (draggedItem && draggedItem->type() == DiagramItem::Type)
        {
            auto draggedDiagramItem = qgraphicsitem_cast<DiagramItem *>(draggedItem);

            if (!draggedDiagramItem->isMarried())
            {
                QList<QGraphicsItem *> list = draggedItem->collidingItems();

                bool found = false;
                for (auto item: list)
                {
                    if (item != draggedItem && item->type() == DiagramItem::Type)
                    {
                        auto diagramItem = qgraphicsitem_cast<DiagramItem *>(item);
                        if (draggedDiagramItem->canMarry(diagramItem))
                        {
                            found = true;
                            highlight(diagramItem);
                            break;
                        }
                    }
                }

                // Nothing found, so unlighlight all.
                if (!found) {
                    unHighlightAll();
                }
            }
        }
    }
}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (line != 0 && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        removeItem(line);
        delete line;

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first() != endItems.first()) {

            // If we release on a text item, use the parent rectangle item instead.
            auto firstItemToCast = startItems.first();
            if (firstItemToCast->type() == DiagramTextItem::Type) {
                firstItemToCast = firstItemToCast->parentItem();
            }
            auto endItemToCast = endItems.first();
            if (endItemToCast->type() == DiagramTextItem::Type) {
                endItemToCast = endItemToCast->parentItem();
            }

            // Now create the arrow.
            if (firstItemToCast->type() == DiagramItem::Type &&
                endItemToCast->type() == DiagramItem::Type)
            {
                DiagramItem *startItem = qgraphicsitem_cast<DiagramItem *>(firstItemToCast);
                DiagramItem *endItem = qgraphicsitem_cast<DiagramItem *>(endItemToCast);
                Arrow *arrow = new Arrow(startItem, endItem);
                arrow->setColor(myLineColor);
                startItem->addArrow(arrow);
                endItem->addArrow(arrow);
                arrow->setZValue(-1000.0);
                addItem(arrow);
                arrow->updatePosition();

                emit arrowAdded(arrow);
            }
        }
    }
    else if (myMode == MoveItem) {
        if (m_busyMoving) {
            m_busyMoving = false;
            emit itemsFinishedMoving();
        }

        if (m_highlightedItem) {
            auto draggedItem = mouseGrabberItem();
            if (draggedItem && draggedItem->type() == DiagramItem::Type) {
                auto item = qgraphicsitem_cast<DiagramItem *>(draggedItem);
                auto name1 = item->name();
                auto name2 = m_highlightedItem->name();

                QMessageBox msgBox;
                msgBox.setWindowTitle("Confirm");
                msgBox.setText(QString("Marry %1 and %2?").arg(name1, name2));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::Yes);
                int ret = msgBox.exec();

                if (ret == QMessageBox::Yes) {
                    marry(item, m_highlightedItem);
                }
            }
        }

        unHighlightAll();
    }

    line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);

    emit mouseReleased();
}

void DiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // Reset double-clicked item.
    DiagramItem::resetDoubleClickedItem();

    // Call parent method.
    QGraphicsScene::mouseDoubleClickEvent(event);

    // Check if item was double-clicked.
    auto item = DiagramItem::getDoubleClickedItem();
    if (item) {
        emit personDoubleClicked(item);
    }
}

void DiagramScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void DiagramScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void DiagramScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        QString action;
        dataStream >> action;

        event->acceptProposedAction();

        DiagramItem *item = createPerson(event->scenePos());
        item->setSelected(true);
//        item->editName(); // This does not work yet.

        // TODO: Simulate mouse click event on the text.
        // I hope this lets the user start editing the text.
    } else {
        event->ignore();
    }
}

bool DiagramScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}

void DiagramScene::parseItemElement(const QDomElement &element, const QString &photosFolderPath)
{
    auto item = new DiagramItem(DiagramItem::Person, myItemMenu);
    item->setBrush(Qt::white);
    addItem(item);
    auto x = element.attribute("x").toDouble();
    auto y = element.attribute("y").toDouble();
    auto firstName = element.attribute("first_name");
    auto lastName = element.attribute("last_name");
    auto name = element.attribute("name");

    QUuid id;
    if (element.hasAttribute("id")) {
        id = QUuid(element.attribute("id"));
    }
    else {
        id = QUuid::createUuid();
    }

    auto bio = element.attribute("bio");
    auto placeOfBirth = element.attribute("place_of_birth");
    auto countryOfBirth = element.attribute("country_of_birth");
    auto placeOfDeath = element.attribute("place_of_death");
    auto gender = element.attribute("gender");
    item->setPos(x, y);
    item->setFirstName(firstName);
    item->setLastName(lastName);
    item->setName(name);
    item->setId(id);
    item->setBio(bio);
    item->setPlaceOfBirth(placeOfBirth);
    item->setCountryOfBirth(countryOfBirth);
    item->setPlaceOfDeath(placeOfDeath);
    item->setGender(gender);

    if (element.hasAttribute("date_of_birth")) {
//        item->setDateOfBirth(QDate::fromString(element.attribute("date_of_birth")));
        item->setDateOfBirth(parseXmlDate(element.attribute("date_of_birth")));
    }
    if (element.hasAttribute("date_of_death")) {
        item->setDateOfDeath(parseXmlDate(element.attribute("date_of_death")));
    }

    if (element.hasAttribute("fill_color")) {
        QColor color;
        color.setNamedColor(element.attribute("fill_color"));
        item->setBrush(color);
    }

    if (element.hasAttribute("text_color")) {
        QColor color;
        color.setNamedColor(element.attribute("text_color"));
        item->setTextColor(color);
    }

    if (element.hasAttribute("border_color")) {
        QColor color;
        color.setNamedColor(element.attribute("border_color"));
        item->setBorderColor(color);
    }

    QStringList photos;
    QDomElement photoElement = element.firstChildElement("photo");
    while (!photoElement.isNull()) {

        // Get the path.
        QString path = photoElement.attribute("path");

        // Check for relative (project dir) path.
        if (photoElement.hasAttribute("project_dir_path")) {

            // Use the relative path.
            path = photoElement.attribute("project_dir_path");

            // Hack: Prepend photos folder.
            path.prepend(photosFolderPath + "/");
        }

        // Add to the person.
        photos << path;
        photoElement = photoElement.nextSiblingElement("photo");
    }
    item->setPhotos(photos);

    emit itemInserted(item, true);
    m_itemsDict[id] = item;

    // Check for GEDCOM pointer.
    if (element.hasAttribute("pointer")) {
        QString pointer = element.attribute("pointer");
        m_pointerDict[pointer] = item;
    }
}

void DiagramScene::parseArrowElement(const QDomElement &element)
{
    DiagramItem *startItem = nullptr;
    DiagramItem *endItem = nullptr;

    if (element.hasAttribute("from_pointer") && element.hasAttribute("to_pointer")) {
        // GEDCOM import.
        auto fromId = element.attribute("from_pointer");
        auto toId = element.attribute("to_pointer");
        startItem = m_pointerDict[fromId];
        endItem = m_pointerDict[toId];
    }
    else {
        // Normal diagram.
        auto fromId = element.attribute("from");
        auto toId = element.attribute("to");
        startItem = m_itemsDict[fromId];
        endItem = m_itemsDict[toId];
    }

    if (startItem && endItem) {
        Arrow *arrow = new Arrow(startItem, endItem);

        if (element.hasAttribute("color")) {
            QColor color;
            color.setNamedColor(element.attribute("color"));
            arrow->setColor(color);
        }
        else {
            arrow->setColor(myLineColor);
        }

        startItem->addArrow(arrow);
        endItem->addArrow(arrow);
        arrow->setZValue(-1000.0);
        addItem(arrow);
        arrow->updatePosition();
    }
}

void DiagramScene::parseMarriageElement(const QDomElement &element)
{
    DiagramItem *personLeft = nullptr;
    DiagramItem *personRight = nullptr;

    if (element.hasAttribute("left_pointer") && element.hasAttribute("right_pointer")) {
        // GEDCOM import.
        auto leftId = element.attribute("left_pointer");
        auto rightId = element.attribute("right_pointer");
        personLeft = m_pointerDict[leftId];
        personRight = m_pointerDict[rightId];
    }
    else {
        // Normal diagram.
        auto leftId = element.attribute("person_left");
        auto rightId = element.attribute("person_right");
        personLeft = m_itemsDict[leftId];
        personRight = m_itemsDict[rightId];
    }

    if (personLeft && personRight) {
        personLeft->marryTo(personRight);
        MarriageItem *marriage = personLeft->getMarriageItem();

        if (element.hasAttribute("date")) {
            marriage->setDate(QDate::fromString(element.attribute("date")));
        }
        marriage->setPlace(element.attribute("place"));
    }
}

void DiagramScene::highlight(DiagramItem *item)
{
    unHighlightAll();

    m_highlightedItem = item;
    if (m_highlightedItem) {
        m_highlightedItem->setHighlighted(true);
    }
}

void DiagramScene::unHighlightAll()
{
    if (m_highlightedItem) {
        m_highlightedItem->setHighlighted(false);
    }

    m_highlightedItem = nullptr;
}

void DiagramScene::removeSearchHighlight()
{
    // Unhighlight item.
    unHighlightAll();

    // Ensure timer is stopped.
    m_searchHighlightTimer->stop();
}

QWidget *DiagramScene::window() const
{
    return m_window;
}

void DiagramScene::setWindow(QWidget *window)
{
    m_window = window;
}

void DiagramScene::marry(DiagramItem *item1, DiagramItem *item2, bool fromUndo)
{
//    item1->marryTo(item2);

    if (!fromUndo) {
        // Make sure to use "left->marryTo(right)".
        if (item2->x() < item1->x()) {
            item2->marryTo(item1);
            item1->moveBy(-1, 0); // Hack to avoid positioning bug.
            emit peopleMarried(item2, item1);
        }
        else {
            item1->marryTo(item2);
            emit peopleMarried(item1, item2);
        }
    }
    else {
        item1->marryTo(item2);
    }
}
