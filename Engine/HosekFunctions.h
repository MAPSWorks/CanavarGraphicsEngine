#ifndef HOSEKFUNCTIONS_H
#define HOSEKFUNCTIONS_H

#include <QVector3D>
#include <QtMath>

// Sky model belongs to Hosek-Wilkie - An Analytic Model for Full Spectral Sky-Dome Radiance
// Code is taken from https://github.com/diharaw/sky-models and adopted

QVector3D pow(const QVector3D &a, const QVector3D &b)
{
    return QVector3D(pow(a.x(), b.x()), pow(a.y(), b.y()), pow(a.z(), b.z()));
}

QVector3D exp(const QVector3D &a)
{
    return QVector3D(exp(a.x()), exp(a.y()), exp(a.z()));
}

QVector3D div(float t, const QVector3D &a)
{
    return QVector3D(t / a.x(), t / a.y(), t / a.z());
}

double evaluateSpline(const double *spline, size_t stride, double value)
{
    return                                                            //
        pow(1 - value, 5) * spline[0 * stride] * 1 +                  //
        pow(1 - value, 4) * pow(value, 1) * spline[1 * stride] * 5 +  //
        pow(1 - value, 3) * pow(value, 2) * spline[2 * stride] * 10 + //
        pow(1 - value, 2) * pow(value, 3) * spline[3 * stride] * 10 + //
        pow(1 - value, 1) * pow(value, 4) * spline[4 * stride] * 5 +  //
        pow(value, 5) * spline[5 * stride] * 1;                       //
                                                                      //
}

double evaluate(const double *dataset, size_t stride, float turbidity, float albedo, float sunTheta)
{
    // splines are functions of elevation^1/3
    double elevationK = pow(std::max<float>(0.f, 1.f - sunTheta / (M_PI / 2.f)), 1.f / 3.0f);

    // table has values for turbidity 1..10
    int turbidity0 = qBound(1, static_cast<int>(turbidity), 10);
    int turbidity1 = qMin(turbidity0 + 1, 10);
    float turbidityK = qBound(0.f, turbidity - turbidity0, 1.f);

    const double *datasetA0 = dataset;
    const double *datasetA1 = dataset + stride * 6 * 10;

    double a0t0 = evaluateSpline(datasetA0 + stride * 6 * (turbidity0 - 1), stride, elevationK);
    double a1t0 = evaluateSpline(datasetA1 + stride * 6 * (turbidity0 - 1), stride, elevationK);
    double a0t1 = evaluateSpline(datasetA0 + stride * 6 * (turbidity1 - 1), stride, elevationK);
    double a1t1 = evaluateSpline(datasetA1 + stride * 6 * (turbidity1 - 1), stride, elevationK);

    return a0t0 * (1 - albedo) * (1 - turbidityK) + a1t0 * albedo * (1 - turbidityK) + a0t1 * (1 - albedo) * turbidityK + a1t1 * albedo * turbidityK;
}

QVector3D hosekWilkie(float cos_theta,
                      float gamma,
                      float cos_gamma,
                      const QVector3D &A,
                      const QVector3D &B,
                      const QVector3D &C,
                      const QVector3D &D,
                      const QVector3D &E,
                      const QVector3D &F,
                      const QVector3D &G,
                      const QVector3D &H,
                      const QVector3D &I)
{
    auto chi = div((1.f + cos_gamma * cos_gamma), pow(QVector3D(1, 1, 1) + (H * H) - 2.f * cos_gamma * H, QVector3D(1.5f, 1.5f, 1.5f)));
    return (QVector3D(1, 1, 1) + A * exp(B / (cos_theta + 0.01f))) * (C + D * exp(E * gamma) + F * (cos_gamma * cos_gamma) + G * chi + I * (float) sqrt(std::max(0.f, cos_theta)));
}

#endif // HOSEKFUNCTIONS_H
