#ifndef ADDRESS_HPP
#define ADDRESS_HPP
#define _CRT_SECURE_NO_WARNINGS




class Address
{
private:
	int dm_streetNumber;
	char* dm_cityName;


public:
	Address();// deafult constructor.
	Address(const int streetNumber, const char* cityName); // Parameters constructor.
	Address(const Address& other); // copy constructor.
	Address& operator=(const Address& other); // Assignment operator.



	void setStreetAddress(const int streetNumber);
	void setCityName(const char* cityName);


	const int getStreetNumber() const;
	const char* getCityName() const;


	~Address();

};




#endif
