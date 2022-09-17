#ifndef EIMAGEVIEWER_H
#define EIMAGEVIEWER_H

#include <QWidget>

class EImageViewer : public QWidget
{
    Q_OBJECT
    QPixmap mPixmap;
    QRectF mRect;
    QPointF mReference;
    QPointF mDelta;
    QRectF mImageRect;

    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *event) override;

    void updateDrawingSettings();

public:
    qreal mScale = 1.0;

    explicit EImageViewer(QWidget *parent = nullptr);

    void setPixmap(const QPixmap &pix);
    void scale(qreal s);
    QSize sizeHint() const override { return {400, 400}; }

signals:
    void imageDidFinishZoom();
};

#endif // EIMAGEVIEWER_H
