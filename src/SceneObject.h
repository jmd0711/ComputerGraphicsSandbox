#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "Ray.h"

// SceneObject Matrix + Hierarchy Functions 
// & SceneObject Member Variables credits to
// Professor Kevin Smith CS116A SJSU
class SceneObject
{
private:
	glm::vec3 position_m;						// Default positions:	0, 0, 0
	glm::vec3 rotation_m = glm::vec3(0, 0, 0);  // Rotation
	glm::vec3 scale_m = glm::vec3(1, 1, 1);		// Scale
	glm::vec3 pivotPoint_m = glm::vec3(0, 0, 0);
	ofColor diffuseColor_m;						// Default color:		gray
	ofColor specularColor_m;					// Default color:		light gray
	std::string name_h = "SceneObject";

protected:
	bool isSelectable_m = true;
	bool hasSDF_m = false;

	SceneObject *parent_m = NULL;
	std::vector<SceneObject *> childList_m;
	
public:
	SceneObject(glm::vec3 pos = glm::vec3(0, 0, 0), ofColor diffuse = ofColor::gray, ofColor specular = ofColor::lightGray) : 
		position_m{ pos }, 
		diffuseColor_m{ diffuse }, 
		specularColor_m{ specular }
	{
	}
	SceneObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca, ofColor diffuse = ofColor::gray, ofColor specular = ofColor::lightGray) : 
		position_m{ pos }, 
		rotation_m{ rot },
		scale_m{ sca },
		diffuseColor_m{ diffuse }, 
		specularColor_m{ specular }
	{
	}

	glm::mat4 getRotateMatrix() const;
	glm::mat4 getTranslateMatrix() const;
	glm::mat4 getScaleMatrix() const;
	glm::mat4 getLocalMatrix() const;
	glm::mat4 getMatrix() const;
	glm::mat4 rotateToVector(glm::vec3 v1, glm::vec3 v2) const;

	glm::vec3 getWorldPosition() const { return (getMatrix() * glm::vec4(0.0, 0.0, 0.0, 1.0)); }
	glm::vec3 getLocalPosition() const { return position_m; }
	glm::vec3 getLocalRotation() { return rotation_m; }
	std::string getName() const { return name_h; }
	ofColor getDiffuse() const { return diffuseColor_m; }
	ofColor getSpecular() const { return specularColor_m; }
	std::string getParentName() const { return (parent_m ? parent_m->getName() : "NULL"); }
	std::vector<SceneObject *> getChildList() const { return childList_m; }
	bool hasParent() const { return parent_m; }
	bool selectable() const { return isSelectable_m; }
	bool hasSDF() const { return hasSDF_m; }

	virtual void setWorldPosition(glm::vec3 pos);
	virtual void setLocalPosition(glm::vec3 pos) { position_m = pos; }
	virtual void setLocalRotation(glm::vec3 rot) { rotation_m = rot; }
	virtual void setName(std::string name) { name_h = name; }

	virtual void addChild(SceneObject *child);
	virtual void fixRotationWith(const glm::vec3 pos);

	virtual bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal) { cout << "SceneObject::intersect\n"; return false; }
	virtual float sdf(const glm::vec3 &p) { return FLT_MAX; }
	virtual void draw() = 0;

	virtual ~SceneObject();
};

// Light Class credits to
// Professor Kevin Smith CS116A SJSU
class Light : public SceneObject
{
private:
	float intensity_m;

public:
	Light(glm::vec3 pos, float i = 0) : SceneObject{ pos, ofColor::yellow }, intensity_m{ i }
	{
		setName("Light");
	}
	float getIntensity() { return intensity_m; }

	virtual bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal);
	virtual void draw() { ofDrawSphere(getWorldPosition(), 0.1); }
};

// Plane Class credits to
// Professor Kevin Smith CS116A SJSU
class Plane : public SceneObject
{
private:
	ofPlanePrimitive plane_m;
	glm::vec3 normal_m;
	float width_m;
	float height_m;

public:
	Plane(glm::vec3 pos, glm::vec3 normal = glm::vec3(0, 1, 0), ofColor diffuse = ofColor::darkOliveGreen, float w = 20, float h = 20)
		: SceneObject{ pos, diffuse },
		normal_m{ normal },
		width_m{ w },
		height_m{ h }
	{
		plane_m.rotateDeg(90, 1, 0, 0);
		isSelectable_m = false;
		hasSDF_m = true;
	}
	Plane() {}

	virtual bool intersect(const Ray &ray, glm::vec3 & point, glm::vec3 & normal);
	virtual float sdf(const glm::vec3 &p);
	virtual void draw();

};

// ViewPlane Class credits to
// Professor Kevin Smith CS116A SJSU
class  ViewPlane : public Plane
{
private:
	glm::vec2 min_m;
	glm::vec2 max_m;

public:
	ViewPlane(glm::vec2 p0, glm::vec2 p1) : min_m{ p0 }, max_m{ p1 }
	{
	}

	// TODO Values not final
	ViewPlane() : Plane{ glm::vec3(0, 0, 5), glm::vec3(0, 0, 1) }, min_m{ glm::vec2(-3, -2) }, max_m{ glm::vec2(3, 2) }
	{
	}

