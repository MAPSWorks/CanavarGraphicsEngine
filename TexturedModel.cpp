#include "TexturedModel.h"

TexturedModel::TexturedModel(const QString &name, QObject *parent)
    : Node(parent)
    , mName(name)
{}

const QString &TexturedModel::name() const
{
    return mName;
}
