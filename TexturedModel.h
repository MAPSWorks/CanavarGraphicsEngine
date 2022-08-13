#ifndef TEXTUREDMODEL_H
#define TEXTUREDMODEL_H

#include "Node.h"

class TexturedModel : public Node
{
public:
    explicit TexturedModel(const QString &name, QObject *parent = nullptr);

    const QString &name() const;

private:
    QString mName;
};

#endif // TEXTUREDMODEL_H
