#ifndef UID_H
#define UID_H

#include <array>

class Uid
{
public:
    enum Part{
        Dataset     = 0,
        CountryCode = 1,
        Subdivision = 2,
        Key         = 3
    };

    Uid();
    Uid(int dataset, int countryCode, int subdivision, int key);

    int getDataset() const;
    int getCountryCode() const;
    int getSubdivision() const;
    int getKey() const;

private:
    std::array<int, 4> parts;
};

#endif // UID_H
