#include "Renderer.h"

const float Renderer::DIST_THRESHOLD = 0.1f;
const float Renderer::MAX_DISTANCE = 10.0f;

void Renderer::render(std::string filename, Renderer::RenderMethod rend) {
	std::cout << "Saving Image to " << filename << "...\n";
	image_m.allocate(imageWidth, imageHeight, OF_IMAGE_COLOR);
	for (int w = 0; w < imageWidth; w++) {
		for (int h = 0; h < imageHeight; h++) {
			float u = (w + 0.5) / imageWidth;
			float v = (h + 0.5) / imageHeight;

			Ray ray = renderCam_m.getRay(u, v);

			glm::vec3 nearestPoint;
			glm::vec3 nearestNorm;
			nearestObj_m = -1;
			bool hit;

			switch (rend)
			{
			// Ray Trace Algorithm
			case Renderer::RenderMethod::RAY_TRACE:
				hit = rayTraceHit(ray, nearestPoint, nearestNorm);
				break;

			// Ray March Algorithm
			case Renderer::RenderMethod::RAY_MARCH:
				hit = rayMarchHit(ray, nearestPoint, nearestNorm);
				break;
			}

			if (!hit)
				image_m.setColor(w, imageHeight - h - 1, ofColor::black);
			else {
				ofColor color = phong(nearestPoint, nearestNorm, scene_m[nearestObj_m]->getDiffuse(), scene_m[nearestObj_m]->getSpecular(), 10.0, rend);
				image_m.setColor(w, imageHeight - h - 1, color);
				//image_m.setColor(w, imageHeight - h - 1, ofColor::white);
			}
		}
	}
	std::cout << "Image Saved.\n";
	image_m.save(filename);
	
}

bool Renderer::inShadow(Ray pointToLight, glm::vec3 lightPos) {
	glm::vec3 intersectPoint, normal;
	//float bias = 0.001;
	for (int i = 0; i < scene_m.size(); i++) {
		if (nearestObj_m != i
			&& scene_m[i]->intersect(pointToLight, intersectPoint, normal) 
			&& !(glm::length(intersectPoint - pointToLight.getPosition()) > glm::length(lightPos - pointToLight.getPosition())))
		{
			return true;
		}
	}
	return false;
}

ofColor Renderer::phong(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power, Renderer::RenderMethod rend) {
	ofColor color = /*ambientLight_m->getDiffuse()*/diffuse * ambientLight_m->getIntensity();
	ofColor lambert, phong;
	float shadowBias = 0.1;
	for (int i = 0; i < lights_m.size(); i++) {
		glm::vec3 n = glm::normalize(norm);
		glm::vec3 l = glm::normalize(lights_m[i]->getWorldPosition() - p);
		glm::vec3 v = glm::normalize(renderCam_m.getWorldPosition() - p);
		glm::vec3 h = glm::normalize(v + l);

		glm::vec3 nearestPoint;
		glm::vec3 nearestNormal;
		bool shadow;

		switch (rend)
		{
		case Renderer::RenderMethod::RAY_TRACE:
			shadow = inShadow(Ray(p + (n * shadowBias), l), lights_m[i]->getWorldPosition());
			break;

		case Renderer::RenderMethod::RAY_MARCH:
			shadow = ((rayMarchHit(Ray(p + (n * shadowBias), l), nearestPoint, nearestNormal)) /*|| glm::length(nearestPoint - p) > glm::length(lights_m[i]->getPosition() - p)*/);
			break;
		}
		if (!shadow) {
			lambert = diffuse * lights_m[i]->getIntensity() * glm::max(0.0f, glm::dot(n, l));
			phong = specular * lights_m[i]->getIntensity() * glm::max(0.0f, glm::pow(glm::dot(n, h), power));
			color += lambert + phong;
		}
	}

	return color;
}

float Renderer::sceneSDF(const glm::vec3 &p) {
	float closestDistance = FLT_MAX;
	for (int i = 0; i < scene_m.size(); i++) {
		if (scene_m[i]->hasSDF()) {
			float d = scene_m[i]->sdf(p);
			if (d < closestDistance) {
				closestDistance = d;
				nearestObj_m = i;
			}
		}
	}
	return closestDistance;
}

bool Renderer::rayTraceHit(Ray r, glm::vec3 &nearestPoint, glm::vec3 &nearestNormal)
{
	glm::vec3 intersectPoint;
	glm::vec3 normal;
	bool hit = false;
	float nearestDistance = std::numeric_limits<float>::max();

	//ofColor setColor = ofColor::black;
	for (int i = 0; i < scene_m.size(); i++)
	{
		if (scene_m[i]->intersect(r, intersectPoint, normal))
		{
			float dist = glm::length(intersectPoint - renderCam_m.getWorldPosition());
			if (nearestDistance > dist) {
				nearestDistance = dist;
				nearestObj_m = i;
				nearestPoint = intersectPoint;
				nearestNormal = normal;

			}
			hit = true;
		}
	}
	return hit;
}


bool Renderer::rayMarchHit(Ray r, glm::vec3 &nearestPoint, glm::vec3 &nearestNormal) {
	bool hit = false;
	nearestPoint = r.getPosition();
	for (int i = 0; i < MAX_RAY_STEPS; i++) {
		float dist = sceneSDF(nearestPoint);
		if (dist < DIST_THRESHOLD) {
			hit = true;

			break;
		}
		else if (dist > MAX_DISTANCE)
			break;
		else nearestPoint += r.getDirection() * dist;
	}
	nearestNormal = getNormalRM(nearestPoint);
	return hit;
}

glm::vec3 Renderer::getNormalRM(const glm::vec3 &nearestPoint) {
	float eps = 0.01;
	glm::vec3 n(sceneSDF(glm::vec3(nearestPoint.x + eps, nearestPoint.y, nearestPoint.z)) - sceneSDF(glm::vec3(nearestPoint.x - eps, nearestPoint.y, nearestPoint.z)), sceneSDF(glm::vec3(nearestPoint.x, nearestPoint.y + eps, nearestPoint.z)) - sceneSDF(glm::vec3(nearestPoint.x, nearestPoint.y - eps, nearestPoint.z)), sceneSDF(glm::vec3(nearestPoint.x, nearestPoint.y, nearestPoint.z + eps)) - sceneSDF(glm::vec3(nearestPoint.x, nearestPoint.y, nearestPoint.z - eps)));
	return glm::normalize(n);
}