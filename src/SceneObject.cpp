#include "SceneObject.h"

// SceneObject Functions
//
glm::mat4 SceneObject::getRotateMatrix() const
{
	// yaw, pitch, roll 
	return (glm::eulerAngleYXZ(glm::radians(rotation_m.y), glm::radians(rotation_m.x), glm::radians(rotation_m.z)));
}

glm::mat4 SceneObject::getTranslateMatrix() const
{
	return (glm::translate(glm::mat4(1.0), glm::vec3(position_m.x, position_m.y, position_m.z)));
}

glm::mat4 SceneObject::getScaleMatrix() const
{
	return (glm::scale(glm::mat4(1.0), glm::vec3(scale_m.x, scale_m.y, scale_m.z)));
}

glm::mat4 SceneObject::getLocalMatrix() const
{
	// get the local transformations + pivot
	//
	glm::mat4 scale = getScaleMatrix();
	glm::mat4 rotate = getRotateMatrix();
	glm::mat4 trans = getTranslateMatrix();

	// handle pivot point  (rotate around a point that is not the object's center)
	//
	glm::mat4 pre = glm::translate(glm::mat4(1.0), glm::vec3(-pivotPoint_m.x, -pivotPoint_m.y, -pivotPoint_m.z));
	glm::mat4 post = glm::translate(glm::mat4(1.0), glm::vec3(pivotPoint_m.x, pivotPoint_m.y, pivotPoint_m.z));

	return (trans * post * rotate * pre * scale);
}

glm::mat4 SceneObject::getMatrix() const
{
	// if we have a parent (we are not the root),
	// concatenate parent's transform (this is recursive)
	// 
	if (parent_m) {
		glm::mat4 M = parent_m->getMatrix();
		return (M * getLocalMatrix());
	}
	else return getLocalMatrix();  // priority order is SRT
}

// Generate a rotation matrix that rotates v1 to v2
// v1, v2 must be normalized
glm::mat4 SceneObject::rotateToVector(glm::vec3 v1, glm::vec3 v2) const
{

	glm::vec3 axis = glm::cross(v1, v2);
	glm::quat q = glm::angleAxis(glm::angle(v1, v2), glm::normalize(axis));
	return glm::toMat4(q);
}

// Set position (pos is in world space)
void SceneObject::setWorldPosition(glm::vec3 pos) {
	if (parent_m)
		position_m = glm::inverse(parent_m->getMatrix()) * glm::vec4(pos, 1.0);
	else
		position_m = pos;
}

// Hierarchy 
void SceneObject::addChild(SceneObject *child)
{
	childList_m.push_back(child);
	child->parent_m = this;
}

// Fix object's rotation vector so that object's z axis aligns with pos
void SceneObject::fixRotationWith(const glm::vec3 pos)
{
	glm::vec3 distVector = getLocalPosition() - pos;
	float xzdis = glm::distance(glm::vec2(distVector.x, distVector.z), glm::vec2(0, 0));
	rotation_m.x = glm::degrees(-atan2f(distVector.y, xzdis));
	rotation_m.y = glm::degrees(-atan2f(-distVector.x, distVector.z));
	rotation_m.z = 0;
}

// SceneObject Destructor
SceneObject::~SceneObject()
{
	if (parent_m)
	{
		parent_m->childList_m.erase(std::remove(parent_m->childList_m.begin(), parent_m->childList_m.end(), this), parent_m->childList_m.end());
		for (int i = 0; i < childList_m.size(); i++) {
			glm::vec3 tempPos = childList_m[i]->getWorldPosition();
			parent_m->addChild(childList_m[i]);
			childList_m[i]->parent_m = parent_m;
			childList_m[i]->setWorldPosition(tempPos);
		}
	}
	else
	{
		for (int i = 0; i < childList_m.size(); i++) {
			glm::vec3 tempPos = childList_m[i]->getWorldPosition();
			childList_m[i]->parent_m = NULL;
			childList_m[i]->setWorldPosition(tempPos);
		}
	}
}

// Light Functions
//
bool Light::intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal)
{
	// transform Ray to object space.  
	//
	glm::mat4 mInv = glm::inverse(getMatrix());
	glm::vec4 p = mInv * glm::vec4(ray.getPosition().x, ray.getPosition().y, ray.getPosition().z, 1.0);
	glm::vec4 p1 = mInv * glm::vec4(ray.getPosition() + ray.getDirection(), 1.0);
	glm::vec3 d = glm::normalize(p1 - p);
	return (glm::intersectRaySphere(glm::vec3(p), d, glm::vec3(0, 0, 0), 0.1, point, normal));
}

