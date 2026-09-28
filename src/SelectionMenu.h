#pragma once

#include "ofMain.h"
#include <string>
#include <vector>

// A small, self-contained dropdown/select control. Built from scratch
// (rather than pulled from an addon) to keep the project dependency-free
// and to demonstrate custom GUI component design.
class SelectionMenu {
public:
	void setup(const std::vector<std::string> & items, int x, int y, int w, int h);

	void draw();
	void mousePressed(int mx, int my);

	void setSelectedIndex(int index);
	int getSelectedIndex() const;
	std::string getSelectedItem() const;

private:
	std::vector<std::string> options;
	int selectedIndex = 0;

	ofRectangle box;
};
