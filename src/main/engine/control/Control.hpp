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
        virtual int getId();

        CompoundControl *getParent();

        virtual ~Control() {}

    protected:
        virtual void setParent(CompoundControl *c);

        virtual void notifyParent(Control *obj);

        virtual void derive(Control *obj);

    public:
        virtual std::string getName();

        virtual void setName(std::string s);

        virtual std::string getValueString();

    protected:
        Control(int id, std::string name);

    private:
        friend class CompoundControl;

        friend class CompoundControlChain;

    };
}
