#include "fieldspace.h"

FieldSpace::FieldSpace()
{
    this->topLeft = QPointF(-1,-1);
    this->topRight = QPointF(-1,-1);
    this->bottomLeft = QPointF(-1,-1);
    this->bottomRight = QPointF(-1,-1);
}

FieldSpace::FieldSpace(QPointF topLeft, QPointF topRight, QPointF bottomLeft, QPointF bottomRight)
{
    this->topLeft = topLeft;
    this->topRight = topRight;
    this->bottomLeft = bottomLeft;
    this->bottomRight = bottomRight;
}

QPointF FieldSpace::getTopLeft() const
{
    return topLeft;
}

void FieldSpace::setTopLeft(const QPointF &value)
{
    topLeft = value;
}

QPointF FieldSpace::getTopRight() const
{
    return topRight;
}

void FieldSpace::setTopRight(const QPointF &value)
{
    topRight = value;
}

QPointF FieldSpace::getBottomLeft() const
{
    return bottomLeft;
}

void FieldSpace::setBottomLeft(const QPointF &value)
{
    bottomLeft = value;
}

QPointF FieldSpace::getBottomRight() const
{
    return bottomRight;
}

void FieldSpace::setBottomRight(const QPointF &value)
{
    bottomRight = value;
}
