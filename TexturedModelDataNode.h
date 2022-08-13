#ifndef TEXTUREDMODELDATANODE_H
#define TEXTUREDMODELDATANODE_H

#include <QMatrix4x4>
#include <QObject>

class TexturedModelData;

class TexturedModelDataNode : public QObject
{
    Q_OBJECT
public:
    explicit TexturedModelDataNode(TexturedModelData *data, QObject *parent = nullptr);

    void addChild(TexturedModelDataNode *child);
    void addMeshIndex(int index);

    const QVector<TexturedModelDataNode *> &children() const;

    const QString &name() const;
    void setName(const QString &newName);

    const QMatrix4x4 &initialTransformation() const;
    void setInitialTransformation(const QMatrix4x4 &newInitialTransformation);

    void render();

private:
    QVector<TexturedModelDataNode *> mChildren;
    QVector<int> mMeshIndices;
    QString mName;
    QMatrix4x4 mInitialTransformation;
    TexturedModelData *mData;
};

#endif // TEXTUREDMODELDATANODE_H
