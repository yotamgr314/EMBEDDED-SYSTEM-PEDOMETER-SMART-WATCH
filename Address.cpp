#include "Address.hpp"
#include <cstring> // for strcpy and strlen
#include <iostream>


Address::Address() :dm_streetNumber(0), dm_cityName(nullptr) {}


Address::Address(const int streetAddress, const char* cityName) : dm_streetNumber(streetAddress)
{
	dm_cityName = new char[strlen(cityName) + 1];
	if (dm_cityName == nullptr)
	{
		throw "ERROR: DM_CITYNAME MEMFAIL";
	}
	strcpy(dm_cityName, cityName);

}

Address::Address(const Address& other) : dm_streetNumber(other.dm_streetNumber)
{
	if (other.dm_cityName != nullptr)
	{
		dm_cityName = new char[strlen(other.dm_cityName) + 1];
		if (dm_cityName == nullptr)
		{
			throw "ERROR: DM_CITYNAME MEMFAIL";
		}
		strcpy(dm_cityName, other.dm_cityName);
	}
	else
	{
		dm_cityName = nullptr;
	}
}

Address& Address::operator=(const Address& other)
{
	if (this == &other) // checks if both adresses are the same. , this is a pointer, hense its value is adress.
	{
		return *this;
	}

	delete[] dm_cityName;

	dm_streetNumber = other.dm_streetNumber;

	if (other.dm_cityName != nullptr)
	{
		dm_cityName = new char[strlen(other.dm_cityName) + 1];
		if (dm_cityName == nullptr)
		{
			throw "ERROR: DM_CITYNAME MEMFAIL";
		}
		strcpy(dm_cityName, other.dm_cityName);
	}
	else
	{
		dm_cityName = nullptr;
	}

	return *this;
}


void Address::setStreetAddress(const int streetNumber)
{
	if (streetNumber < 0)
	{
		throw "**ERROR: STREET NUMBER MUST BE POSITIVE**";
	}
	dm_streetNumber = streetNumber;
}

void Address::setCityName(const char* cityName)
{
	if (cityName == nullptr)
	{
		throw "**ERROR: INVALIDE ARGUMENT - PASSED NULLPTR";
	}

	delete[] dm_cityName; // freeing any existing dynamic memory allocated of prev name.

	dm_cityName = new char[strlen(cityName) + 1];
	if (dm_cityName == nullptr)
	{
		throw "**ERROR: DM_CITYNAME MEMEALOCATION FAILED**";
	}
	strcpy(dm_cityName, cityName);
}


const int Address::getStreetNumber() const
{
	return dm_streetNumber;
}


const char* Address::getCityName() const
{
	return dm_cityName;
}

Address::~Address()
{
	delete[] dm_cityName;

}
