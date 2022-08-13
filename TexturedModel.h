#ifndef TEXTUREDMODEL_H
#define TEXTUREDMODEL_H

#include "Node.h"

class TexturedModel : public Node
{
public:
    explicit TexturedModel(const QString &name, QObject *parent = nullptr);

    const QString &name() const;

    float shininess() const;
    void setShininess(float newShininess);

private:
    QString mName;
    float mShininess;
};

#endif // TEXTUREDMODEL_H
