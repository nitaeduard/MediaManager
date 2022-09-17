#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QMouseEvent>

#include "mmfilemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    MMFileModel *fileModel;

public slots:
    void selectedFileChanged(QModelIndex newIndex);
    void on_restoreButton_triggered();

private slots:
    void imageWasZoomed();
};
#endif // MAINWINDOW_H
