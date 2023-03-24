#pragma once

#include <cstdint>
#include <string>
#include <map>

namespace mpc::engine::control {
    class CompoundControl;

    class Control
    {
    private:
        int id;
        CompoundControl *parent{nullptr};
        std::string name;

    public:
        int getId();

        CompoundControl *getParent();


    protected:
        void setParent(CompoundControl *c);

        void notifyParent(Control *obj);

        virtual void derive(Control *obj);

    public:
        std::string getName();

        void setName(std::string s);

        virtual std::string getValueString();

    protected:
        Control(int id, std::string name);

    private:
        friend class CompoundControl;
        friend class CompoundControlChain;

    };
}
