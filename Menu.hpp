#ifndef MENU_HPP
#define MENU_HPP
#include "Dish.hpp"

class Menu
{
private:
    Dish* dishes;
    int dishCount;

public:
    Menu(); // Default constructor.
    Menu(const Menu& other); // Copy constructor.
    Menu& operator=(const Menu& other); // Assignment operator.
    ~Menu(); // Destructor.

    void addDish(const Dish& newDish);
    void addDish(const char* dishName, dishTypeEnum dishType, const int dishID, const int dishPrice);
    void displayMenu() const;
    void deleteDish(const int arrayIndex);
    const int findDishIndexByID(const int dishID);
};

#endif


