#ifndef RAY_H
#define RAY_H

#include "ofMain.h"

class Ray 
{
private:
	glm::vec3 pos_m;
	glm::vec3 dir_m;

public:
	Ray(glm::vec3 pos, glm::vec3 dir) : pos_m{ pos }, dir_m{ dir }
	{
	}

	void draw(float t) { ofDrawLine(pos_m, pos_m + t * dir_m); }
	glm::vec3 evalPoint(float t) const { return (pos_m + t * dir_m); }
	glm::vec3 getPosition() const { return pos_m; }
	glm::vec3 getDirection() const { return dir_m; }
};

#endif