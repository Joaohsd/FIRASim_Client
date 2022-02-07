#ifndef FIELDSPACE_H
#define FIELDSPACE_H

#include <QObject>
#include <QPoint>

using namespace std;

class FieldSpace
{
public:
    explicit FieldSpace();
    explicit FieldSpace(QPoint topLeft, QPoint topRight, QPoint bottomLeft, QPoint bottomRight);
    QPoint getTopLeft() const;
    void setTopLeft(const QPoint &value);
    QPoint getTopRight() const;
    void setTopRight(const QPoint &value);
    QPoint getBottomLeft() const;
    void setBottomLeft(const QPoint &value);
    QPoint getBottomRight() const;
    void setBottomRight(const QPoint &value);
private:
    QPoint topLeft;
    QPoint topRight;
    QPoint bottomLeft;
    QPoint bottomRight;
};

#endif // FIELDSPACE_H
