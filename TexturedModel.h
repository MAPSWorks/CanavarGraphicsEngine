#ifndef TEXTUREDMODEL_H
#define TEXTUREDMODEL_H

#include "Node.h"

class TexturedModel : public Node
{
public:
    explicit TexturedModel(QObject *parent = nullptr);

    const QString &path() const;
    void setPath(const QString &newPath);

private:
    QString mPath;
};

#endif // TEXTUREDMODEL_H
