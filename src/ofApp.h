#ifndef OFAPP_H
#define OFAPP_H

#include <fstream>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "Animator.h"
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"
#include "Ray.h"
#include "Renderer.h"
#include "SceneObject.h"

enum Mode
{
	COMPOSITION,
	ANIMATION,
	RENDERING,
};

// mousePressed + mouseReleased + mouseToDragPlane credits to
// Professor Kevin Smith CS116A SJSU
class ofApp : public ofBaseApp
{
private:
	Mode mode{ COMPOSITION };

	//--------------------------//
	//		  Camera			//
	//--------------------------//

	ofEasyCam mainCam;
	ofCamera sideCam;
	ofCamera previewCam;
	ofCamera *currentCam;

	//--------------------------//
	//		 Composition		//
	//--------------------------//

	ofxAssimpModelLoader modelLoader;

	// Scene
	//
	std::vector<SceneObject *> selected;
	std::vector<SceneObject *> scene;
	std::vector<SceneObject *> renderObjects;
	std::vector<Light *> lights;
	// AmbientLight tells renderer to apply minimum light to
	// every object rendered to prevent pitch black shadows.
	Light* ambientLight;

	// Function Variables
	//
	int nearestObj = -1;
	glm::vec3 lastPoint;
	int jointIndex = 0;
	bool dragMouse = false;
	bool bHoldX = false;
	bool bHoldY = false;
	bool bHoldZ = false;

	//--------------------------//
	//		  Animation 		//
	//--------------------------//

	Animator animator{ scene };
	bool playAnimation = false;

	//--------------------------//
	//		  Rendering			//
	//--------------------------//

	Renderer renderer{renderObjects, lights, ambientLight};

	//--------------------------//
	//			GUI				//
	//--------------------------//

	// Fonts
	//
	ofTrueTypeFont verdana50;
	ofTrueTypeFont verdana30;

	// Gui Parameters
	//
	ofParameterGroup parameters;
	ofParameter<float> radiusSlider;
	ofParameter<float> heightSlider;
	ofParameter<ofColor> colorSlider;
	ofxButton addSphere;
	ofxButton addCone;
	ofxButton addMesh;
	ofxButton addJoint;
	ofxButton addLight;

	ofxPanel gui;
	

public:
	void setup();
	void setupGui();
	void update();
	void draw();
	void drawGui(ofEventArgs &args);

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	// App Functions
	//
	bool mouseToDragPlane(int x, int y, glm::vec3 &point);
	bool objSelected() { return (selected.size() ? true : false); };
	void deleteSceneObj(SceneObject* selectedObj);
	void fileSaveSceneObject(SceneObject* selectedObj, std::string filename);
	void fileSaveSceneObjectRecurse(SceneObject* selectedObj, std::ofstream &outF);
	std::string getObjectData(SceneObject* selectedObj);
	void fileLoadSceneObject(std::string filename);
	void renderAnimation();

	// Gui Event Listener Functions
	//
	void addSpherePressed();
	void addConePressed();
	void addMeshPressed();
	void addJointPressed();
	void addLightPressed();

private:
	std::string compControls = 
		"Controls\n"
		"-----------\n"
		"~  - Lock Main Camera View\n"
		"F1 - Switch to Main Cam\n"
		"F2 - Switch to Side Cam\n"
		"F3 - Switch to Preview Cam\n"
		"F11- Fullscreen\n"
		"TAB- Change Modes\n"
		"D  - Delete Selected Object\n"
		"L  - Load JointFileSample.so\n"
		"O  - Print Object Local Position\n"
		"S  - Save Selected Object\n"
		"W  - Print Object World Position\n"
		"X  - (hold) Rotate Around X Axis\n"
		"Y  - (hold) Rotate Around Y Axis\n"
		"Z  - (hold) Rotate Around Z Axis\n";

	std::string animControls =
		"Controls\n"
		"-----------\n"
		"~  - Lock Main Camera View\n"
		"F1 - Switch to Main Cam\n"
		"F2 - Switch to Side Cam\n"
		"F3 - Switch to Preview Cam\n"
		"F11- Fullscreen\n"
		"TAB- Change Modes\n"
		"1  - Initialize Start Scene\n"
		"2  - Initialize End Scene\n"
		"O  - Print Object Local Position\n"
		"P  - Play Animation\n"
		"S  - Stop Animation\n"
		"W  - Print Object World Position\n";

	std::string rendControls =
		"Controls\n"
		"-----------\n"
		"~  - Lock Main Camera View\n"
		"F1 - Switch to Main Cam\n"
		"F2 - Switch to Side Cam\n"
		"F3 - Switch to Preview Cam\n"
		"F11- Fullscreen\n"
		"TAB- Change Modes\n"
		"A  - RayTrace Animation /animation/\n"
		"M  - RayMarch Scene imageM.png\n"
		"T  - RayTrace Scene imageT.png\n";
};

#endif