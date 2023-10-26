#include "Dish.hpp"
#include "Menu.hpp"
#include "Restaurant.hpp"
#include <iostream>

int main()
{

	Menu* m = new Menu();
	m->addDish("Pizza", dishTypeEnum::MAIN_COURSE, 1, 50);
	m->addDish("Pizza1", dishTypeEnum::MAIN_COURSE, 2, 51);
	m->addDish("Pizza2", dishTypeEnum::MAIN_COURSE, 3, 52);
	m->addDish("Pizza3", dishTypeEnum::MAIN_COURSE, 4, 53);
	/*m->displayMenu();*/
	
	

	// Restaurant class check
	Restaurant* sartanParich = new Restaurant();
	sartanParich->addMenu(m);
	sartanParich->displayMenu();
	

	
	//Dish newDish;


	//newDish.setDishName("PIZZA");
	//newDish.setDishPrice(50);
	//newDish.setDishType(dishTypeEnum::DESSERT);
	//newDish.displayDish();
	//

	//TESTING DYNAMIC MEM OBJECT ALLCOATION.
	/*Dish* heapDish = new Dish;
	heapDish->setDishName("YOTAM");
	heapDish->setDishPrice(1000);
	heapDish->setDishType(dishTypeEnum::MAIN_COURSE);
	heapDish->displayDish();*/

	// TESTING COPY CONSTRUCTOR.
	//Dish copiedDishObject = newDish;
	//copiedDishObject.displayDish();

	// TESTING ASSIGNMENT OPERATOR.
	//Dish assigmentObject;
	//assigmentObject.setDishName("blabla");
	//assigmentObject.displayDish();
	//assigmentObject = newDish;
	//assigmentObject.displayDish();

	// TESTING CONSTRUCTOR WITH PARAMETERS
	/*Dish tryDish("FUCKYOU", dishTypeEnum::APPETIZER, 400);
	tryDish.displayDish();*/



	return 0;
}
