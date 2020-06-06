#include "Controls.hpp"

#include <Mpc.hpp>

#include <controls/BaseControls.hpp>

#include <controls/GlobalReleaseControls.hpp>

#include <controls/midisync/SyncControls.hpp>

using namespace mpc::controls;

Controls::Controls()
{
	pressedPadVelos = std::vector<int>(16);

	controls["release"] = new GlobalReleaseControls();

	controls["sync"] = new midisync::SyncControls();
}

void Controls::setCtrlPressed(bool b) {
	ctrlPressed = b;
}

void Controls::setAltPressed(bool b) {
	altPressed = b;
}

bool Controls::isCtrlPressed() {
	return ctrlPressed;
}

bool Controls::isAltPressed() {
	return altPressed;
}

bool Controls::isErasePressed() {
	return erasePressed;
}

bool Controls::isRecPressed() {
	return recPressed;
}

bool Controls::isOverDubPressed() {
	return overDubPressed;
}

bool Controls::isTapPressed() {
	return tapPressed;
}

bool Controls::isGoToPressed() {
	return goToPressed;
}

bool Controls::isShiftPressed() {
	return shiftPressed;
}

bool Controls::isF3Pressed() {
	return f3Pressed;
}

bool Controls::isF4Pressed() {
	return f4Pressed;
}

bool Controls::isF5Pressed() {
	return f5Pressed;
}

bool Controls::isF6Pressed() {
	return f6Pressed;
}

std::set<int>* Controls::getPressedPads() {
	return &pressedPads;
}

std::vector<int>* Controls::getPressedPadVelos() {
	return &pressedPadVelos;
}

void Controls::setErasePressed(bool b) {
	erasePressed = b;
}

void Controls::setRecPressed(bool b) {
	recPressed = b;
}

void Controls::setOverDubPressed(bool b) {
	overDubPressed = b;
}

void Controls::setTapPressed(bool b) {
	tapPressed = b;
}

void Controls::setGoToPressed(bool b) {
	goToPressed = b;
}

void Controls::setShiftPressed(bool b) {
	shiftPressed = b;
}

void Controls::setF3Pressed(bool b) {
	f3Pressed = b;
}

void Controls::setF4Pressed(bool b) {
	f4Pressed = b;
}

void Controls::setF5Pressed(bool b) {
	f5Pressed = b;
}

void Controls::setF6Pressed(bool b) {
	f6Pressed = b;
}

BaseControls* Controls::getControls(std::string s)
{
	return controls[s];
}

GlobalReleaseControls* Controls::getReleaseControls()
{
	return (GlobalReleaseControls*) controls["release"];
}

Controls::~Controls()
{
	for (auto c : controls)
	{
		delete c.second;
	}
}
