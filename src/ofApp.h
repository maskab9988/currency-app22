#pragma once
#include "CurrencyAPI.h"
#include "SelectionMenu.h"
#include "TextInputField.h"
#include "ofMain.h"
#include <string>
#include <vector>

class ofApp : public ofBaseApp {
public:
	void setup() override;
	void update() override;
	void draw() override;

	void keyPressed(int key) override;
	void mousePressed(int x, int y, int button) override;

private:
	void loadCurrencyList();
	void loadApiKey();
	void doConversion();
	void swapCurrencies();

	void onRateReceived(double & rate);
	void onApiError(std::string & message);

	CurrencyAPI api;
	SelectionMenu fromDropdown;
	SelectionMenu toDropdown;
	TextInputField amountField;

	std::vector<std::string> currencyCodes;

	double pendingAmount = 0.0;
	double convertedAmount = 0.0;
	double currentRate = 0.0;

	std::string statusMessage = "Enter an amount and press Convert.";
	bool hasError = false;

	ofRectangle convertButton;
	ofRectangle swapButton;
};
