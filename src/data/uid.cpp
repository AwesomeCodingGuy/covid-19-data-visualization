#include "uid.h"

Uid::Uid()
    : parts(std::array<int, 4>{0, 0, 0, 0})
{

}
Uid::Uid(int dataset, int countryCode, int subdivision, int key)
    : parts(std::array<int, 4>{dataset, countryCode, subdivision, key})
{

}

int Uid::getDataset() const
{
    return parts.at(Part::Dataset);
}


int Uid::getCountryCode() const
{
    return parts.at(Part::CountryCode);
}

int Uid::getSubdivision() const
{
    return parts.at(Part::Subdivision);
}

int Uid::getKey() const
{
    return parts.at(Part::Key);
}
