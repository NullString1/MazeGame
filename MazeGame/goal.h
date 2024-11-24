#pragma once
#include "glad/glad.h"
#include "gameObject.h"
#include <utility>
#include <vector>

class Goal : public GameObject {
public:
	void setVisible(bool visible);
	bool isVisible() const;
	GLuint getTexture() override;
	float getSize() override;
	inline static GLuint texture;
	using GameObject::GameObject;
private:
	bool visible = true;
};

class Peppermint : public Goal {
public:
	GLuint getTexture() override;
	inline static GLuint texture;
	using Goal::Goal;
};

class Lock : public Goal {
public:
	GLuint getTexture() override;
	inline static GLuint texture;
	std::pair<const char*, const char*>* question;
	bool showQuestion = false;
	Lock(Cell* cell);
	void newQuestion();
	using Goal::Goal;
private:
	inline static std::vector<std::pair<const char*, const char*>> questions = {
		std::make_pair("What is the capital of France?", "paris"),
		std::make_pair("What is the capital of Germany?", "berlin"),
		std::make_pair("What is 8*8?", "64"),
		std::make_pair("What is 9*9?", "81"),
		std::make_pair("What is 10*10?", "100"),
	};
};