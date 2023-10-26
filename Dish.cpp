#include "Dish.hpp"
#include <cstring> // for strcpy and strlen
#include <iostream>




// DEAFULT CONSTRUCTOR.
Dish::Dish() : dm_dishName(nullptr), dm_dishType(dishTypeEnum::UNKNOWN), dm_dishPrice(0), dm_dishID(0) {} // initlize list to intilize memebers directly.

// PARAMETER CONSTRUCTOR.
Dish::Dish(const char* dishName, dishTypeEnum dishType, const int dishID, const int dishPrice) : dm_dishType(dishType), dm_dishPrice(dishPrice), dm_dishID(dishID)
{
	dm_dishName = new char[strlen(dishName) + 1];
	if (dm_dishName == nullptr)
	{
		throw "**ERROR: DISH NAME MEMFAIL";
	}
	strcpy(dm_dishName, dishName);

}







// Copy constructor
Dish::Dish(const Dish& dishToCopyFrom)
{
	dm_dishType = dishToCopyFrom.dm_dishType;
	dm_dishPrice = dishToCopyFrom.dm_dishPrice;
	dm_dishID = dishToCopyFrom.dm_dishID;
	if (dishToCopyFrom.dm_dishName)
	{
		dm_dishName = new char[strlen(dishToCopyFrom.dm_dishName) + 1];
		strcpy(dm_dishName, dishToCopyFrom.dm_dishName);
	}
	else
	{
		dm_dishName = nullptr;
	}
}

// Assigment operator
Dish& Dish::operator=(const Dish& dishToAssignFrom)
{
	if (this == &dishToAssignFrom) // special case: self assigmnment dish1 = dish1.
	{
		return *this; // derefrencing "this pointer" - by doing so return a refrence to the actual object.
	}

	delete[] dm_dishName; // Free existing resources (if any)

	dm_dishType = dishToAssignFrom.dm_dishType;
	dm_dishPrice = dishToAssignFrom.dm_dishPrice;
	dm_dishID = dishToAssignFrom.dm_dishID;

	if (dishToAssignFrom.dm_dishName != nullptr)
	{
		dm_dishName = new char[strlen(dishToAssignFrom.dm_dishName) + 1]; // allocating new resources && copy.
		if (dm_dishName == nullptr)
		{
			throw "**ERROR: DYNAMIC DM_DISHNAME MEMFAIL**";
		}
		strcpy(dm_dishName, dishToAssignFrom.dm_dishName);
	}

	else
	{
		dm_dishName = nullptr;
	}

	return *this;

}



const char* Dish::getDishName() const
{
	if (dm_dishName == nullptr)
	{
		return "UNINITIALIZED";
	}

	return dm_dishName;
}

const int Dish::getDishPrice() const
{
	return dm_dishPrice;
}

const char* Dish::getDishType() const
{
	switch (dm_dishType)
	{
	case dishTypeEnum::APPETIZER:
		return "APPETIZER";
	case dishTypeEnum::MAIN_COURSE:
		return "MAIN_COURSE";
	case dishTypeEnum::DESSERT:
		return "DESSERT";
	case dishTypeEnum::UNKNOWN:
		return "UNKNOWN";
	default:
		return "INVALID_TYPE";
	}
}


const int Dish::getDishID() const
{
	return dm_dishID;
}




void Dish::setDishName(const char* dishName)
{
	delete[] dm_dishName;
	dm_dishName = new char[strlen(dishName) + 1];
	strcpy(dm_dishName, dishName);
}

void Dish::setDishPrice(const int dishPrice)
{
	if (dishPrice < 0)
	{
		throw "**ERROR: DISH PRICE MUST BE POSITIVE NUMBER**";
	}
	// if(dishPrice paramter is not integer.. check with utility Class and throw error)...
	dm_dishPrice = dishPrice;
}

void Dish::setDishType(dishTypeEnum dishType)
{
	dm_dishType = dishType;
}

void Dish::setDishID(const int dishID)
{
	dm_dishID = dishID;
}


void Dish::displayDish()
{
	std::cout << "**DISH DISPLAY**" << std::endl;
	std::cout << "----------------" << std::endl;

	std::cout << "DISH NAME: " << getDishName() << std::endl;
	std::cout << "DISH PRICE: " << getDishPrice() << std::endl;
	std::cout << "DISH TYPE: " << getDishType() << std::endl;

	std::cout << std::endl;

}

Dish::~Dish()
{
	delete[] dm_dishName;
}
