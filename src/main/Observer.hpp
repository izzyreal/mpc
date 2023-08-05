#pragma once
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace mpc {
class Observable;
}

namespace mpc {
using Message = std::variant<std::string, int, std::pair<float, float>>;
class Observer
{
public:
    virtual void update(Observable*, Message) {}
};

class Observable
{

public:
    virtual void notifyObservers()
    {
        notifyObservers(nullptr);
    }

    virtual void notifyObservers(Message message)
    {
        for (auto& a : observers)
        {
            a->update(this, message);
        }
    }

    void addObserver(Observer* o)
    {
        for (auto& obs : observers)
        {
            if (obs == o)
            {
                return;
            }
        }

        observers.push_back(o);
    }

    void deleteObserver(Observer* o)
    {
        int counter = 0;

        for (auto& o1 : observers)
        {
            if (o1 == o)
            {
                observers.erase(observers.begin() + counter);
                break;
            }
            counter++;
        }
    }

private:
    std::vector<Observer*> observers;

};

}
