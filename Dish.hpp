#ifndef DISH_HPP
#define DISH_HPP
#define _CRT_SECURE_NO_WARNINGS

enum class dishTypeEnum
{
	APPETIZER,
	MAIN_COURSE,
	DESSERT,
	UNKNOWN
};

class Dish
{
private:
	char* dm_dishName;
	dishTypeEnum dm_dishType;
	int dm_dishPrice;
	int dm_dishID;

public:
	Dish(); // deafult constructor.
	Dish(const char* dishName, dishTypeEnum dishType, const int dishID, const int dishPrice); // Constructor
	Dish(const Dish& dishToCopyFrom); // copyConstructor.
	Dish& operator=(const Dish& dishToAssignFrom); // Assignment operator.

	const char* getDishName() const;
	const int getDishPrice() const;
	const char* getDishType() const;
	const int getDishID() const;

	void setDishName(const char* dishName);
	void setDishPrice(const int dishPrice);
	void setDishType(dishTypeEnum dishType);
	void setDishID(const int dishID);
	void displayDish();

	~Dish();

};

#endif

 