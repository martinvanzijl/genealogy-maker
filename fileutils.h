#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>

class FileUtils
{
public:
    FileUtils();

    /**
     * @brief copyAndReplace Copy a file, replacing the existing desintation if required.
     * @param source The source file path.
     * @param dest The destination file path.
     * @return True if successful.
     */
    static bool copyAndReplace(const QString &source, const QString &dest);

    /**
     * @brief getPhotosFolderFor Get the project photos directory.
     * @param fileName The project (XML) file name.
     * @return The directory where photos should be stored.
     */
    static QString getPhotosFolderFor(const QString &fileName);

    /**
     * @brief moveFolder Move a folder, backing up the old folder if it exists.
     * @param source The source path.
     * @param dest The destination path.
     * @return True if successful.
     */
    static bool moveFolder(const QString &source, const QString &dest);
};

#endif // FILEUTILS_H
