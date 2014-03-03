#include <QtGui>
#include "scribblearea.h"

ScribbleArea::ScribbleArea(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    modified = false;
    scribbling = false;

    myPenWidth = 5;
    myPenColor = Qt::blue;
    myBrushColor = Qt::white;
    myPenStyle = Qt::SolidLine;
    myBrushStyle = Qt::NoBrush;

    myShape = PENCIL;
    ratioXRad = 8;
    ratioYRad = 8;

    image = QImage(size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgb(255, 255, 255));

    imageHistory.clear();
    imageHistory.append(image);
    idxHistory = 0;
}

bool ScribbleArea::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    update();

    imageHistory.clear();
    imageHistory.append(image);
    idxHistory = 0;

    return true;
}

bool ScribbleArea::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;
        return true;
    } else {
        return false;
    }
}

void ScribbleArea::setPenColor(const QColor &newColor)
{
    myPenColor = newColor;
}

void ScribbleArea::setBrushColor(const QColor &newColor)
{
    myBrushColor = newColor;
}

void ScribbleArea::setPenWidth(int newWidth)
{
    myPenWidth = newWidth;
}

void ScribbleArea::setPenStyle(const Qt::PenStyle newPenStyle)
{
    myPenStyle = newPenStyle;
}

void ScribbleArea::setBrushStyle(const Qt::BrushStyle newBrushStyle)
{
    myBrushStyle = newBrushStyle;
}

void ScribbleArea::setShape(const Shape newShape)
{
    myShape = newShape;
}

void ScribbleArea::clearImage()
{
    image.fill(qRgb(255, 255, 255));
    modified = true;
    imageHistory.append(image);
    idxHistory++;
    update();
}

void ScribbleArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        scribbling = true;
    }
}

void ScribbleArea::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && scribbling) {
        if (myShape != PENCIL && myShape != ERASER)
            image = imageHistory[idxHistory];
        drawShape(event->pos(), myShape);
    }
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
        drawShape(event->pos(), myShape);
        scribbling = false;
        modified = true;
        lastPoint = event->pos();

        imageHistory.erase(imageHistory.begin() + idxHistory+1, imageHistory.end());
        imageHistory.append(image);
        idxHistory++;
    }
}

void ScribbleArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);
}

void ScribbleArea::resizeEvent(QResizeEvent *event)
{
    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void ScribbleArea::drawShape(const QPointF endPoint, const Shape shape)
{
    QPainter painter(&image);
    QColor color = (shape == ERASER)? myBrushColor : myPenColor;
    painter.setPen(QPen(color, myPenWidth, myPenStyle, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.setBrush(QBrush(myBrushColor, myBrushStyle));

    QPointF p = endPoint - lastPoint;
    switch (shape)
    {
        case ERASER:
        case PENCIL:
            painter.drawLine(lastPoint, endPoint);
            lastPoint = endPoint;
            break;
        case LINE:
            painter.drawLine(lastPoint, endPoint);
            break;
        case RECT:
            painter.drawRect(QRectF(lastPoint, endPoint));
            break;
        case ROUNDRECT:
            painter.drawRoundedRect(QRectF(lastPoint, endPoint),
                                    p.x()*ratioXRad/100,
                                    p.y()*ratioYRad/100       );
            break;
        case ELLIPSE:
            painter.drawEllipse(QRectF(lastPoint, endPoint));
            break;
        case POLYGON:
        case TEXT:
        case BRUSH:
        default:
            break;
    }

    update();
}

void ScribbleArea::resizeImage(QImage *image, const QSize &newSize)
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

void ScribbleArea::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
 
    QPrintDialog *printDialog = new QPrintDialog(&printer, this);
    if (printDialog->exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = image.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(image.rect());
        painter.drawImage(0, 0, image);
    }
#endif // QT_NO_PRINTER
}

void ScribbleArea::moveHistory(int x)
{
    if ( (idxHistory + x) < 0 || (idxHistory + x) >= imageHistory.size() )
        return;

    idxHistory += x;
    image = imageHistory[idxHistory];
    update();
}
