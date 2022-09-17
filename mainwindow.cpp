#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStringListModel>
#include <QSettings>
#include <QFileSystemModel>
#include <QImageReader>

#include "exif.h"
#include "heifreader.h"

easyexif::EXIFInfo getExif(QString fileName) {
    QFile file(fileName);
    easyexif::EXIFInfo info;
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        if (int code = info.parseFrom((unsigned char *)data.data(), data.size())){
            qDebug() << "Error parsing EXIF: code " << code;
        }
    }
    return info;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fileModel = new MMFileModel();
    ui->treeView->setModel(fileModel);
    ui->treeView->setHeaderHidden(true);
    connect(ui->imageWidget, &EImageViewer::imageDidFinishZoom, this, &MainWindow::imageWasZoomed);
    connect(ui->treeView, &QTreeView::expanded, fileModel, &MMFileModel::handleFolderExpand);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectedFileChanged(QModelIndex newIndex)
{
    QString selectedPath = fileModel->filePath(newIndex);
    setWindowTitle(selectedPath);
    QImageReader reader = QImageReader(selectedPath);
    QImage image;
    if (reader.canRead()) {
        image = reader.read();

        QString text = QString();
        text += QString("%1 x %2\n").arg(QString::number(image.size().width ()), image.size().height());
        easyexif::EXIFInfo info = getExif(selectedPath);
        text += "camera: " + QString(info.Model.c_str());
        ui->plainTextEdit->setPlainText(text);
    }
    else {
        HEIFReader reader = HEIFReader(selectedPath);
        if (!reader.read(&image)) {
            return;
        }
        QString text = QString();
        text += QString("%1 x %2\n").arg(QString::number(reader.info.ImageWidth), QString::number(reader.info.ImageHeight));
        text += QString("Model: %1").arg(QString(reader.info.Model.c_str()));
        ui->plainTextEdit->setPlainText(text);
    }

    ui->imageWidget->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::on_restoreButton_triggered()
{
    if (isMaximized()) {
        showNormal();
    }
    else {
        showMaximized();
    }
}

void MainWindow::imageWasZoomed() {
    QString text = QString();
    qreal scale = ui->imageWidget->mScale;
    text.setNum(scale);
    ui->zoomBar->setValue(scale * 10);
    ui->zoomText->setText(text);
}
