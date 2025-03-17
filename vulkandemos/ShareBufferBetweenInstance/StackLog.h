#pragma once
#include "include.h"
class StackLog
{
public:
	StackLog(int& logStack, std::string logstr)
		: logStack(logStack)
	{
		for (int i = 0; i < logStack; i++)
		{
			std::cout << "  ";
		}
		std::cout << logstr << std::endl;
		logStack++;
	}
	~StackLog() {
		logStack--;
	}
	int& logStack;
};