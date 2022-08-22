#ifndef SKY_H
#define SKY_H

#include <QObject>
#include <QVector4D>

class Sky : public QObject
{
    Q_OBJECT
public:
    explicit Sky(QObject *parent = nullptr);

    const QVector4D &skyColorTop() const;
    const QVector4D &skyColorBottom() const;

private:
    QVector4D mSkyColorTop;
    QVector4D mSkyColorBottom;
};

#endif // SKY_H
