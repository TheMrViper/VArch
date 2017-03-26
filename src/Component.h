#ifndef COMPONENT_H
#define COMPONENT_H

class Component {

public:
	Component() {}
	~Component() {}

	virtual void update() = 0;
	virtual void update(glm::mat4 model) {}
};

#endif //COMPONENT_H