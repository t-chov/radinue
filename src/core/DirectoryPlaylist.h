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

  private:
    QString m_directoryPath;
    QStringList m_fileNames;
    QString m_errorString;
};

} // namespace radinue
