#include <disk/Stores.hpp>

#include <disk/Store.hpp>
#include <Paths.hpp>

#include <file/FileSystem.hpp>

using namespace mpc::disk;
using namespace std;

Stores::Stores()
{
	vector<string> potentialRawStores = moduru::file::FileSystem::getRootDirectories(); // check for FAT16 volumes etc.
	/*
	auto iterator = npc(npc(::java::nio::file::FileSystems::getDefault())->getFileStores())->iterator();
	while (npc(iterator)->hasNext()) {
		auto fs = java_cast< ::java::nio::file::FileStore* >(npc(iterator)->next());
		if (npc(npc(fs)->type())->.compare(u"msdos"_j)) || npc(npc(fs)->type())->.compare(u"FAT"_j))) {
			try {
				auto volumeLabel = mpc::maingui::StartUp::osx() ? u"unknown"_j : npc(fs)->name();
				auto path = npc(fs)->name();
				if (!mpc::maingui::StartUp::osx()) {
					auto colonIndex = npc(npc(fs)->toString())->indexOf(u":"_j);
					path = npc(npc(fs)->toString())->substring(colonIndex - 1, colonIndex + 1);
				}
				auto store = new Store(this, path, npc(fs)->getTotalSpace(), volumeLabel, true);
				npc(stores).push_back(store));
			}
			catch (::java::io::IOException* e) {
				e->printStackTrace();
			}
		}
	}
	*/

	string defaultStdStoreLabel = "MPC2000XL";
	string defaultStdStorePath = mpc::Paths::storesPath() + defaultStdStoreLabel;
	int size = 0;
	size = 512 * 1024 * 1024;
	stores.push_back(make_shared<Store>(this, defaultStdStorePath, size, defaultStdStoreLabel, false));
}

void Stores::printStore(Store* s)
{
	/*
    npc(::java::lang::System::out())->println((u"path: "_j)->append(npc(s)->path)
        ->append(u"   size: "_j)
        ->append(npc(s)->totalSpace)
        ->append(u"   label: "_j)
        ->append(npc(s)->volumeLabel)
        ->append(u"   raw: "_j)
        ->append(npc(s)->raw)->toString());
		*/
}

weak_ptr<Store> Stores::getStdStore(int store)
{
	return getStdStores()[store];
}

weak_ptr<Store> Stores::getRawStore(int store)
{
	return getRawStores()[store];
}

vector<weak_ptr<Store>> Stores::getRawStores()
{
	vector<weak_ptr<Store>> res;
	for (auto& s : stores) {
		if (s->raw)
			res.push_back(s);
	}
	return res;
}

vector<weak_ptr<Store>> Stores::getStdStores()
{
	vector<weak_ptr<Store>> res;
	for (auto& s : stores) {
		if (!s->raw)
			res.push_back(s);
	}
	return res;
}

Stores::~Stores() {
}
