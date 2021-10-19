#include "fileutils.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

FileUtils::FileUtils()
{

}

/**
 * @brief backUpFolder Create a backup copy of a folder.
 * @param dir The folder to back up.
 */
static void backUpFolder(const QDir &dir)
{
    // Get parent directory.
    QDir parentDir = dir;
    bool parentDirOK = parentDir.cdUp();

    // Back up the existing folder.
    if (parentDirOK) {

        // Get timestamp.
        QDateTime date = QDateTime::currentDateTime();
        QString timeStamp = date.toString("yyyy-MM-dd-HH-mm-ss");

        // Get backup folder.
        QString dirName = dir.dirName();
        QString backupName = dirName + ".backup-" + timeStamp;

        QDir backupDir = parentDir.filePath(backupName);

        if (!backupDir.exists()) {
            // Rename if it does not exist.
            bool renamedOK = parentDir.rename(dirName, backupName);

            // Warn if there was a problem.
            if (!renamedOK) {
                qDebug() << "Could not back up photos folder" << dirName << "to" << backupName;
            }
        }
        else {
            // TODO: Copy each file individually if it does exist.
        }
    }
    else {
        qDebug() << "Could not go to parent directory to clear existing folder.";
    }
}

//static bool copyAndRename(const QString &source, const QString &dest)
//{
//    if (source == dest)
//    {
//        qDebug() << "Source and destination are the same. Skipping.";
//        return false;
//    }

//    QString finalDest = dest;
//    if (QFile::exists(finalDest))
//    {
//        QFileInfo info(finalDest);
//        QString baseName = info.baseName();
//        QString path = info.absolutePath();
//        QString ext = info.suffix();
//        QDir dir(path);

//        for (int suffix = 2; suffix < 10; ++suffix)
//        {
//            QString newFileName = baseName + "_" + QString::number(suffix) + "." + ext;
//            finalDest = dir.absoluteFilePath(newFileName);
//            if (!QFile::exists(finalDest))
//            {
//                qDebug() << "Renaming to" << finalDest;
//                break;
//            }
//        }
//    }

//    return QFile::copy(source, finalDest);
//}

bool FileUtils::copyAndReplace(const QString &source, const QString &dest)
{
    if (source == dest)
    {
        qDebug() << "Source and destination are the same. Skipping.";
        return false;
    }

    if (QFile::exists(dest))
    {
//        qDebug() << "Replacing existing file:" << source;
        QFile::remove(dest);
    }

    return QFile::copy(source, dest);
}

QString FileUtils::getPhotosFolderFor(const QString &fileName)
{
    if (fileName.isEmpty()) {
        // Diagram has not been saved yet.
        return "save-files/new-diagram-photos";
    }

    // Get file info.
    QFileInfo info(fileName);

    // Create dir name.
    QDir dir = info.dir();
    QString folderName = info.baseName() + "-photos";

    // Return.
    return dir.absoluteFilePath(folderName);
}

bool FileUtils::moveFolder(const QString &source, const QString &dest)
{
    // Back up.
    QDir destDir(dest);

    if (destDir.exists()) {
        backUpFolder(destDir);
    }

    // Move.
    bool renameOK = QDir().rename(source, dest);

    // Show warning.
    if (!renameOK) {
        qDebug() << "Could not move" << source << "to" << dest;
    }

    // Return.
    return renameOK;
}
