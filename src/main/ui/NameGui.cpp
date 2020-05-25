#include <ui/NameGui.hpp>
#include <lang/StrUtil.hpp>
#include <Mpc.hpp>
#include <Util.hpp>

using namespace moduru::lang;
using namespace mpc::ui;
using namespace std;

void NameGui::setName(string name)
{
	this->name = StrUtil::padRight(name, " ", 16);
	nameLimit = 16;
}

void NameGui::setNameLimit(int i)
{
	name = name.substr(0, i);
	nameLimit = i;
}

int NameGui::getNameLimit()
{
    return nameLimit;
}

void NameGui::setName(string str, int i)
{
	name[i] = str[0];
}

string NameGui::getName()
{
	string s = name;
	while (!s.empty() && isspace(s.back())) s.pop_back();
	
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == ' ') s[i] = '_';
	}

	return StrUtil::padRight(s, " ", nameLimit);
}

void NameGui::changeNameCharacter(int i, bool up)
{
	char schar = name[i];
	string s{ schar };
	auto stringCounter = 0;
	for (auto str : mpc::Mpc::akaiAscii) {
		if (str.compare(s) == 0) {
			break;
		}
		stringCounter++;
	}

	if (stringCounter == 0 && !up) return;
	if (stringCounter == 75 && up) return;

	auto change = -1;
	if (up) change = 1;

	if (stringCounter > 75) {
		s = " ";
	}
	else {
		s = mpc::Mpc::akaiAscii[stringCounter + change];
	}
	name = name.substr(0, i).append(s).append(name.substr(i + 1, name.length()));
	setChanged();
	notifyObservers(string("name"));
}

void NameGui::setNameBeingEdited(bool b)
{
    editing = b;
}

void NameGui::setParameterName(string s)
{
	parameterName = s;
}

string NameGui::getParameterName()
{
    return parameterName;
}

bool NameGui::isNameBeingEdited()
{
	return editing;
}
