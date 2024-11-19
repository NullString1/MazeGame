#include "goal.h"

void Goal::setVisible(const bool visible) {
	this->visible = visible;
}
bool Goal::isVisible() const
{
	return this->visible;
}
GLuint Goal::getTexture() {
	return Goal::texture;
}
float Goal::getSize() {
	return 0.05f;
}
GLuint Peppermint::getTexture() {
	return Peppermint::texture;
}
GLuint Lock::getTexture() {
	return Lock::texture;
}
