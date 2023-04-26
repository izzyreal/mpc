#pragma once

#include <string>

namespace mpc {
	class Mpc;
}

namespace mpc::hardware {
	class HwComponent
	{

	protected:
		mpc::Mpc& mpc;
        const std::string label;
        
	public:
        virtual void push();
        virtual void push(int /* velocity */);
        virtual void release() {}

        std::string getLabel();
        
	protected:
		HwComponent(mpc::Mpc& mpc, const std::string& label);

	};
}
