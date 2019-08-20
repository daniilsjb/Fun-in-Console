#pragma once

#include "Object.h"

class Sign : public Object
{
public:
	Sign(std::string name, std::string message);
	~Sign();

	void onInteract(Game* context) override;

private:
	std::string message;
};

