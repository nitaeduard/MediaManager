#ifndef MMFILEMODEL_H
#define MMFILEMODEL_H

#include <QStandardItemModel>
#include <QStandardItem>

class MMFileModel : public QStandardItemModel
{
    Q_OBJECT
public:
    MMFileModel(QObject* parent = 0);
    ~MMFileModel();

    QString filePath(QModelIndex index);

public slots:
    void handleFolderExpand(const QModelIndex &index);

private:
    QStandardItem *rootItem;
    QIcon dirIcon;
    QIcon fileIcon;
    QIcon driveIcon;
};

#endif // MMFILEMODEL_H
