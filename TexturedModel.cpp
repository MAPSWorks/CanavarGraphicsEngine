#include "TexturedModel.h"

TexturedModel::TexturedModel(QObject *parent)
    : Node{parent}
{}

const QString &TexturedModel::path() const
{
    return mPath;
}

void TexturedModel::setPath(const QString &newPath)
{
    mPath = newPath;
}
