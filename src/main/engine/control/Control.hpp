#pragma once

#include <string>
#include <map>

namespace mpc::engine::control
{
    class CompoundControl;

    class Control
    {
        int id;
        CompoundControl *parent{nullptr};
        std::string name;

    public:
        virtual ~Control() = default;
        int getId() const;

        CompoundControl *getParent() const;

    protected:
        void setParent(CompoundControl *c);

        void notifyParent(Control *obj);

        virtual void derive(Control *obj);

    public:
        virtual std::string getName();

        void setName(const std::string &s);

        virtual std::string getValueString();

    protected:
        Control(int id, const std::string &name);

    private:
        friend class CompoundControl;
        friend class CompoundControlChain;
    };
} // namespace mpc::engine::control
