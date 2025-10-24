#pragma once
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <algorithm>

namespace mpc
{
    class Observable;
}

namespace mpc
{

    using Message = std::variant<std::string, int, std::pair<float, float>>;

    class Observer
    {
    public:
        virtual void update(Observable *, Message) {}
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
            notifying = true;

            for (auto *a : observers)
            {
                // Skip any that were already scheduled for removal
                if (std::find(pendingRemovals.begin(), pendingRemovals.end(), a) == pendingRemovals.end())
                {
                    a->update(this, message);
                }
            }

            notifying = false;

            // Perform deferred removals
            for (auto *o : pendingRemovals)
            {
                eraseObserver(o);
            }
            pendingRemovals.clear();
        }

        void addObserver(Observer *o)
        {
            auto it = std::find(pendingRemovals.begin(), pendingRemovals.end(), o);
            if (it != pendingRemovals.end())
            {
                pendingRemovals.erase(it);
            }

            if (std::find(observers.begin(), observers.end(), o) == observers.end())
            {
                observers.push_back(o);
            }
        }

        void deleteObserver(Observer *o)
        {
            if (notifying)
            {
                // Mark for removal after notification
                if (std::find(pendingRemovals.begin(), pendingRemovals.end(), o) == pendingRemovals.end())
                {
                    pendingRemovals.push_back(o);
                }
                return;
            }

            eraseObserver(o);
        }

    private:
        std::vector<Observer *> observers;
        std::vector<Observer *> pendingRemovals;
        bool notifying = false;

        void eraseObserver(Observer *o)
        {
            observers.erase(std::remove(observers.begin(), observers.end(), o), observers.end());
        }
    };

} // namespace mpc
