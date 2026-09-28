#include "TextInputField.h"

void TextInputField::setup(float x, float y, float width, float height) {
	bounds = ofRectangle(x, y, width, height);
	text = "";
	active = false;
}

void TextInputField::draw() {
	ofSetColor(255);
	ofDrawRectangle(bounds);

	ofSetColor(active ? ofColor(80, 140, 255) : ofColor(120));
	ofNoFill();
	ofDrawRectangle(bounds);
	ofFill();

	ofSetColor(isValid() ? ofColor(30) : ofColor(200, 40, 40));
	std::string display = text.empty() ? "0" : text;
	ofDrawBitmapString(display, bounds.x + 10, bounds.y + bounds.height / 2 + 4);

	// Simple blinking caret when focused.
	if (active && (int)(ofGetElapsedTimef() * 2) % 2 == 0) {
		float textWidth = 8.0f * display.length(); // bitmap font is ~8px per char
		ofDrawLine(bounds.x + 10 + textWidth, bounds.y + 6,
		           bounds.x + 10 + textWidth, bounds.y + bounds.height - 6);
	}

	ofSetColor(255);
}

void TextInputField::keyPressed(int key) {
	if (!active) return;

	if (key == OF_KEY_BACKSPACE) {
		if (!text.empty()) text.pop_back();
		return;
	}

	// Only accept digits and a single decimal point - this is where
	// non-numerical input gets rejected, rather than after the fact.
	if (key >= '0' && key <= '9') {
		text += (char)key;
	}
	else if (key == '.' && text.find('.') == std::string::npos) {
		// Don't allow a leading '.' with nothing before it looking odd -
		// insert a leading zero for clarity.
		if (text.empty()) text = "0";
		text += '.';
	}
}

void TextInputField::mousePressed(int x, int y) {
	active = bounds.inside(x, y);
}

bool TextInputField::isValid() const {
	if (text.empty() || text == ".") return false;
	try {
		std::stod(text);
		return true;
	}
	catch (const std::exception &) {
		return false;
	}
}

double TextInputField::getValue() const {
	if (!isValid()) return 0.0;
	try {
		return std::stod(text);
	}
	catch (const std::exception &) {
		return 0.0;
	}
}
