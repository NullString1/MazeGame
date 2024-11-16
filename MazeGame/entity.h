#pragma once
#include "gameObject.h"


class Entity: virtual public GameObject {
public:
	void draw() override;
};
