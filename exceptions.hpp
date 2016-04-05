#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <iostream>
#include <exception>
#include <string>

class IndexError : public std::exception
{
private:
	std::string msg;

public:
	IndexError(std::string msg_):
		msg(msg_) {}
	
	const char* what() const throw()
	{
		return msg.c_str();
	}
};


class DimensionError : public std::exception
{
private:
	std::string msg;

public:
	DimensionError(std::string msg_):
		msg(msg_) {}
	
	const char* what() const throw()
	{
		return msg.c_str();
	}
};


#endif /* EXCEPTIONS_H */
