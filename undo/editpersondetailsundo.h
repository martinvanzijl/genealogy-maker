#ifndef EDITPERSONDETAILSUNDO_H
#define EDITPERSONDETAILSUNDO_H

#include <QDate>
#include <QUndoCommand>

class DiagramItem;

class EditPersonDetailsUndo : public QUndoCommand
{
public:
    EditPersonDetailsUndo(DiagramItem *item, QUndoCommand *parent = nullptr);

    void setAfterState(DiagramItem *item);

    void undo() override;
    void redo() override;

private:
    DiagramItem *m_item;

    QString m_originalName;
    QString m_newName;

    QString m_originalFirstName;
    QString m_newFirstName;

    QString m_originalLastName;
    QString m_newLastName;

    QDate m_originalDateOfBirth;
    QDate m_newDateOfBirth;

    QString m_originalPlaceOfBirth;
    QString m_newPlaceOfBirth;

    QString m_originalCountryOfBirth;
    QString m_newCountryOfBirth;

    QDate m_originalDateOfDeath;
    QDate m_newDateOfDeath;

    QString m_originalPlaceOfDeath;
    QString m_newPlaceOfDeath;

    QString m_originalGender;
    QString m_newGender;

    QString m_originalBio;
    QString m_newBio;

    QStringList m_originalPhotos;
    QStringList m_newPhotos;

    bool m_undone;
};

#endif // EDITPERSONDETAILSUNDO_H
