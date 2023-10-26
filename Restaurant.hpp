#ifndef RESTAURANT_HPP
#define RESTAURANT_HPP

#include "Address.hpp"
#include "Menu.hpp"

class Restaurant
{
private:
	char* dm_restaurantName;
	Address* dm_restaurantAddress;
	Menu dm_restaurantMenu;

public:
	Restaurant();// default const' - make sure to do it like the deafult const' of Menu
	Restaurant(const Restaurant& other); // Copy constructor.
	Restaurant& operator=(const Restaurant& other); // Assignment operator.
	~Restaurant(); // distructor. 
	
	void addMenu(const Menu* newMenu);
	
	void displayMenu() const;
};



#endif