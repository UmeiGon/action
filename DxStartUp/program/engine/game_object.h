#pragma once
#include "../library/translation.h"

class GameObject : public Translation {
public:
	bool is_life;
	virtual ~GameObject() {
		is_life = true;
	};

	// ���z�֐�
	virtual void update() {};
	virtual void render() {};
};
