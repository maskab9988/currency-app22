#include "ofApp.h"
#include <algorithm>
#include <fstream>

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Currency Converter");
	ofBackground(230, 240, 250);
	ofSetFrameRate(30);

	loadCurrencyList();
	loadApiKey();

	fromDropdown.setup(currencyCodes, 130, 140, 160, 30);
	toDropdown.setup(currencyCodes, 390, 140, 160, 30);
	toDropdown.setSelectedIndex(currencyCodes.size() > 1 ? 1 : 0);

	amountField.setup(130, 80, 240, 34);

	convertButton = ofRectangle(630, 130, 180, 50);
	swapButton = ofRectangle(325, 130, 55, 50);

	// Subscribe to the API's async events instead of polling it.
	ofAddListener(api.onRateReceived, this, &ofApp::onRateReceived);
	ofAddListener(api.onError, this, &ofApp::onApiError);
}

//--------------------------------------------------------------
void ofApp::loadCurrencyList() {
	// Currencies are loaded from an external file rather than hard-coded,
	// so the supported list can be edited without recompiling.
	ofFile file("currencies.txt");

	if (file.exists()) {
		ofBuffer buffer = ofBufferFromFile("currencies.txt");
		for (auto line : buffer.getLines()) {
			std::string code = ofTrim(line);
			if (!code.empty()) currencyCodes.push_back(code);
		}
	}

	// Fallback list in case the data file is missing, so the app never
	// starts with an empty dropdown.
	if (currencyCodes.empty()) {
		currencyCodes = { "USD", "GBP", "EUR", "JPY", "AUD", "CAD", "CHF", "CNY", "INR", "NZD" };
		ofLogWarning("ofApp") << "currencies.txt not found - using built-in default list.";
	}
}

//--------------------------------------------------------------
void ofApp::loadApiKey() {
	ofFile file("apikey.txt");
	std::string key = "";

	if (file.exists()) {
		ofBuffer buffer = ofBufferFromFile("apikey.txt");
		key = ofTrim(buffer.getText());
	}

	if (key.empty() || key == "YOUR_API_KEY_HERE") {
		ofLogWarning("ofApp") << "No valid API key found in bin/data/apikey.txt";
	}

	api.setup(key);
}

//--------------------------------------------------------------
void ofApp::update() {
	// Intentionally minimal - all API results arrive asynchronously via
	// the event listeners (onRateReceived / onApiError), not by polling.
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofSetColor(20);
	std::string title = "Currency Converter";
	float titleScale = 2.0;
	float titleWidth = title.length() * 8 * titleScale;
	float titleX = (ofGetWidth() - titleWidth) / 2.0;
	ofPushMatrix();
	ofTranslate(titleX, 25);
	ofScale(titleScale, titleScale);
	ofDrawBitmapString(title, 0, 0);
	ofPopMatrix();

	amountField.draw();
	ofSetColor(70, 130, 220);
	ofDrawBitmapString("Amount", 130, 75);

	fromDropdown.draw();
	toDropdown.draw();

	// Swap button
	ofSetColor(230);
	ofDrawRectangle(swapButton);
	ofSetColor(60);
	ofNoFill();
	ofDrawRectangle(swapButton);
	ofFill();
	ofSetColor(30);
	ofDrawBitmapString("<>", swapButton.x + 18, swapButton.y + 30);

	// Convert button
	ofSetColor(70, 130, 220);
	ofDrawRectangle(convertButton);
	ofSetColor(255);
	ofDrawBitmapString("Convert", convertButton.x + 60, convertButton.y + 30);

	// Status / result area
	ofSetColor(255);
	ofDrawRectangle(130, 220, 640, 140);
	ofSetColor(120);
	ofNoFill();
	ofDrawRectangle(130, 220, 640, 140);
	ofFill();

	if (api.getState() == ApiState::Loading) {
		ofSetColor(90);
		ofDrawBitmapString("Fetching latest rates...", 150, 260);
	} else if (hasError) {
		ofSetColor(200, 40, 40);
		ofDrawBitmapString("Error:", 150, 250);
		ofDrawBitmapString(statusMessage, 150, 270);
	} else {
		ofSetColor(20);
		ofPushMatrix();
		ofTranslate(120, 0);
		ofScale(1.8, 1.8);
		std::string resultLine = ofToString(pendingAmount, 2) + " " + fromDropdown.getSelectedItem() + " = " + ofToString(convertedAmount, 2) + " " + toDropdown.getSelectedItem();
		ofDrawBitmapString(resultLine, 50, 140);
		ofPopMatrix();

		ofSetColor(90);
		if (currentRate > 0) {
			std::string rateLine = "1 " + fromDropdown.getSelectedItem() + " = " + ofToString(currentRate, 4) + " " + toDropdown.getSelectedItem();
			ofDrawBitmapString(rateLine, 150, 300);
		}
		if (!api.getLastUpdated().empty()) {
			ofDrawBitmapString("Last updated: " + api.getLastUpdated(), 150, 320);
		}
	}

	if (!amountField.isValid() && !amountField.getText().empty()) {
		ofSetColor(200, 40, 40);
		ofDrawBitmapString("Please enter a valid number.", 130, 115);
	}

	ofSetColor(255);
}

//--------------------------------------------------------------
void ofApp::doConversion() {

	if (!amountField.isValid()) {
		hasError = true;
		statusMessage = "Please enter a valid numeric amount before converting.";
		return;
	}

	hasError = false;
	pendingAmount = amountField.getValue();
	api.requestRate(fromDropdown.getSelectedItem(), toDropdown.getSelectedItem());
}

//--------------------------------------------------------------
void ofApp::swapCurrencies() {
	int fromIndex = fromDropdown.getSelectedIndex();
	int toIndex = toDropdown.getSelectedIndex();
	fromDropdown.setSelectedIndex(toIndex);
	toDropdown.setSelectedIndex(fromIndex);
}

//--------------------------------------------------------------
void ofApp::onRateReceived(double & rate) {
	currentRate = rate;
	convertedAmount = pendingAmount * rate;
	hasError = false;
}

//--------------------------------------------------------------
void ofApp::onApiError(std::string & message) {
	hasError = true;
	statusMessage = message;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	amountField.keyPressed(key);

	if (key == OF_KEY_RETURN) {
		doConversion();
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	amountField.mousePressed(x, y);

	fromDropdown.mousePressed(x, y);
	toDropdown.mousePressed(x, y);

	if (swapButton.inside(x, y)) {
		swapCurrencies();
	}

	if (convertButton.inside(x, y)) {
		doConversion();
	}
}
