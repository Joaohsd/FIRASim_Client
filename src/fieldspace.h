#ifndef FIELDSPACE_H
#define FIELDSPACE_H

#include <QObject>
#include <QPoint>

using namespace std;

class FieldSpace
{
public:
    explicit FieldSpace();
    explicit FieldSpace(QPointF topLeft, QPointF topRight, QPointF bottomLeft, QPointF bottomRight);
    QPointF getTopLeft() const;
    void setTopLeft(const QPointF &value);
    QPointF getTopRight() const;
    void setTopRight(const QPointF &value);
    QPointF getBottomLeft() const;
    void setBottomLeft(const QPointF &value);
    QPointF getBottomRight() const;
    void setBottomRight(const QPointF &value);
private:
    QPointF topLeft;
    QPointF topRight;
    QPointF bottomLeft;
    QPointF bottomRight;
};

#endif // FIELDSPACE_H
