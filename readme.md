# ofxMachineVision

A set of tools for dealing with machine vision cameras, based on interface ofxMachineVision::Device.

# Usage

## Visual Studio

1. Add the ofxMachineVisionLib/ofxMachineVisionLib.vcxproj project to your solution
2. Go to your project properties, go to 'Common Properties' at the top of the tree on the left, select 'Add New Reference...' and add the ofxMachineVisionLib project.
3. Add the props file from your camera driver to your project:
    - Go to Property Manager tab
	- Right click on your project
	- 'Add Existing Property Sheet...'
	- Select the property sheet for your camera driver addon (e.g. addons/ofxXimea/ofxXimea.props)
4. Add the source files from your camera driver to your project (.cpp, .h)
5. Include the camera driver header in your testApp.h (e.g. ofxXimea.h) or as appropriate
6. Denote your camera driver's `Device` class when instantiating `ofxMachineVison::Grabber`s (e.g. `ofxMachineVision::SimpleGrabber<ofxXimea::Device> grabber;`)