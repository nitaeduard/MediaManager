#include "mmfilemodel.h"
#include <QApplication>
#include <QStyle>
#include <QDir>
#include <QStorageInfo>

MMFileModel::MMFileModel(QObject *parent)
    :QStandardItemModel(parent)
{
    rootItem = this->invisibleRootItem();
    dirIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);           //icon for directories
    fileIcon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);         //icon for files
    driveIcon = QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon);     //icon for drives

    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady()) {
            QString driveName = storage.displayName() + " (" + storage.rootPath() + ")";
            QStandardItem *item = new QStandardItem(driveIcon, driveName);
            item->setData(storage.rootPath());
            item->setEditable(false);
            QStandardItem* dummyItem = new QStandardItem("Loading...");
            dummyItem->setSelectable(false);
            item->appendRow(dummyItem);
            rootItem->appendRow(item);
        }
    }
}

MMFileModel::~MMFileModel()
{
}

QString MMFileModel::filePath(QModelIndex index)
{
    QStandardItem* item = itemFromIndex(index);
    if (item == 0) {
        return "";
    }
    return item->data().toString();
}

void MMFileModel::handleFolderExpand(const QModelIndex &index) {
    QStandardItem* item = itemFromIndex(index);
    if (item == 0) {
        return;
    }
    QStandardItem* loadingItem = item->child(0);
    if (loadingItem == 0 || loadingItem->text() != "Loading...") {
        return;
    }
    QString path = item->data().toString();
    QDir dir(path);
    QFileInfoList subFolders;
    QFileInfo folderName;
    QStandardItem* child;
    subFolders = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);    //get all the sub folders

    foreach (folderName, subFolders) {
        if(folderName.isFile()) {
            child = new QStandardItem(fileIcon, folderName.fileName());                 //Append a file
            child->setAccessibleDescription(folderName.filePath());                     //set actual path to item
            child->setData(folderName.filePath());
            child->setEditable(false);
        }
        else {
            child = new QStandardItem(dirIcon, folderName.fileName());                  //Append a folder
            child->setAccessibleDescription(folderName.filePath());                     //set actual path to item
            child->setData(folderName.filePath());
            QStandardItem* dummyItem = new QStandardItem("Loading...");
            dummyItem->setSelectable(false);
            child->setEditable(false);
            child->appendRow(dummyItem);
        }
        item->appendRow(child);
    }
    // remove "Loading..."
    item->removeRow(0);
}


