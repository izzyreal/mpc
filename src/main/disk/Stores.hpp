#pragma once

#include <memory>
#include <vector>

namespace mpc::disk
{
	class Store;

	class Stores final
	{

	private:
		std::vector<std::shared_ptr<Store>> stores;

	public:
		std::weak_ptr<Store> getStdStore(int store);
		std::weak_ptr<Store> getRawStore(int store);
		std::vector<std::weak_ptr<Store>> getRawStores();
		std::vector<std::weak_ptr<Store>> getStdStores();

		Stores();

	};
}
