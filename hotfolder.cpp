#include "hotfolder.h"

#include "QDebug"
#include <QFile>
#include <QDir>

hotfolder::hotfolder()
    : watcher(),
      inputFolder("e:\\Hotfolder"),
      outputFolder("e:\\Hotfolder")

{
}

void hotfolder::start() {
    watcher.addPath(inputFolder);

    QObject::connect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(handleFileChanged(QString)));
    QObject::connect(&watcher, SIGNAL(directoryChanged(QString)), this, SLOT(handleFolderChanged(QString)));
}

void hotfolder::stop() {
    watcher.removePath(inputFolder);
}

void hotfolder::handleFileChanged(const QString& fileName) {
    qDebug() << "file changed: " << QString(fileName);
}

void hotfolder::handleFolderChanged(const QString& path) {
    qDebug() << "file in folder changed: " << path;
    QDir inputDir = QDir(path);
    //foreach(inputDir.Files) {

    //}
    qDebug() << QDir(path).count();
}
