#include "Animator.h"

void Animator::play()
{
	if (framesStart_m.size() != framesEnd_m.size())
	{
		std::cout << "Cannot animate, Start and End scenes have different scene objects.\n"
			<< "Clearing KeyFrames...\n";
		framesStart_m.clear();
		framesEnd_m.clear();
		startSet_m = false;
		endSet_m = false;
		std::cout << "KeyFrames cleared. Please set new Start and End scenes\n";
		return;
	}
	else
	{
		std::cout << "Playing keyframe...\n";
		currentFrame_m = minFrame_m;
		play_m = true;
	}
}

void Animator::advanceFrame()
{
	if (play_m && !scene_m.empty() && startSet_m && endSet_m) {
		animate();
		if (currentFrame_m < maxFrame_m) {
			currentFrame_m++;
		}
		else currentFrame_m = minFrame_m;
	}
}

void Animator::animate()
{
	if (play_m && !scene_m.empty() && startSet_m && endSet_m)
	{
		for (int i = 0; i < scene_m.size(); i++)
		{
			if (currentFrame_m <= framesEnd_m[i]->cFrame_m) {
				glm::vec3 position;
				glm::vec3 rotation;
				position.x = linear(currentFrame_m, framesStart_m[i]->translate_m.x, framesEnd_m[i]->translate_m.x - framesStart_m[i]->translate_m.x, framesEnd_m[i]->cFrame_m);
				position.y = linear(currentFrame_m, framesStart_m[i]->translate_m.y, framesEnd_m[i]->translate_m.y - framesStart_m[i]->translate_m.y, framesEnd_m[i]->cFrame_m);
				position.z = linear(currentFrame_m, framesStart_m[i]->translate_m.z, framesEnd_m[i]->translate_m.z - framesStart_m[i]->translate_m.z, framesEnd_m[i]->cFrame_m);
				rotation.x = linear(currentFrame_m, framesStart_m[i]->rotate_m.x, framesEnd_m[i]->rotate_m.x - framesStart_m[i]->rotate_m.x, framesEnd_m[i]->cFrame_m);
				rotation.y = linear(currentFrame_m, framesStart_m[i]->rotate_m.y, framesEnd_m[i]->rotate_m.y - framesStart_m[i]->rotate_m.y, framesEnd_m[i]->cFrame_m);
				rotation.z = linear(currentFrame_m, framesStart_m[i]->rotate_m.z, framesEnd_m[i]->rotate_m.z - framesStart_m[i]->rotate_m.z, framesEnd_m[i]->cFrame_m);

				scene_m[i]->setLocalPosition(position);
				scene_m[i]->setLocalRotation(position);
			}
		}
	}
}

void Animator::initializeStartScene()
{
	framesStart_m.clear();
	for (SceneObject* sceneObj : scene_m)
	{
		framesStart_m.push_back(new KeyFrame{ sceneObj->getLocalPosition(), sceneObj->getLocalRotation(), minFrame_m });
	}
	startSet_m = true;
	endSet_m = false;
	std::cout << "Start scene set.\n";
}

void Animator::initializeEndScene()
{
	framesEnd_m.clear();
	for (SceneObject* sceneObj : scene_m)
	{
		framesEnd_m.push_back(new KeyFrame{ sceneObj->getLocalPosition(), sceneObj->getLocalRotation(), maxFrame_m });
	}
	endSet_m = true;
	std::cout << "End scene set.\n";
}

float Animator::linear(float cFrame, float start, float change, float mFrame)
{
	return change * cFrame / mFrame + start;
}

float Animator::easeIn(float cFrame, float start, float change, float mFrame)
{
	cFrame /= mFrame;
	return change * cFrame * cFrame + start;
}

float Animator::easeOut(float cFrame, float start, float change, float mFrame)
{
	cFrame /= mFrame;
	return -change * cFrame * (cFrame - 2) + start;
}

float Animator::easeInOut(float cFrame, float start, float change, float mFrame)
{
	cFrame /= mFrame / 2;
	if (cFrame < 1) return change / 2 * cFrame * cFrame + start;
	cFrame--;
	return (-change) / 2 * (cFrame * (cFrame - 2) - 1) + start;
}