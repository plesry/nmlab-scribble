#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>

#include "common.h"

class ScribbleArea : public QWidget
{
    Q_OBJECT

public:
    ScribbleArea(QWidget *parent = 0);

    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void setPenColor(const QColor &newColor);
    void setBrushColor(const QColor &newColor);
    void setPenWidth(int newWidth);
    void setPenStyle(const Qt::PenStyle newPenStyle);
    void setBrushStyle(const Qt::BrushStyle newBrushStyle);
    void setShape(const Shape newShape);

    void moveHistory(int x);

    bool isModified() const { return modified; }
    QColor penColor() const { return myPenColor; }
    QColor brushColor() const { return myBrushColor; }
    int penWidth() const { return myPenWidth; }
    Qt::PenStyle penStyle() const { return myPenStyle; }
    Qt::BrushStyle brushStyle() const { return myBrushStyle; }

public slots:
    void clearImage();
    void print();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void drawShape(const QPointF endPoint, const Shape);
    void resizeImage(QImage *image, const QSize &newSize);

    bool modified;
    bool scribbling;
    int myPenWidth;

    QColor myPenColor;
    QColor myBrushColor;
    Qt::PenStyle myPenStyle;
    Qt::BrushStyle myBrushStyle;
    QPointF lastPoint;

    QImage image;
    QList<QImage> imageHistory;
    int idxHistory;

    Shape myShape;
    int ratioXRad;
    int ratioYRad;
};

#endif
