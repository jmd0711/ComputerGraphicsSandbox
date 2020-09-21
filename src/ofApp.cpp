#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetBackgroundColor(ofColor::black);

	// Cameras Setup
	//
	mainCam.setDistance(30);
	currentCam = &mainCam;
	sideCam.setPosition(175, 0, 0);
	sideCam.lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	previewCam.setPosition(0, 0, 10);
	previewCam.lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	
	// Initial Lights Setup
	//
	ambientLight = new Light(glm::vec3(0, 0, 0), 0.23);
	lights.push_back(new Light(glm::vec3(0, 4, 4), 0.8));
	for (int i = 0; i < lights.size(); i++) {
		scene.push_back(lights[i]);
	}

	// Initial Objects Setup
	//
	// Plane is an immovable SceneObject that is rendered
	// throughout the whole scene.
	renderObjects.push_back(new Plane(glm::vec3(0, -2, 0), glm::vec3(0, 1, 0)));
	for (int i = 0; i < renderObjects.size(); i++) {
		scene.push_back(renderObjects[i]);
	}

	// Model Loader
	//
	// Loads meshes from .obj files. Currently only
	// loads Utah Teapot.
	// TODO: Give users ability to load different model files.
	modelLoader.loadModel("teapot.obj");
	modelLoader.setRotation(0, 180, 1, 0, 0);
	modelLoader.setScale(0.03, 0.03, 0.03);
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
	ofSetBackgroundColor(35, 35, 35);

	// Fonts Setup
	//
	ofTrueTypeFont::setGlobalDpi(72);
	verdana50.load("verdana.ttf", 50, true, true);
	verdana50.setLineHeight(34.0f);
	verdana50.setLetterSpacing(1.035);

	verdana30.load("verdana.ttf", 30, true, true);
	verdana30.setLineHeight(34.0f);
	verdana30.setLetterSpacing(1.035);

	// Button Event Listeners
	//
	addSphere.addListener(this, &ofApp::addSpherePressed);
	addCone.addListener(this, &ofApp::addConePressed);
	addMesh.addListener(this, &ofApp::addMeshPressed);
	addJoint.addListener(this, &ofApp::addJointPressed);
	addLight.addListener(this, &ofApp::addLightPressed);

	// GUI Setup
	//
	ofxPanel::loadFont("verdana.ttf", 20);
	ofxPanel::setDefaultWidth(500);
	ofxPanel::setDefaultHeight(35);
	parameters.setName("parameters");
	parameters.add(radiusSlider.set("radius", 1, 1, 5));
	parameters.add(heightSlider.set("height", 1, 1, 10));
	parameters.add(colorSlider.set("color", 100, ofColor(0, 0), 255));
	gui.setup(parameters);
	gui.add(addSphere.setup("add sphere"));
	gui.add(addCone.setup("add cone"));
	gui.add(addMesh.setup("add mesh"));
	gui.add(addJoint.setup("add joint"));
	gui.add(addLight.setup("add light"));
	gui.setPosition(50, 700);
}

//--------------------------------------------------------------
void ofApp::update()
{
	if (playAnimation)
		animator.advanceFrame();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	currentCam->begin();
	for (int i = 0; i < scene.size(); i++) {
		if (objSelected() && scene[i] == selected[0])
			ofSetColor(ofColor::white);
		else ofSetColor(scene[i]->getDiffuse());
		ofNoFill();
		scene[i]->draw();
	}

	ofSetColor(ofColor::blue);
	renderer.draw();
	mainCam.draw();
	currentCam->end();
}

