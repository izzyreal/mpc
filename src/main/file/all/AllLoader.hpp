#pragma once

#include <memory>
#include <vector>

namespace mpc {

	class Mpc;

	namespace ui {
		class Uis;
	}

	namespace disk {
		class MpcFile;
	}

	namespace sequencer {
		class Sequence;
	}

	namespace file {
		namespace all {

			class AllParser;
			class Sequence;

			class AllLoader
			{

			private:
				AllParser* allParser{};
				std::vector<mpc::file::all::Sequence*> sequencesALL{};
				std::vector<std::shared_ptr<mpc::sequencer::Sequence>> mpcSequences{};
				mpc::Mpc* mpc{};
			
			private:
				void convertSequences(const bool indiv);

			public:
				std::vector<std::shared_ptr<mpc::sequencer::Sequence>>* getSequences();

				AllLoader(mpc::disk::MpcFile* file);
				AllLoader(mpc::Mpc* mpc, mpc::disk::MpcFile* file);

			};

		}
	}
}