// Plane Functions
//
bool Plane::intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal)
{
	float dist;
	bool hit = glm::intersectRayPlane(ray.getPosition(), ray.getDirection(), getWorldPosition(), normal_m, dist);
	if (hit) {
		point = ray.evalPoint(dist);
		normal = normal_m;
	}
	return hit;
}

float Plane::sdf(const glm::vec3 &p) {
	return p.y - getWorldPosition().y;
}

void Plane::draw()
{
	plane_m.setPosition(getWorldPosition());
	plane_m.setWidth(width_m);
	plane_m.setHeight(height_m);
	plane_m.setResolution(4, 4);
	plane_m.drawWireframe();
}

// ViewPlane Functions
//
glm::vec3 ViewPlane::toWorld(float u, float v)
{
	float w = getWidth();
	float h = getHeight();
	return (glm::vec3((u * w) + min_m.x, (v * h) + min_m.y, getWorldPosition().z));
}

// RenderCam Functions
//
Ray RenderCam::getRay(float u, float v)
{
	glm::vec3 pointOnPlane = view.toWorld(u, v);
	return(Ray(getWorldPosition(), glm::normalize(pointOnPlane - getWorldPosition())));
}

void RenderCam::draw()
{
	ofDrawBox(getWorldPosition(), 1.0);
	view.draw();
}

// Sphere Functions
//
bool Sphere::intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal)
{
	// transform Ray to object space.  
	glm::mat4 mInv = glm::inverse(getMatrix());
	glm::vec4 p = mInv * glm::vec4(ray.getPosition().x, ray.getPosition().y, ray.getPosition().z, 1.0);
	glm::vec4 p1 = mInv * glm::vec4(ray.getPosition() + ray.getDirection(), 1.0);
	glm::vec3 d = glm::normalize(p1 - p);
	
	return (glm::intersectRaySphere(glm::vec3(p), d, glm::vec3(0, 0, 0), radius_m, point, normal));
}

float Sphere::sdf(const glm::vec3 &p)
{
	return glm::length(p - getWorldPosition()) - radius_m;
}

void Sphere::draw() {
	// get the current transformation matrix for this object
	glm::mat4 m = getMatrix();

	// push the current stack matrix and multiply by this object's
	// matrix. now all vertices drawn will be transformed by this matrix
	ofPushMatrix();
	ofMultMatrix(m);
	ofDrawSphere(radius_m);
	ofPopMatrix();
}

// Cone Functions
//
bool Cone::intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal)
{
	bool hit = false;

	// transform Ray to object space.  
	//
	glm::mat4 mInv = glm::inverse(getMatrix());
	glm::vec4 p = mInv * glm::vec4(ray.getPosition().x, ray.getPosition().y, ray.getPosition().z, 1.0);
	glm::vec4 p1 = mInv * glm::vec4(ray.getPosition() + ray.getDirection(), 1.0);
	glm::vec3 d = glm::normalize(p1 - p);

	// Cone is pointing towards the negative Z axis.
	glm::vec3 axis = glm::vec3(0, 0, -1);

	glm::vec3 vertex_m = (height_m / 2) * axis;
	glm::vec3 oppVertex = (height_m / 2) * glm::vec3(0, 0, 1);
	
	float theta = glm::atan(radius_m / height_m);

	float cos2 = (glm::cos(theta) * glm::cos(theta));
	float DDotV = glm::dot(d, axis);
	glm::vec3 COVector = glm::vec3(p) - glm::vec3(vertex_m);
	float CODotV = glm::dot(COVector, axis);

	float a = (DDotV * DDotV) - cos2;
	float b = 2 * ((DDotV * CODotV) - (glm::dot(d, COVector) * cos2));
	float c = (CODotV * CODotV) - (glm::dot(COVector, COVector) * cos2);

	float determ = b * b - 4 * a * c;

	float t;
	if (determ < 0)
	{
		return false;
	}
	else if (determ == 0)
	{
		t = (-b) / (2 * a);
		hit = true;
	}
	else
	{
		float t1 = (-b - glm::sqrt(determ)) / (2 * a);
		float t2 = (-b + glm::sqrt(determ)) / (2 * a);
		t1 < t2 ? (t = t1) : (t = t2);
		hit = true;
	}

	if (hit)
	{
		point = Ray(p, d).evalPoint(t);
		if (point.z < vertex_m.z)
		{
			return false;
		}
		else if (point.z > oppVertex.z)
		{
			glm::vec3 rayPos = glm::vec3(p);
			glm::vec3 planeNorm = glm::vec3(0, 0, 1);
			hit = glm::intersectRayPlane(rayPos, d, oppVertex, planeNorm, t);
			if (hit)
			{
				point = Ray(p, d).evalPoint(t);
				if (glm::length(((point - oppVertex) * (point - oppVertex))) > (radius_m * radius_m))
					hit = false;
				else
					normal = planeNorm;
			}
		}
		else
		{
			float xNorm = point.x - vertex_m.x;
			float yNorm = point.y - vertex_m.y;
			float zNorm = glm::length(glm::vec2(xNorm, yNorm)) * glm::tan(theta);
			normal = glm::normalize(glm::vec3(xNorm, yNorm, zNorm));
		}
	}

	return hit;
}

