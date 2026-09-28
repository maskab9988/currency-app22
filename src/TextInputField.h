#pragma once

#include "ofMain.h"
#include <string>

// A small numeric-only text field. Restricts input as the user types
// (digits and a single decimal point only) and exposes a validated
// double via getValue(), so ofApp never has to deal with malformed
// numeric strings.
class TextInputField {
public:
	void setup(float x, float y, float width, float height);
	void draw();

	void keyPressed(int key);
	void mousePressed(int x, int y); // returns focus state via getActive()

	double getValue() const;
	bool isValid() const;
	std::string getText() const { return text; }

	void setActive(bool a) { active = a; }
	bool getActive() const { return active; }

private:
	std::string text = "";
	bool active = false;
	ofRectangle bounds;
};
