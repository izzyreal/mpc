#include "Layer.hpp"
#include "Field.hpp"
#include "Label.hpp"

#include "LayeredScreen.hpp"

#include <Logger.hpp>

#include <string>

using namespace rapidjson;
using namespace mpc::lcdgui;
using namespace std;

Layer::Layer(mpc::lcdgui::LayeredScreen* ls, std::vector<std::vector<bool>>* atlas, moduru::gui::bmfont* font)
{
	this->ls = ls;
	bg = std::make_shared<Background>();
	fk = std::make_shared<FunctionKeys>(atlas, font);
	for (int i = 0; i < 100; i++) {
		unusedFields.push_back(make_shared<mpc::lcdgui::Field>(ls, atlas, font));
	}
	for (int i = 0; i < 1000; i++) {
		unusedLabels.push_back(make_shared<mpc::lcdgui::Label>(atlas, font));
	}
	blinkLabel = make_shared<mpc::lcdgui::BlinkLabel>("SOLO", atlas, font);
	blinkLabel->initialize("soloblink", "SOLO", 133, 51, 4);
}

weak_ptr<mpc::lcdgui::Field> Layer::getUnusedField() {
	auto res = unusedFields[0];
	unusedFields.erase(unusedFields.begin());
	usedFields.push_back(res);
	return res;
}

void Layer::unuseField(weak_ptr<mpc::lcdgui::Field> field) {
	auto lField = field.lock();
	lField->setFocusable(false);
	for (int i = 0; i < usedFields.size(); i++) {
		if (usedFields[i] == lField) {
			unusedFields.push_back(lField);
			usedFields.erase(usedFields.begin() + i);
			break;
		}
	}
}

weak_ptr<mpc::lcdgui::Label> Layer::getUnusedLabel() {
	auto res = unusedLabels[0];
	unusedLabels.erase(unusedLabels.begin());
	usedLabels.push_back(res);
	return res;
}

void Layer::unuseLabel(weak_ptr<mpc::lcdgui::Label> label) {
	auto lLabel = label.lock();
	for (int i = 0; i < usedLabels.size(); i++) {
		if (usedLabels[i] == lLabel) {
			unusedLabels.push_back(lLabel);
			usedLabels.erase(usedLabels.begin() + i);
		}
	}
}

vector<mpc::lcdgui::Parameter*> Layer::getParameters() {
	auto res = vector<mpc::lcdgui::Parameter*>();
	for (auto& p : params)
		res.push_back(p.get());
	return res;
}

vector<mpc::lcdgui::Info*> Layer::getInfos() {
	auto res = vector<mpc::lcdgui::Info*>();
	for (auto& p : infos)
		res.push_back(p.get());
	return res;
}

mpc::lcdgui::Background* Layer::getBackground() {
	return bg.get();
}

mpc::lcdgui::FunctionKeys* Layer::getFunctionKeys() {
	return fk.get();
}

void Layer::setFocus(string textFieldName) {

	string oldFocus = focus;
	// First make sure the desired focus field exists and is focusable
	bool exists = false;
	for (auto& a : getAllFields()) {
		auto tf = dynamic_pointer_cast<Field>(a.lock());
		//if (!tf->IsHidden() && tf->getName().compare(textFieldName) == 0 && tf->isFocusable()) {
		if (tf->getName().compare(textFieldName) == 0 && tf->isFocusable()) {
			exists = true;
			break;
		}
	}
	if (!exists) return;
	for (auto& a : getAllFields()) {
		auto tf = dynamic_pointer_cast<Field>(a.lock());
		if (tf->getName().compare(focus) == 0 && tf->isFocusable()) {
			tf->loseFocus(textFieldName);
			break;
		}
	}
	focus = textFieldName;
	for (auto& a : getAllFields()) {
		auto tf = dynamic_pointer_cast<Field>(a.lock());
		if (tf->getName().compare(textFieldName) == 0 && tf->isFocusable()) {
			tf->takeFocus(oldFocus);
			break;
		}
	}
}

string Layer::getFocus() {
	return focus;
}