void Cone::draw() {
	glm::mat4 m = getMatrix();

	ofPushMatrix();
	ofMultMatrix(m);
	ofRotate(90, 1, 0, 0);
	ofDrawCone(radius_m, height_m);
	ofPopMatrix();
}

// Mesh Functions
//
bool Mesh::intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal)
{
	glm::mat4 mInv = glm::inverse(getMatrix());
	glm::vec4 p = mInv * glm::vec4(ray.getPosition().x, ray.getPosition().y, ray.getPosition().z, 1.0);
	glm::vec4 p1 = mInv * glm::vec4(ray.getPosition() + ray.getDirection(), 1.0);
	glm::vec3 d = glm::normalize(p1 - p);

	int j = 0;
	bool hit = false;
	float nearestDist = FLT_MAX;
	glm::vec2 bary;
	for (ofMeshFace tri : mesh_m.getUniqueFaces())
	{
		float dist;
		if (glm::intersectRayTriangle(glm::vec3(p), d, tri.getVertex(0), tri.getVertex(1), tri.getVertex(2), bary, dist) &&  dist < nearestDist)
		{
			nearestDist = dist;
			hit = true;
			normal = tri.getFaceNormal();
			point = glm::vec3(p) + d * nearestDist;
		}
	}
	return hit;
}

void Mesh::draw()
{
	glm::mat4 m = getMatrix();

	ofPushMatrix();
	ofMultMatrix(m);
	mesh_m.drawWireframe();
	ofPopMatrix();
}

// Joint Functions
//
const float Joint::defaultRadius = 0.3;

void Joint::setWorldPosition(glm::vec3 pos)
{
	SceneObject::setWorldPosition(pos);
	if (parent_m)
	{
		adjustConnector();
	}
}

void Joint::setLocalPosition(glm::vec3 pos)
{
	SceneObject::setLocalPosition(pos);
	if (parent_m)
	{
		adjustConnector();
	}
}

void Joint::setLocalRotation(glm::vec3 rot)
{
	SceneObject::setLocalRotation(rot);
	//std::cout << "test\n";
	if (parent_m)
	{
		adjustConnector();
	}
}

void Joint::addChild(SceneObject *child) {
	SceneObject::addChild(child);
	Joint* jointChild = dynamic_cast<Joint*>(child);
	if (jointChild)
		jointChild->adjustConnector();
}

void Joint::adjustConnector()
{
	glm::vec3 connPos = glm::inverse(getMatrix()) * glm::vec4(getMidPoint(parent_m->getWorldPosition()), 1.0);
	conn_m.setWorldPosition(connPos);
	conn_m.fixRotationWith(glm::vec3(0, 0, 0));
	conn_m.setHeight(glm::length(getLocalPosition()) - 0.6);
}

bool Joint::intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal)
{
	glm::mat4 mInv = glm::inverse(getMatrix());
	glm::vec4 p = mInv * glm::vec4(ray.getPosition().x, ray.getPosition().y, ray.getPosition().z, 1.0);
	glm::vec4 p1 = mInv * glm::vec4(ray.getPosition() + ray.getDirection(), 1.0);
	glm::vec3 d = glm::normalize(p1 - p);
	
	glm::vec3 nodePoint;
	glm::vec3 nodeNormal;
	glm::vec3 connPoint;
	glm::vec3 connNormal;
	bool nodeHit = false;
	bool connHit = false;
	nodeHit = node_m.intersect(Ray(p, d), nodePoint, nodeNormal);
	if (parent_m)
	{
		connHit = conn_m.intersect(Ray(p, d), connPoint, connNormal);

		if (nodeHit && connHit)
		{
			if (glm::distance(glm::vec3(p), nodePoint) < glm::distance(glm::vec3(p), connPoint))
				connHit = false;
			else
				nodeHit = false;
		}

		if (nodeHit)
		{
			point = nodePoint;
			normal = nodeNormal;
			return true;
		}
		else if (connHit)
		{
			point = connPoint;
			normal = connNormal;
			return true;
		}
	}
	else
	{
		point = nodePoint;
		normal = nodeNormal;
	}
	return nodeHit;
}

void Joint::draw()
{
	glm::mat4 m = getMatrix();

	ofPushMatrix();
	ofMultMatrix(m);
	node_m.draw();
	if (parent_m)
		conn_m.draw();
	ofPopMatrix();
}