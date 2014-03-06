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
    Shape shape() const { return myShape; }

    void copySelectedImage();
    void clearSelected(bool clearArea);
    void togglePasting() { pasting = !pasting; }

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
    void drawShape(const QPoint endPoint, const Shape);
    void resizeImage(QImage *image, const QSize &newSize);

    bool modified;
    bool selected;
    bool scribbling;
    bool pasting;
    int myPenWidth;

    QColor myPenColor;
    QColor myBrushColor;
    Qt::PenStyle myPenStyle;
    Qt::BrushStyle myBrushStyle;
    QPoint lastPoint;
    QPoint* polyPoints;

    QImage image;
    QImage selectedImage;
    QRect  selectedArea;
    QList<QImage> imageHistory;
    int idxHistory;

    Shape myShape;
};

#endif