string Layer::openScreen(Value& screenJson, string screenName) {
	//focus = "";
	bg->Hide(false);
	bg->setName(screenName);
	Value& labels = screenJson["labels"];
	Value& x = screenJson["x"];
	Value& y = screenJson["y"];
	Value& parameters = screenJson["parameters"];
	Value& tfsize = screenJson["tfsize"];

	blinkLabel->setBlinking(false);

	string firstField = "";
	for (int i = 0; i < labels.Size(); i++) {
		if (i == 0) {
			firstField = parameters[i].GetString();
		}
		params.push_back(make_unique<lcdgui::Parameter>(getUnusedField(), getUnusedLabel(), labels[i].GetString()
			, parameters[i].GetString()
			, x[i].GetInt()
			, y[i].GetInt()
			, tfsize[i].GetInt()
			));
	}

	if (screenJson.HasMember("infowidgets")) {
		Value& infoNames = screenJson["infowidgets"];
		Value& infoSize = screenJson["infosize"];
		Value& infoX = screenJson["infox"];
		Value& infoY = screenJson["infoy"];
		for (int i = 0; i < infoNames.Size(); i++) {
			infos.push_back(make_unique<lcdgui::Info>(getUnusedLabel(), infoNames[i].GetString()
				, infoX[i].GetInt()
				, infoY[i].GetInt()
				, infoSize[i].GetInt()));
		}
	}

	if (screenJson.HasMember("fblabels")) {
		Value& fklabels = screenJson["fblabels"];
		Value& fktypes = screenJson["fbtypes"];
		fk->Hide(false);
		fk->initialize(fklabels, fktypes);
	}
	else {
		fk->Hide(true);
	}
	return firstField;
}

void Layer::clear() {
	for (auto& p : params) {
		unuseField(p->getTf());
		unuseLabel(p->getLabel());
	}
	params.clear();
	for (auto& i : infos) {
		unuseLabel(i->getLabel());
	}
	infos.clear();
	bg->Hide(true);
	fk->Hide(true);
}

void Layer::setDirty() {
	bg->SetDirty();
	fk->SetDirty();
	for (auto& i : infos) {
		i->getLabel().lock()->SetDirty();
	}
	for (auto& p : params) {
		p->getTf().lock()->SetDirty();
		p->getLabel().lock()->SetDirty();
	}
}

vector<weak_ptr<mpc::lcdgui::Component>> Layer::getComponentsThatNeedClearing() {
	vector<weak_ptr<mpc::lcdgui::Component>> result;
	for (auto& c : usedFields) {
		if (c->NeedsClearing()) {
			result.push_back(c);
		}
	}
	for (auto& c : getAllLabels()) {
		if (c.lock()->NeedsClearing()) {
			result.push_back(c);
		}
	}
	return result;
}

vector<weak_ptr<mpc::lcdgui::Component>> Layer::getAllLabelsAndFields() {
	vector<weak_ptr<mpc::lcdgui::Component> > result;
	for (auto& c : getAllLabels()) {
		result.push_back(c);
	}
	for (auto& c : getAllFields()) {
		result.push_back(c);
	}
	return result;
}


vector<weak_ptr<mpc::lcdgui::Component>> Layer::getAllFields() {
	vector<weak_ptr<mpc::lcdgui::Component>> result;
	weak_ptr<mpc::lcdgui::Component> focus;
	for (auto& f : usedFields) {
		if (!f->hasFocus()) {
			result.push_back(f);
		}
		else {
			focus = f;
		}
	}
	if (focus.lock()) result.push_back(focus);
	return result;
}

vector<weak_ptr<mpc::lcdgui::Component>> Layer::getAllLabels() {
	vector<weak_ptr<mpc::lcdgui::Component>> result;
	for (auto& l : usedLabels)
		result.push_back(l);
	if (blinkLabel && ls->getCurrentScreenName().compare("sequencer") == 0) result.push_back(blinkLabel);
	return result;
}

std::weak_ptr<mpc::lcdgui::Background> Layer::getBackgroundWk() {
	return bg;
}

std::weak_ptr<mpc::lcdgui::FunctionKeys> Layer::getFunctionKeysWk() {
	return fk;
}

Layer::~Layer() {
	//if (bg != nullptr) delete bg;
	//if (fk != nullptr) delete fk;
}
