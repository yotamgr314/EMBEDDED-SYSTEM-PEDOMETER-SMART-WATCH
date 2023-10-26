#include "Menu.hpp"
#include <iostream>

// Default constructor.
Menu::Menu() : dishes(new Dish[10]), dishCount(0) {}


// Copy constructor.
Menu::Menu(const Menu& other) : dishes(nullptr), dishCount(other.dishCount)
{
    if (other.dishCount > 0)
    {
        dishes = new Dish[dishCount]; // allocating an array in size of numberOfDishes (of other).
        for (int i = 0; i < dishCount; ++i)
        {
            dishes[i] = other.dishes[i]; // Using Dish's assignment operator.
        }
    }
}



// Assignment operator.
Menu& Menu::operator=(const Menu& other)
{
    if (this != &other) // Protect against self-assignment.
    {
        delete[] dishes; // Delete existing data.

        dishCount = other.dishCount;
        dishes = new Dish[dishCount];
        for (int i = 0; i < dishCount; ++i)
        {
            dishes[i] = other.dishes[i]; // Using Dish's assignment operator.
        }
    }
    return *this;
}

// Destructor.
Menu::~Menu()
{
    delete[] dishes;
}

// Add a new dish to the menu.
void Menu::addDish(const Dish& newDish)
{
    Dish* temp = new Dish[dishCount + 1];
    for (int i = 0; i < dishCount; ++i)
    {
        temp[i] = dishes[i]; // Using Dish's assignment operator.
    }
    temp[dishCount] = newDish; // Add the new dish.

    ++dishCount;
    delete[] dishes; // Delete old array.
    dishes = temp; // Point to new array.
}

void Menu::addDish(const char* dishName, dishTypeEnum dishType, const int dishID, const int dishPrice)
{
    Dish dish(dishName, dishType, dishID, dishPrice);
    dishes[dishCount++] = dish;
}

// Display all dishes in the menu.
void Menu::displayMenu() const
{
    std::cout << "MENU\n";
    std::cout << "----\n";
    for (int i = 0; i < dishCount; ++i)
    {
        dishes[i].displayDish();
    }
}

const int Menu::findDishIndexByID(const int dishID) // -1 as index indicates notFound.
{
    if (dishCount < 0)
    {
        return -1;
    }

    // Find the index of the dish to delete
    for (int i = 0; i < dishCount; ++i) {
        if (dishes[i].getDishID() == dishID)
        {
            return i;
        }
    }
    return -1;
}


void Menu::deleteDish(int dishID)
{
    // Check if there are dishes to delete
    if (dishCount == 0) {
        std::cerr << "Error: No dishes to delete." << std::endl;
        return;
    }


    int indexToDelete = findDishIndexByID(dishID);
    // Check if the dish was found
    if (indexToDelete == -1) {

        std::cerr << "Error: Dish not found." << std::endl;
        return;
    }

    // Create a new array and copy over all dishes except the one to delete
    Dish* newDishes = new Dish[dishCount - 1];
    for (int i = 0, j = 0; i < dishCount; ++i) {
        if (i != indexToDelete) {
            newDishes[j] = dishes[i];
            ++j;
        }
    }

    // Delete the old array and update the pointer and size
    delete[] dishes;
    dishes = newDishes;
    --dishCount;
}
