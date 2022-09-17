#include "eimageviewer.h"
#include <QMainWindow>
#include <QFileSystemModel>
#include <QMouseEvent>
#include <QPainter>
#include <QApplication>

EImageViewer::EImageViewer(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("background-color: #202020");
}

void EImageViewer::paintEvent(QPaintEvent *)  {
    QPainter p{this};
    p.translate(mDelta);
    p.setPen(QColor(50,50,50));
    p.setBrush(QColor(32, 32, 32));

    p.drawRect(mImageRect.adjusted(-1, -1, 1, 1));
    p.drawPixmap(mImageRect, mPixmap, mPixmap.rect());
}

void EImageViewer::wheelEvent(QWheelEvent *event) {
    QPoint numDegrees = event->angleDelta() / 8;
    if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        if (numSteps.y() > 0) {
            mScale += numSteps.y() / 10.0;
        }
        else if (numSteps.y() < 0) {
            mScale += numSteps.y() / 10.0;
        }
        if (mScale > 16) {
            mScale = 16;
        }
        if (mScale < 1) {
            mScale = 1;
        }
        updateDrawingSettings();
        update();
    }\
    event->accept();
    emit imageDidFinishZoom();
}

void EImageViewer::mousePressEvent(QMouseEvent *event)  {
    mReference = event->pos();
    qApp->setOverrideCursor(Qt::ClosedHandCursor);
    setMouseTracking(true);
}

void EImageViewer::mouseMoveEvent(QMouseEvent *event)  {
    mDelta += (event->pos() - mReference) * 1.0;// / mScale;
    mReference = event->pos();
    updateDrawingSettings();
    update();
}

void EImageViewer::mouseReleaseEvent(QMouseEvent *)  {
    qApp->restoreOverrideCursor();
    setMouseTracking(false);
}

void EImageViewer::setPixmap(const QPixmap &pix) {
    mPixmap = pix;
    QSize optimalSize = pix.size().scaled(rect().size(), Qt::KeepAspectRatio);
    mRect = QRectF(-mRect.center(), optimalSize);
    mRect.adjust(8,8,-8,-8);

    mReference = QPoint();
    mDelta = QPoint();
    mScale = 1.0;

    updateDrawingSettings();
    update();
}

void EImageViewer::scale(qreal s) {
    mScale *= s;
    updateDrawingSettings();
    update();
}

void EImageViewer::updateDrawingSettings() {
    QSizeF scaledSize = QSizeF(mRect.width() * mScale, mRect.height() * mScale);
    QPointF origin = QPointF(-scaledSize.width()/2, -scaledSize.height()/2);
    origin += rect().center();
    qDebug("%f %f", mDelta.x() + mRect.x(), mDelta.y());
    mImageRect = QRectF(origin, scaledSize);
}
