#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "ofMain.h"
#include "SceneObject.h"

struct KeyFrame
{
	glm::vec3 translate_m;
	glm::vec3 rotate_m;
	//glm::vec3 scale_m;
	int cFrame_m;
};

class Animator
{
private:
	std::vector<KeyFrame *> framesStart_m;
	std::vector<KeyFrame *> framesEnd_m;
	std::vector<SceneObject *> &scene_m;

	int currentFrame_m = 1;
	int minFrame_m = 1;
	int maxFrame_m;
	bool play_m = false;
	bool startSet_m = false;
	bool endSet_m = false;

public:
	Animator(std::vector<SceneObject *> &scene, int maxFrame = 60) : scene_m{ scene }, maxFrame_m { maxFrame }
	{
	}

	void play();
	void stop() { play_m = false; }
	void setCurrentFrame(int frame) 
	{
		if (!play_m) {
			currentFrame_m = frame;
		}
	}
	int getMinFrame() { return minFrame_m; }
	int getMaxFrame() { return maxFrame_m; }
	int getCurrentFrame() { return currentFrame_m; }
	void advanceFrame();
	void animate();
	void initializeStartScene();
	void initializeEndScene();
	float linear(float cFrame, float start, float change, float mFrame);
	float easeIn(float cFrame, float start, float change, float mFrame);
	float easeOut(float cFrame, float start, float change, float mFrame);
	float easeInOut(float cFrame, float start, float change, float mFrame);
};

#endif