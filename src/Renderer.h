#ifndef RENDERER_H
#define RENDERER_H

#include <string>

#include "ofMain.h"
#include "Ray.h"
#include "SceneObject.h"

class Renderer
{
public:
	enum RenderMethod{
		RAY_TRACE,
		RAY_MARCH,
	};

	static const int imageWidth{ 600 };
	static const int imageHeight{ 400 };

private:
	static const int MAX_RAY_STEPS{ 200 };
	static const float DIST_THRESHOLD;
	static const float MAX_DISTANCE;

	RenderCam renderCam_m;
	ofImage image_m;
	std::vector<SceneObject *> &scene_m;
	std::vector<Light *> &lights_m;
	Light* &ambientLight_m;
	int nearestObj_m = -1;

public:
	Renderer(std::vector<SceneObject *> &scene, std::vector<Light *> &lights, Light* &ambientLight) :
		scene_m{ scene }, 
		lights_m{ lights }, 
		ambientLight_m{ ambientLight }
	{
	}
	void render(std::string filename, RenderMethod rend);

	bool inShadow(Ray pointToLight, glm::vec3 lightPos);
	ofColor phong(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power, RenderMethod rend);
	float sceneSDF(const glm::vec3 &p);
	void draw() { renderCam_m.draw(); }

private:
	bool rayTraceHit(Ray r, glm::vec3 &nearestPoint, glm::vec3 &nearestNormal);
	bool rayMarchHit(Ray r, glm::vec3 &nearestPoint, glm::vec3 &nearestNormal);
	glm::vec3 getNormalRM(const glm::vec3 &nearestPoint);
};



#endif