#pragma once

#include "Arduino.h"
#include "InterruptButton.h"

class InterruptButtonWrapper {
public:
	InterruptButtonWrapper(const int32_t pin, const std::function<void(void)> &handler)
		: button(pin, LOW), onButtonPressed(handler) {};
	void loop() {
		if (buttonPressed) {
			onButtonPressed();
			buttonPressed = false;
		}
	}

	void bind() {
		button.bind(Event_KeyPress, [&]() { buttonPressed = true; });
	}

private:
	InterruptButton button;
	bool buttonPressed = false;
	std::function<void(void)> onButtonPressed;
};

template <int ButtonCount> class InterruptButtonManager {
public:
	InterruptButtonManager(InterruptButtonWrapper *_buttons[]) : buttons(_buttons) {}

	void bindAllButtons() {
		for (int i = 0; i < ButtonCount; i++) {
			buttons[i]->bind();
		}
	}

	void loop() {
		for (int i = 0; i < ButtonCount; i++) {
			buttons[i]->loop();
		}
	}

private:
	InterruptButtonWrapper **buttons;
};