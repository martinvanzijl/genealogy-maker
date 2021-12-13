#include "editpersondetailsundo.h"

#include "diagramitem.h"
#include "diagramscene.h"

EditPersonDetailsUndo::EditPersonDetailsUndo(DiagramItem *item,
                                             QUndoCommand *parent) :
    QUndoCommand("edit person details", parent),
    m_item(item),
    m_undone(false)
{
    m_originalName = item->name();
    m_originalFirstName = item->getFirstName();
    m_originalLastName = item->getLastName();
    m_originalDateOfBirth = item->getDateOfBirth();
    m_originalPlaceOfBirth = item->getPlaceOfBirth();
    m_originalCountryOfBirth = item->getCountryOfBirth();
    m_originalDateOfDeath = item->getDateOfDeath();
    m_originalPlaceOfDeath = item->getPlaceOfDeath();
    m_originalGender = item->getGender();
    m_originalBio = item->bio();
    m_originalPhotos = item->photos();
}

void EditPersonDetailsUndo::setAfterState(DiagramItem *item)
{
    m_newName = item->name();
    m_newFirstName = item->getFirstName();
    m_newLastName = item->getLastName();
    m_newDateOfBirth = item->getDateOfBirth();
    m_newPlaceOfBirth = item->getPlaceOfBirth();
    m_newCountryOfBirth = item->getCountryOfBirth();
    m_newDateOfDeath = item->getDateOfDeath();
    m_newPlaceOfDeath = item->getPlaceOfDeath();
    m_newGender = item->getGender();
    m_newBio = item->bio();
    m_newPhotos = item->photos();
}

void EditPersonDetailsUndo::undo()
{
    if (!m_undone)
    {
        m_item->setName(m_originalName);
        m_item->setFirstName(m_originalFirstName);
        m_item->setLastName(m_originalLastName);
        m_item->setDateOfBirth(m_originalDateOfBirth);
        m_item->setPlaceOfBirth(m_originalPlaceOfBirth);
        m_item->setCountryOfBirth(m_originalCountryOfBirth);
        m_item->setDateOfDeath(m_originalDateOfDeath);
        m_item->setPlaceOfDeath(m_originalPlaceOfDeath);
        m_item->setGender(m_originalGender);
        m_item->setBio(m_originalBio);
        m_item->setPhotos(m_originalPhotos);

        m_undone = true;
    }
}

void EditPersonDetailsUndo::redo()
{
    if (m_undone)
    {
        m_item->setName(m_newName);
        m_item->setFirstName(m_newFirstName);
        m_item->setLastName(m_newLastName);
        m_item->setDateOfBirth(m_newDateOfBirth);
        m_item->setPlaceOfBirth(m_newPlaceOfBirth);
        m_item->setCountryOfBirth(m_newCountryOfBirth);
        m_item->setDateOfDeath(m_newDateOfDeath);
        m_item->setPlaceOfDeath(m_newPlaceOfDeath);
        m_item->setGender(m_newGender);
        m_item->setBio(m_newBio);
        m_item->setPhotos(m_newPhotos);

        m_undone = false;
    }
}
