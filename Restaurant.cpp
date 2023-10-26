#include "Restaurant.hpp"
#include <cstring> // for strcpy and strlen
#include "Address.hpp"


Restaurant::Restaurant() : dm_restaurantName(nullptr), dm_restaurantAddress(nullptr) {} // Default constructor.

Restaurant::Restaurant(const Restaurant& other) : dm_restaurantMenu(other.dm_restaurantMenu) // Copy constuctor.
{
	if (other.dm_restaurantName != nullptr)
	{
		dm_restaurantName = new char[strlen(other.dm_restaurantName) + 1];
		if (dm_restaurantName == nullptr)
		{
			throw "**ERROR: RESTAURANT NAME MEMFAIL";
		}
		strcpy(dm_restaurantName, other.dm_restaurantName);
	}

}


// Restaurant.cpp
Restaurant& Restaurant::operator=(const Restaurant& other) 
{
    if (this != &other)
    {
        delete[] dm_restaurantName;
        dm_restaurantName = nullptr;
        if (other.dm_restaurantName != nullptr) {
            dm_restaurantName = new char[strlen(other.dm_restaurantName) + 1];
            strcpy(dm_restaurantName, other.dm_restaurantName);
        }

        delete dm_restaurantAddress;
        dm_restaurantAddress = nullptr;
        if (other.dm_restaurantAddress != nullptr)
        {
            dm_restaurantAddress = new Address(*other.dm_restaurantAddress);
        }

        dm_restaurantMenu = other.dm_restaurantMenu;
    }

    return *this;
}


Restaurant::~Restaurant()
{
    delete dm_restaurantName;
    delete dm_restaurantAddress;
}

void Restaurant::addMenu(const Menu* newMenu)
{
    dm_restaurantMenu = *newMenu;
}

void Restaurant::displayMenu() const
{
    dm_restaurantMenu.displayMenu();
}