//--------------------------------------------------------------
void ofApp::drawGui(ofEventArgs &args)
{
	switch (mode)
	{
	case COMPOSITION:
		verdana50.drawString("Composition", 30, 60);
		verdana30.drawString(compControls, 30, 110);
		gui.draw();
		break;
	case ANIMATION:
		verdana50.drawString("Animation", 30, 60);
		verdana30.drawString(animControls, 30, 110);
		break;
	case RENDERING:
		verdana50.drawString("Rendering", 30, 60);
		verdana30.drawString(rendControls, 30, 110);
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (mode == COMPOSITION)
	{
		switch (key)
		{
		case 'X':
		case 'x':
			bHoldX = true;
			break;
		case 'Y':
		case 'y':
			bHoldY = true;
			break;
		case 'Z':
		case 'z':
			bHoldZ = true;
			break;
		}
	}
	else if (mode == ANIMATION)
	{
		switch (key)
		{
		case 'X':
		case 'x':
			bHoldX = true;
			break;
		case 'Y':
		case 'y':
			bHoldY = true;
			break;
		case 'Z':
		case 'z':
			bHoldZ = true;
			break;
		}
	}
	else if (mode == RENDERING)
	{
		switch (key)
		{
		case 'X':
		case 'x':
			bHoldX = true;
			break;
		case 'Y':
		case 'y':
			bHoldY = true;
			break;
		case 'Z':
		case 'z':
			bHoldZ = true;
			break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
	if (mode == COMPOSITION)
	{
		switch (key)
		{
		case 'D':
		case 'd':
			if (objSelected())
				deleteSceneObj(selected[0]);
			break;
		case 'L':
		case 'l':
			fileLoadSceneObject("JointFileSample.so");
			break;
		case 'O':
		case 'o':
			std::cout << selected[0]->getLocalPosition() << '\n';
			break;
		case 'S':
		case 's':
			if (objSelected())
				fileSaveSceneObject(selected[0], "SceneObjectFile.so");
			break;
		case 'W':
		case 'w':
			std::cout << selected[0]->getWorldPosition() << '\n';
			break;
		case 'X':
		case 'x':
			bHoldX = false;
			break;
		case 'Y':
		case 'y':
			bHoldY = false;
			break;
		case 'Z':
		case 'z':
			bHoldZ = false;
			break;
		case '~':
		case '`':
			if (mainCam.getMouseInputEnabled()) mainCam.disableMouseInput();
			else mainCam.enableMouseInput();
			break;
		case OF_KEY_F1:
			currentCam = &mainCam;
			break;
		case OF_KEY_F2:
			currentCam = &sideCam;
			break;
		case OF_KEY_F3:
			currentCam = &previewCam;
			break;
		case OF_KEY_F11:
			ofToggleFullscreen();
			break;
		case OF_KEY_TAB:
			mode = ANIMATION;
			break;
		}
	}
	else if (mode == ANIMATION)
	{
		switch (key)
		{
		case 'O':
		case 'o':
			std::cout << selected[0]->getLocalPosition() << '\n';
			break;
		case 'P':
		case 'p':
			animator.play();
			playAnimation = true;
			break;
		case 'S':
		case 's':
			animator.stop();
			playAnimation = false;
			break;
		case 'W':
		case 'w':
			std::cout << selected[0]->getWorldPosition() << '\n';
			break;
		case 'X':
		case 'x':
			bHoldX = false;
			break;
		case 'Y':
		case 'y':
			bHoldY = false;
			break;
		case 'Z':
		case 'z':
			bHoldZ = false;
			break;
		case '1':
			animator.initializeStartScene();
			break;
		case '2':
			animator.initializeEndScene();
			break;
		case '~':
		case '`':
			if (mainCam.getMouseInputEnabled()) mainCam.disableMouseInput();
			else mainCam.enableMouseInput();
			break;
		case OF_KEY_F1:
			currentCam = &mainCam;
			break;
		case OF_KEY_F2:
			currentCam = &sideCam;
			break;
		case OF_KEY_F3:
			currentCam = &previewCam;
			break;
		case OF_KEY_F11:
			ofToggleFullscreen();
			break;
		case OF_KEY_TAB:
			mode = RENDERING;
			break;
		}
	}
	else if (mode == RENDERING)
	{
		switch (key)
		{
		case 'A':
		case 'a':
			renderAnimation();
			break;
		case 'M':
		case 'm':
			renderer.render("imageM.png", Renderer::RenderMethod::RAY_MARCH);
			break;
		case 'T':
		case 't':
			renderer.render("imageT.png", Renderer::RenderMethod::RAY_TRACE);
			break;
		case 'X':
		case 'x':
			bHoldX = false;
			break;
		case 'Y':
		case 'y':
			bHoldY = false;
			break;
		case 'Z':
		case 'z':
			bHoldZ = false;
			break;
		case '~':
		case '`':
			if (mainCam.getMouseInputEnabled()) mainCam.disableMouseInput();
			else mainCam.enableMouseInput();
			break;
		case OF_KEY_F1:
			currentCam = &mainCam;
			break;
		case OF_KEY_F2:
			currentCam = &sideCam;
			break;
		case OF_KEY_F3:
			currentCam = &previewCam;
			break;
		case OF_KEY_F11:
			ofToggleFullscreen();
			break;
		case OF_KEY_TAB:
			mode = COMPOSITION;
			break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	if (objSelected() && dragMouse) {
		glm::vec3 point;
		mouseToDragPlane(x, y, point);
		if (bHoldX)
		{
			// Rotate selected object around local X axis.
			glm::vec3 rot = selected[0]->getLocalRotation();
			selected[0]->setLocalRotation(rot + glm::vec3((point.x - lastPoint.x) * 20.0, 0, 0));
		}
		else if (bHoldY)
		{
			// Rotate selected object around local Y axis.
			glm::vec3 rot = selected[0]->getLocalRotation();
			selected[0]->setLocalRotation(rot + glm::vec3(0, (point.x - lastPoint.x) * 20.0, 0));
		}
		else if (bHoldZ)
		{
			// Rotate selected object around local Z axis.
			glm::vec3 rot = selected[0]->getLocalRotation();
			selected[0]->setLocalRotation(rot + glm::vec3(0, 0, (point.x - lastPoint.x) * 20.0));
		}
		else
		{
			// Move selected object around plane parallel
			// to camera's view plane.
			selected[0]->setWorldPosition(point);
		}
		lastPoint = point;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	// if we are moving the camera around, don't allow selection
	//
	if (mainCam.getMouseInputEnabled()) return;

	// clear selection list
	//
	selected.clear();

	//
	// test if something selected
	//
	vector<SceneObject *> hits;

	glm::vec3 p = currentCam->screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 d = p - currentCam->getPosition();
	glm::vec3 dn = glm::normalize(d);

	// check for selection of scene objects
	//
	for (int i = 0; i < scene.size(); i++) {

		glm::vec3 point, norm;

		//  We hit an object
		//
		if (scene[i]->selectable() && scene[i]->intersect(Ray(p, dn), point, norm)) {
			hits.push_back(scene[i]);
		}
	}


	// if we selected more than one, pick nearest
	//
	SceneObject *selectedObj = NULL;
	if (hits.size() > 0) {
		selectedObj = hits[0];
		float nearestDist = std::numeric_limits<float>::infinity();
		for (int n = 0; n < hits.size(); n++) {
			float dist = glm::length(hits[n]->getWorldPosition() - currentCam->getPosition());
			if (dist < nearestDist) {
				nearestDist = dist;
				selectedObj = hits[n];
			}
		}
	}
	if (selectedObj) {
		selected.push_back(selectedObj);
		dragMouse = true;
		mouseToDragPlane(x, y, lastPoint);
	}
	else {
		selected.clear();
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	dragMouse = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{ 

}

//-------------------------//
//	ofApp Extra Functions  //
//-------------------------//

bool ofApp::mouseToDragPlane(int x, int y, glm::vec3 &point)
{
	// Point is the world space position of the mouse in the
	// plane of the selected object parallel to camera view.
	// If no selected object, then plane of origin paralelel
	// to camera view.
	glm::vec3 p = currentCam->screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 d = p - currentCam->getPosition();
	glm::vec3 dn = glm::normalize(d);

	float dist;
	glm::vec3 pos;
	if (objSelected()) {
		pos = selected[0]->getWorldPosition();
	}
	else pos = glm::vec3(0, 0, 0);

	if (glm::intersectRayPlane(p, dn, pos, glm::normalize(currentCam->getZAxis()), dist)) {
		point = p + dn * dist;
		return true;
	}
	return false;
}

// Delete Selected SceneObject
//
void ofApp::deleteSceneObj(SceneObject* selectedObj)
{
	scene.erase(std::remove(scene.begin(), scene.end(), selectedObj), scene.end());
	Light* lightObj = dynamic_cast<Light*>(selectedObj);
	if (lightObj)
	{
		lights.erase(std::remove(lights.begin(), lights.end(), lightObj), lights.end());
	}
	else
	{
		renderObjects.erase(std::remove(renderObjects.begin(), renderObjects.end(), selectedObj), renderObjects.end());
	}
	selected.clear();
	delete selectedObj;
}

// Save Selected SceneObject and each descendants to given filename
//
void ofApp::fileSaveSceneObject(SceneObject* selectedObj, std::string filename)
{
	std::cout << "Saving " << selectedObj->getName() << " to file " << filename << "...\n";
	std::ofstream outF{ "data/" + filename, std::ios::trunc };
	if (!outF)
		std::cerr << filename << " could not be opened for writing!\n";

	fileSaveSceneObjectRecurse(selectedObj, outF);
		
	std::cout << filename << " saved.\n";
	outF.close();
}

void ofApp::fileSaveSceneObjectRecurse(SceneObject* selectedObj, std::ofstream &outF)
{
	outF << getObjectData(selectedObj) << '\n';
	if (!selectedObj->getChildList().empty())
	{
		for (SceneObject* child : selectedObj->getChildList())
			fileSaveSceneObjectRecurse(child, outF);
	}
}

std::string ofApp::getObjectData(SceneObject* selectedObj)
{
	std::string output = "create -joint " + selectedObj->getName()
		+ " -rotate <" + glm::to_string(selectedObj->getLocalRotation()) //<%f, %f, %f>"
		+ "> -translate <" + glm::to_string(selectedObj->getLocalPosition()) //<%f, %f, %f>"
		+ "> -parent " + selectedObj->getParentName() + ';';
	return output;
}

// Load SceneObject from filename
//
void ofApp::fileLoadSceneObject(std::string filename)
{
	std::cout << "Loading " << filename << "...\n";
	std::ifstream inF{ "data/" + filename };
	if (!inF)
		std::cerr << filename << " could not be opened for reading!\n";

	std::vector<SceneObject *> objs;
	std::string line;
	while (std::getline(inF, line))
	{
		// Erase spaces
		line.erase(remove_if(line.begin(), line.end(), isspace), line.end());

		// -jointName-rotate
		// x         y
		// nameLen =  y - x - len("-joint")
		int nameLen = line.find("-rotate") - line.find("-joint") - 6;
		string name = line.substr(line.find("-joint") + 6, nameLen);

		// -parentParentName;
		// x                y
		// parentNameLen =  y - x - len("-parent")
		int parentNameLen = line.find(";") - line.find("-parent") - 7;
		string parentName = line.substr(line.find("-parent") + 7, parentNameLen);

		int rotLen = line.find(")") - line.find("(") - 1;
		string rot = line.substr(line.find("(") + 1, rotLen);

		string nLine = line.substr(line.find(")") + 1);
		int traLen = nLine.find(")") - nLine.find("(") - 1;
		string tra = nLine.substr(nLine.find("(") + 1, traLen);

		replace(rot.begin(), rot.end(), ',', ' ');
		replace(tra.begin(), tra.end(), ',', ' ');

		float data[6];
		stringstream dataR(rot);
		stringstream dataT(tra);
		for (int i = 0; i <= 3; i++) {
			dataR >> data[i];
			dataT >> data[i + 3];
		}

		Joint* newJoint = new Joint(glm::vec3(data[3], data[4], data[5]), 
			glm::vec3(data[0], data[1], data[2]), 
			glm::vec3(1, 1, 1), 
			name);
		objs.push_back(newJoint);
		if (parentName.compare("NULL")) {
			//std::cout << name << " has parent\n";
			for (SceneObject* obj : objs)
			{
				//std::cout << parentName << ' ' << obj->getName() << '\n';
				if (!parentName.compare(obj->getName()))
					obj->addChild(newJoint);
			}
		}
		renderObjects.push_back(newJoint);
		scene.push_back(newJoint);
	}

	std::cout << filename << " Loaded.\n";
}

// Render each frame set on animation mode
//
void ofApp::renderAnimation()
{
	if (!playAnimation)
	{
		playAnimation = true;
		animator.play();
		std::string frameName = "/animation/anim";
		std::string extension = ".png";
		animator.animate();
		for (int i = animator.getMinFrame(); i <= animator.getMaxFrame(); i++)
		{
			renderer.render(frameName + std::to_string(animator.getCurrentFrame()) + extension, Renderer::RenderMethod::RAY_TRACE);
			animator.advanceFrame();
		}

		playAnimation = false;
		animator.stop();
	}
}

// GUI buttons event listeners
//
void ofApp::addSpherePressed()
{
	SceneObject* newObject = new Sphere(glm::vec3(0, 0, 0), radiusSlider, colorSlider);
	renderObjects.push_back(newObject);
	scene.push_back(newObject);
}

void ofApp::addConePressed()
{
	SceneObject* newObject = new Cone(glm::vec3(0, 0, 0), radiusSlider, heightSlider, colorSlider);
	renderObjects.push_back(newObject);
	scene.push_back(newObject);
}

void ofApp::addMeshPressed()
{
	SceneObject* newObject = new Mesh(glm::vec3(0, 0, 0), modelLoader.getMesh(0), colorSlider);
	renderObjects.push_back(newObject);
	scene.push_back(newObject);
}

void ofApp::addJointPressed()
{
	if (objSelected() && dynamic_cast<Joint*>(selected[0]))
	{
		Joint* newJoint = new Joint(selected[0], "Joint" + std::to_string(jointIndex));
		renderObjects.push_back(newJoint);
		scene.push_back(newJoint);
	}
	else
	{
		Joint* newJoint = new Joint("Joint" + std::to_string(jointIndex));
		renderObjects.push_back(newJoint);
		scene.push_back(newJoint);
	}
	jointIndex++;
}

void ofApp::addLightPressed()
{
	{
		Light* newLight = new Light(glm::vec3(0, 0, 0), 0.8);
		lights.push_back(newLight);
		scene.push_back(newLight);
	}
}