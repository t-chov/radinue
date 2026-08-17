#pragma once

#include <QString>
#include <QStringList>

namespace radinue {

class DirectoryPlaylist final {
  public:
    enum class SortDirection {
        Ascending,
        Descending,
    };

    [[nodiscard]] static const QStringList &supportedExtensions();
    static void sortFileNames(QStringList &fileNames, SortDirection sortDirection);

    bool openDirectory(const QString &directoryPath,
                       SortDirection sortDirection = SortDirection::Ascending);

    [[nodiscard]] const QString &directoryPath() const noexcept;
    [[nodiscard]] const QStringList &fileNames() const noexcept;
    [[nodiscard]] const QString &errorString() const noexcept;
    [[nodiscard]] qsizetype currentIndex() const noexcept;
    [[nodiscard]] QString currentFileName() const;
    [[nodiscard]] QString currentFilePath() const;
    [[nodiscard]] bool hasPrevious() const noexcept;
    [[nodiscard]] bool hasNext() const noexcept;

    bool setCurrentIndex(qsizetype index);
    bool setCurrentFileName(const QString &fileName);
    bool movePrevious();
    bool moveNext();

  private:
    QString m_directoryPath;
    QStringList m_fileNames;
    QString m_errorString;
    qsizetype m_currentIndex = -1;
};

} // namespace radinue
