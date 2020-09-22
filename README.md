# ComputerGraphicsSandbox
Program that implements various CG tools such as Scene Creation, Keyframe Animation, and Image Rendering.

by Jasper Matthew Dumdumaya

**Requires OpenFrameworks v11+**

**Addons**
- ofxAssimpModelLoader
- ofxGui

**Setup**

- Start ProjectGenerator.exe
- Addons: ofxAssimpModelLoader, ofxGui
- Generate project
- Move bin/data/ and src/ to *project*/

**TODO**
- add more SceneObjects
- fix raymarcher/add more sdf
- fix various bugs
- improve GUI

This project was built on and inspired by a 
raytracing project I did in university.
There are some functions I used that were
written by my professor. Credits for these
go to Kevin Smith, CS116A SJSU, 2018.
ofApp::mousePressed
ofApp::mouseReleased
ofApp::mouseToDragPlane
SceneObject::getRotateMatrix
SceneObject::getTranslateMatrix
SceneObject::getScaleMatrix
SceneObject::getLocalMatrix
SceneObject::getMatrix
SceneObject::rotateToVector
SceneObject::addChild
Sphere::draw