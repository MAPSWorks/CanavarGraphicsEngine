#ifndef TEXTUREDMODEL_H
#define TEXTUREDMODEL_H

#include "Node.h"

class TexturedModel : public Node
{
public:
    explicit TexturedModel(const QString &modelName, QObject *parent = nullptr);

    float shininess() const;
    void setShininess(float newShininess);

    const QString &modelName() const;

private:
    float mShininess;
    QString mModelName;
};

#endif // TEXTUREDMODEL_H
