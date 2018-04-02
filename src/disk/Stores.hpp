#pragma once

#include <memory>
#include <vector>

namespace mpc {
	namespace disk {

		class Store;

		class Stores final
		{

		private:
			std::vector<std::shared_ptr<Store>> stores{};

		private:
			void printStore(Store* s);

		public:
			std::weak_ptr<Store> getStdStore(int store);
			std::weak_ptr<Store> getRawStore(int store);
			std::vector<std::weak_ptr<Store>> getRawStores();
			std::vector<std::weak_ptr<Store>> getStdStores();

			Stores();
			~Stores();
			//friend class Store;
		};

	}
}
