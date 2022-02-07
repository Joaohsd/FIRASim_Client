#include "fieldspace.h"

FieldSpace::FieldSpace()
{
    this->topLeft = QPoint(-1,-1);
    this->topRight = QPoint(-1,-1);
    this->bottomLeft = QPoint(-1,-1);
    this->bottomRight = QPoint(-1,-1);
}

FieldSpace::FieldSpace(QPoint topLeft, QPoint topRight, QPoint bottomLeft, QPoint bottomRight)
{
    this->topLeft = topLeft;
    this->topRight = topRight;
    this->bottomLeft = bottomLeft;
    this->bottomRight = bottomRight;
}

QPoint FieldSpace::getTopLeft() const
{
    return topLeft;
}

void FieldSpace::setTopLeft(const QPoint &value)
{
    topLeft = value;
}

QPoint FieldSpace::getTopRight() const
{
    return topRight;
}

void FieldSpace::setTopRight(const QPoint &value)
{
    topRight = value;
}

QPoint FieldSpace::getBottomLeft() const
{
    return bottomLeft;
}

void FieldSpace::setBottomLeft(const QPoint &value)
{
    bottomLeft = value;
}

QPoint FieldSpace::getBottomRight() const
{
    return bottomRight;
}

void FieldSpace::setBottomRight(const QPoint &value)
{
    bottomRight = value;
}
