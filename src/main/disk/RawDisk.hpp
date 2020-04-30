#pragma once
#include <disk/AbstractDisk.hpp>

#include <file/File.hpp>

using namespace moduru::file;

namespace mpc {
	
	namespace disk {

		class Store;
		class MpcFile;

		namespace device {
			class Device;
		}

		class RawDisk
			: public AbstractDisk
		{

		public:
			typedef AbstractDisk super;

		private:
			//::java::util::List* path{};
			//::de::waldheinz::fs::fat::AkaiFatLfnDirectory* root{};

		public:
			void initFiles() override;

		private:
			void initParentFiles();

		protected:
            int getPathDepth() override;

		public:
			std::string getDirectoryName() override;
			bool moveBack() override;
			bool moveForward(std::string directoryName) override;

		private:
			//::de::waldheinz::fs::fat::AkaiFatLfnDirectory* getDir();
			//::de::waldheinz::fs::fat::AkaiFatLfnDirectory* getParentDir();
			void refreshPath();

		public:
			//static File* entryToFile(::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* entry);
			bool deleteDir(MpcFile* f) override;

		private:
			//bool deleteFilesRecursive(::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* entry);
			//bool deleteDirsRecursive(::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* entry, bool checkExist);

		public:
			bool deleteAllFiles(int dwGuiDelete) override;
			bool newFolder(std::string newDirName) override;
			MpcFile* newFile(std::string newFileName) override;
			std::string getAbsolutePath() override;

			RawDisk(std::weak_ptr<Store> store);

		};

	}
}
