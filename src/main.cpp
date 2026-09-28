#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	// Classic OF window setup - compatible with OF 0.10 through 0.12
	ofSetupOpenGL(900, 600, OF_WINDOW);

	// This kicks off the running of my app
	ofRunApp(new ofApp());
}
