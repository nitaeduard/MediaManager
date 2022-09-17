#ifndef HOTFOLDER_H
#define HOTFOLDER_H

#include "QFileSystemWatcher"

class hotfolder: QObject
{
    Q_OBJECT

    QFileSystemWatcher watcher;

    QString inputFolder;
    QString outputFolder;
public:
    hotfolder();

    void start();
    void stop();

public slots:
    void handleFileChanged(const QString&);
    void handleFolderChanged(const QString&);
};

#endif // HOTFOLDER_H
