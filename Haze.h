#ifndef HAZE_H
#define HAZE_H

#include <QObject>
#include <QVector3D>

#include <imgui.h>
#include <QtImGui.h>

class Haze : public QObject
{
    Q_OBJECT
private:
    explicit Haze(QObject *parent = nullptr);

public:
    static Haze *instance();

    float gradient() const;
    void setGradient(float newGradient);

    float density() const;
    void setDensity(float newDensity);

    const QVector3D &color() const;
    void setColor(const QVector3D &newColor);

    bool enabled() const;
    void setEnabled(bool newEnabled);

    void drawGUI();
    void reset();

private:
    bool mEnabled;
    QVector3D mColor;
    float mDensity;
    float mGradient;
};

#endif // HAZE_H
