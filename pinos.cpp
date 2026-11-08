#include <Arduino.h>

void setPins() {
	pinMode(13, OUTPUT);
	pinMode(21, OUTPUT);
	pinMode(22, OUTPUT);
	pinMode(23, OUTPUT);
	pinMode(24, OUTPUT);
	pinMode(25, OUTPUT);
	pinMode(26, OUTPUT);
	pinMode(27, OUTPUT);
	pinMode(28, OUTPUT);
};

void makeChange(String room, String changeReq) {
	changeReq.trim();
	bool change;

	if (changeReq == "true") {
		change = true;
	} else if (changeReq == "false") {
		change = false;
	}

	if (room == "sala") {
		digitalWrite(13, change);
	} else if (room == "qua1") {
		digitalWrite(21, change);
	} else if (room == "qua2") {
		digitalWrite(22, change);
	} else if (room == "czna") {
		digitalWrite(23, change);
	} else if (room == "bnho") {
		digitalWrite(24, change);
	} else if (room == "jan1") {
		digitalWrite(25, change);
	} else if (room == "jan2") {
		digitalWrite(26, change);
	} else if (room == "grgm") {
		digitalWrite(27, change);
	} else if (room == "port") {
		digitalWrite(28, change);
	}
};