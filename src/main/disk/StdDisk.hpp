#pragma once
#include <disk/AbstractDisk.hpp>
#include <file/File.hpp>

#include <memory>
#include <string>
#include <vector>

namespace moduru {
	namespace file {
		class File;
		class Directory;
		class FsNode;
	}
}

namespace mpc {
	class Mpc;
	namespace disk {

		namespace device {
			class Device;
		}

		class Store;
		class MpcFile;

		class StdDisk
			: public AbstractDisk
		{

		public:
			typedef AbstractDisk super;

		private:
			std::vector<std::weak_ptr<moduru::file::Directory>> path{};
			std::weak_ptr<moduru::file::Directory> root{};

		private:
			void initParentFiles();

		public:
			void initFiles() override;
			std::string getDirectoryName() override;
			bool moveBack() override;
			bool moveForward(std::string directoryName) override;
			std::weak_ptr<moduru::file::Directory> getDir();

		private:
			std::weak_ptr<moduru::file::Directory> getParentDir();
			void renameFilesToAkai();

		protected:
			int getPathDepth() override;

		public:
			bool deleteAllFiles(int dwGuiDelete) override;
			bool newFolder(std::string newDirName) override;
			bool deleteDir(MpcFile* f) override;

		private:
			bool deleteRecursive(moduru::file::FsNode* deleteMe);

		public:
			MpcFile* newFile(std::string newFileName) override;
			std::string getAbsolutePath() override;

			StdDisk(std::weak_ptr<Store> store, mpc::Mpc* mpc);
			~StdDisk();

		};

	}
}
