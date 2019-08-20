#pragma once

#include <string>

class Stat
{
public:
	Stat(std::string name, int initialValue);
	~Stat();

	const std::string name;

	int getValue();

	void increase(int amount);
	void decrease(int amount);

	friend Stat operator+(Stat lhs, const Stat& rhs)
	{
		lhs.value += rhs.value;
		return lhs;
	}

	Stat& operator+=(const Stat& other)
	{
		(*this).value += other.value;
		return *this;
	}

	friend Stat operator-(Stat lhs, const Stat& rhs)
	{
		lhs.value -= rhs.value;
		return lhs;
	}

	Stat& operator-=(const Stat& other)
	{
		(*this).value -= other.value;
		return *this;
	}

private:
	const int MAX_VALUE = 100;
	const int MIN_VALUE = 0;
	int value;

	void clamp();
};