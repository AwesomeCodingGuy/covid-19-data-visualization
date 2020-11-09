#include "utility.h"

#include <QList>

QVector<int> calculateIncrease(const QVector<int> &cumulatedIn)
{
    QVector<int> out(cumulatedIn.size());

    out[0] = cumulatedIn[0];
    for(int i = 1; i < cumulatedIn.size(); ++i) {
        out[i] = cumulatedIn[i] - cumulatedIn[i-1];
    }

    return out;
}

QVector<float> calculateAveragedIncrease(const QVector<int> &in)
{
    // init out vector
    QVector<float> out(in.size());

    // init first 7 values
    int cumulatedValues = 0;
    for(int i = 0; i < 7 && i < in.size(); ++i) {
        cumulatedValues += in[i];
        out[i] = cumulatedValues / static_cast<float>(i + 1);
    }

    // calculate all following values
    for(int i = 7; i < in.size(); ++i) {
        cumulatedValues = cumulatedValues + in[i] - in[i - 7];
        out[i] = cumulatedValues / 7.0f;
    }

    return out;
}

void escapeCommasInStrings(QByteArray &array)
{
    // commas that need to be escaped are only allowed to appear in quotes
    // it is assumed that quotes appear in pairs
    while(array.contains('"')) {
        // get ptr to opening and closing quote
        int first = array.indexOf('"', 0);
        int second = array.indexOf('"', first + 1);

        // if no more quotes exist it is done
        // !!! this should not happen in well formed csv files
        if(second == -1)
            return;

        // escape all commas
        QByteArray sub = array.mid(first + 1, second - first - 1);
        sub.replace(",", "::comma::");

        // replace old string with escaped sub string
        array.replace(first, second - first + 1, sub);
    }
}

void resolveEscapes(QList<QByteArray> &list)
{
    for(QByteArray &item : list) {
        item.replace("::comma::", ",");
    }
}