	glm::vec3 toWorld(float u, float v);   //   (u, v) --> (x, y, z) [ world space ]
	void draw() { ofDrawRectangle(glm::vec3(min_m.x, min_m.y, getWorldPosition().z), getWidth(), getHeight()); }

	void setSize(glm::vec2 min, glm::vec2 max) { min_m = min; max_m = max; }
	float getAspect() const { return getWidth() / getHeight(); }
	float getWidth() const { return max_m.x - min_m.x; }
	float getHeight() const { return (max_m.y - min_m.y); }

	glm::vec2 topLeft() const { return glm::vec2(min_m.x, max_m.y); }
	glm::vec2 topRight() const { return max_m; }
	glm::vec2 bottomLeft() const { return min_m; }
	glm::vec2 bottomRight() const { return glm::vec2(max_m.x, min_m.y); }

	//  To define an infinite plane, we just need a point and normal.
	//  The ViewPlane is a finite plane so we need to define the boundaries.
	//  We will define this in terms of min, max  in 2D.  
	//  (in local 2D space of the plane)
	//  ultimately, will want to locate the ViewPlane with RenderCam anywhere
	//  in the scene, so it is easier to define the View rectangle in a local'
	//  coordinate system.
	//
};

// Render Class credits to
// Professor Kevin Smith CS116A SJSU
class RenderCam : public SceneObject
{
private:
	glm::vec3 aim;
	ViewPlane view;

public:
	RenderCam() : SceneObject{ glm::vec3(0, 0, 10) }, aim{ glm::vec3(0, 0, -1) }
	{
		isSelectable_m = false;
	}
	void draw();

	Ray getRay(float u, float v);
	ViewPlane& getView() { return view; }
};

class Sphere : public SceneObject 
{
private:
	float radius_m = 1.0;

public:
	Sphere(glm::vec3 pos, float radius, ofColor diffuse = ofColor::lightGray) : SceneObject{ pos, diffuse }, radius_m{ radius }
	{
		setName("Sphere");
		hasSDF_m = true;
	}

	float getRadius() { return radius_m; }
	void setRadius(float rad) { radius_m = rad; }

	virtual bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal);
	virtual float sdf(const glm::vec3 &p);
	virtual void draw();

};

class Cone : public SceneObject {
private:
	float radius_m = 1.0;
	float height_m = 2.0;

public:
	Cone(glm::vec3 pos, float rad, float h, ofColor color = ofColor::gray) :
		SceneObject{ pos, color },
		radius_m{ rad },
		height_m{ h }
	{
	}

	void setRadius(float r) { radius_m = r; }
	void setHeight(float h) { height_m = h; }

	virtual bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal);
	virtual void draw();
};

class Mesh : public SceneObject {
private:
	ofMesh mesh_m;

public:
	Mesh(glm::vec3 pos, ofMesh mesh, ofColor diffuse = ofColor::gray) : SceneObject{ pos, diffuse }, mesh_m{ mesh }
	{
	}
	virtual bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal);
	virtual void draw();
};

class Joint : public SceneObject
{
private:
	static const float defaultRadius;
	Sphere node_m;
	Cone conn_m;
public:
	
	Joint(std::string name = "Joint") : 
		node_m{ glm::vec3(0, 0, 0), defaultRadius }, 
		// conn_m does not matter here because first joint
		// does not have a parent to connect to
		conn_m{ glm::vec3(0, 0, 0), defaultRadius, 0.1 }
	{
		setName(name);
	}

	Joint(glm::vec3 pos, std::string name = "Joint") : 
		SceneObject{ pos }, 
		node_m{ glm::vec3(0, 0, 0), 0.3 },
		// conn_m does not matter here because first joint
		// does not have a parent to connect to
		conn_m{ glm::vec3(0, 0, 0), 0.3, 0.1 }
	{
		setName(name);
	}

	Joint(glm::vec3 pos, glm::vec3 rot, glm::vec3 sc, std::string name = "Joint") :
		SceneObject{ pos, rot, sc },
		node_m{ glm::vec3(0, 0, 0), 0.3 },
		// conn_m does not matter here because first joint
		// does not have a parent to connect to
		conn_m{ glm::vec3(0, 0, 0), 0.3, 0.1 }
	{
		setName(name);
	}

	Joint(SceneObject *parent, std::string name = "Joint") :
		node_m{ glm::vec3(0, 0, 0), 0.3 },
		conn_m{ glm::vec3(0, 0, 0), 0.3, 0.1 }
	{
		setWorldPosition(glm::vec3(0, 3, 0));
		parent->addChild(this);
		adjustConnector();
		setName(name);
	}

	virtual void setWorldPosition(glm::vec3 pos);
	virtual void setLocalPosition(glm::vec3 pos);
	virtual void setLocalRotation(glm::vec3 rot);
	virtual void addChild(SceneObject *child);

	void adjustConnector();
		
	bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal);
	void draw();
	
private:
	glm::vec3 getMidPoint(glm::vec3 otherPos) { return (getWorldPosition() + otherPos) / 2; }
};

#endif