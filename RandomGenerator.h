#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include <QObject>
#include <QRandomGenerator>

class RandomGenerator : public QObject
{
    Q_OBJECT
private:
    explicit RandomGenerator(QObject *parent = nullptr);

public:
    static float getRandomFloat(float bound);
    static QVector3D getRandomVec3(float x = 1.0, float y = 1.0, float z = 1.0);

private:
    static QRandomGenerator mGenerator;
    static RandomGenerator *mInstance;
};

#endif // RANDOMGENERATOR_H
