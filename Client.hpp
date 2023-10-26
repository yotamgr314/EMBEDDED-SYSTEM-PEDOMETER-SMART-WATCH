#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "Address.hpp"


class Client
{
private:
	
	char* dm_clientName;
	const int dm_phoneNumber;
	int dm_credits;
	Address dm_homeAddress;

public:
	Client(const int PhoneNumber, Address homeAddress, ); // deafult const
	Client(const Client& other); // copy Const
	Client& operator=(const Client& other); // Assignment operator.
	~Client();

};











#endif  CLIENT_HPP
