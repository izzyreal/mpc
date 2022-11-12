#include <sequencer/TimeSignature.hpp>

using namespace mpc::sequencer;

void TimeSignature::setNumerator(int i)
{
    numerator = i;
    
    notifyObservers(std::string("timesignature"));
    
    notifyObservers(std::string("beat"));
}

int TimeSignature::getNumerator()
{
    return numerator;
}

void TimeSignature::setDenominator(int i)
{
    denominator = i;
    
    notifyObservers(std::string("timesignature"));
    
    notifyObservers(std::string("beat"));
}

int TimeSignature::getDenominator()
{
    return denominator;
}

void TimeSignature::increase()
{
    switch (getDenominator()) {
    case 4:
    case 8:
    case 16:
        if(getNumerator() != 16) {
            setNumerator(getNumerator() + 1);
            break;
        } else {
            setNumerator(1);
            setDenominator(getDenominator() * 2);
            break;
        }
    case 32:
        if(getNumerator() != 32) {
            setNumerator(getNumerator() + 1);
            break;
        }
    }

}

void TimeSignature::decrease()
{
	switch (getDenominator()) {
	case 4:
		if (getNumerator() != 1) {
			setNumerator(getNumerator() - 1);
		}
		break;
	case 8:
	case 16:
	case 32:
		if (getNumerator() == 1) {
			setNumerator(16);
			setDenominator(getDenominator() / 2);
			break;
		}
		else {
			setNumerator(getNumerator() - 1);
			break;
		}
	}

}
