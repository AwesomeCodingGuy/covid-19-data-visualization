#ifndef UTILITY_H
#define UTILITY_H

#include <QVector>
#include <QByteArray>

#include <functional>
#include <algorithm>

QVector<int> calculateIncrease(const QVector<int> &cumulatedIn);
QVector<float> calculateAveragedIncrease(const QVector<int> &in);

void resolveEscapes(QList<QByteArray> &list);
void escapeCommasInStrings(QByteArray &array);

template<typename T>
void addVectors(QVector<T> &sourceAndResult, const QVector<T> &source2)
{
    std::transform(sourceAndResult.begin(), sourceAndResult.end(),
                   source2.begin(), sourceAndResult.begin(), std::plus<T>());
}

#endif // UTILITY_H
