#include <engine/control/CompoundControl.hpp>
#include <engine/control/Control.hpp>

using namespace ctoot::control;
using namespace std;

CompoundControl::CompoundControl(int id, string name) : Control(id, name)
{
}

vector<string> CompoundControl::getControlNamesRecursive(int generation)
{
	vector<string> res;
	string indent;

    for (int i = 0; i < generation; i++)
		indent += "     ";

    res.push_back("\n" + indent + getName() + " has these controls:");
	
    for (auto& c : controls)
		res.push_back(indent + c->getName());

    for (auto& c : controls)
    {
		auto cc = dynamic_pointer_cast<CompoundControl>(c);
	
        if (cc)
        {
			auto strings = cc->getControlNamesRecursive(generation + 1);
		
            for (auto& s : strings)
				res.push_back(s);
		}
	}
    
	return res;
}

void CompoundControl::add(shared_ptr<Control> control)
{
	if (!control) return;
	string name = control->getName();
	control->setParent(this);
	controls.push_back(move(control));
    weakControls.push_back(controls.back());
}

void CompoundControl::remove(shared_ptr<Control> c)
{
	auto control = c;
	
    if (!control)
        return;
	
    for (int i = 0; i < controls.size(); i++)
    {
		auto currentControl = controls[i];
		
        if (currentControl == control)
        {
			controls.erase(begin(controls) + i);
            weakControls.erase(begin(weakControls) + i);
			break;
		}
	}
}

vector<shared_ptr<Control>> CompoundControl::getControls()
{
    return weakControls;
}

shared_ptr<Control> CompoundControl::find(string name)
{
	for (int i = 0; i < controls.size(); i++) {
		if (controls[i]->getName().compare(name) == 0) {
			return controls[i];
		}
	}
	return {};
}

shared_ptr<Control> CompoundControl::deepFind(int controlId)
{
	for (auto& c : controls) {

		auto cc = dynamic_pointer_cast<CompoundControl>(c);

		if (cc) {
			auto c2 = cc->deepFind(controlId);
			if (c2) return c2;

		}
		else if (controlId == c->getId()) {
			return c;
		}
	
	}
	return {};
}

void CompoundControl::disambiguate(shared_ptr<CompoundControl> c)
{
	auto original = c->getName();
	if (!find(original)) {
		return;
	}
	int index = 1;
	string str;
	do {
		index++;
		str = original + " #" + (to_string(index));
	} while (find(str));
	c->setName(str);
}
