#include "SelectionMenu.h"

void SelectionMenu::setup(const std::vector<std::string> & items, int x, int y, int w, int h) {
	options = items;
	box.set(x, y, w, h);
}

void SelectionMenu::draw() {
	ofSetColor(255);
	ofNoFill();
	ofDrawRectangle(box);

	if (!options.empty()) {
		ofSetColor(70, 130, 220); // currency text color
		ofDrawBitmapString(options[selectedIndex], box.x + 10, box.y + box.height / 2);
	}
}

void SelectionMenu::mousePressed(int mx, int my) {
	if (box.inside(mx, my)) {
		selectedIndex++;
		if (selectedIndex >= options.size()) {
			selectedIndex = 0;
		}
	}
}

void SelectionMenu::setSelectedIndex(int index) {
	if (index >= 0 && index < options.size()) {
		selectedIndex = index;
	}
}

int SelectionMenu::getSelectedIndex() const {
	return selectedIndex;
}

std::string SelectionMenu::getSelectedItem() const {
	if (options.empty()) return "";
	return options[selectedIndex];
}
