#ifndef COMMON_H
#define COMMON_H

enum Shape {
    PENCIL,
    LINE,
    RECT,
    ROUNDRECT,
    ELLIPSE,
    POLYGON,
    TEXT,
    ERASER,
    PIE,
    CURVE,
    SELECT
};

enum Item
{
   PathItem,
   RectItem,
   EllipseItem,
   PolygonItem,
   LineItem,
   PixmapItem
};

/*
enum DrawMode {
    DRAW_CURVE,
    DRAW_LINE,
    DRAW_RECTANGLE,
    DRAW_ELLIPSE,
    SELECT_RECTANGLE
};
*/

#endif // COMMON_H
