#pragma once
#include <observer/Observable.hpp>
#include <disk/device/Device.hpp>
#include <disk/SoundSaver.hpp>

#include <vector>
#include <string>

namespace mpc::sequencer {
	class Sequence;
}

namespace mpc::sampler {
	class Program;
	class Sound;
}

namespace mpc::ui::disk {
	class DiskGui;
}

namespace mpc::disk {
	class MpcFile;
	class Store;
}

namespace mpc::disk {

	class AbstractDisk
		: public moduru::observer::Observable
	{

	private:
		bool busy{ false };
		std::weak_ptr<Store> store{};
		std::unique_ptr<SoundSaver> soundSaver;

	protected:
		mpc::ui::disk::DiskGui* diskGui;
		std::unique_ptr<mpc::disk::device::Device> device{};

	public:
		std::vector<std::string> extensions{};
		std::vector<MpcFile*> files{};
		std::vector<MpcFile*> allFiles{};
		std::vector<MpcFile*> parentFiles{};

	public:
		static std::string formatFileSize(int size);
		static std::string padFileName16(std::string s);
		MpcFile* getFile(int i);
		std::vector<std::string> getFileNames();
		std::string getFileName(int i);
		std::vector<std::string> getParentFileNames();
		bool renameSelectedFile(std::string s);
		bool deleteSelectedFile();
		std::vector<MpcFile*> getFiles();
		std::vector<MpcFile*> getAllFiles(); // used by those who want to disregard ext filters
		MpcFile* getParentFile(int i);
		std::vector<MpcFile*> getParentFiles();
		void writeSound(std::weak_ptr<mpc::sampler::Sound> s);
		void writeWav(std::weak_ptr<mpc::sampler::Sound> s);
		void writeSound(mpc::sampler::Sound* s, MpcFile* f);
		void writeWav(mpc::sampler::Sound* s, MpcFile* f);
		void writeSequence(mpc::sequencer::Sequence* s, std::string fileName);
		bool checkExists(std::string fileName);
		MpcFile* getFile(std::string fileName);
		void writeProgram(mpc::sampler::Program* program, std::string fileName);
		std::weak_ptr<mpc::disk::Store> getStore();

		void setBusy(bool b);
		bool isBusy();
		bool isDirectory(MpcFile* f);
		void flush();
		void close();
		bool isRoot();

		virtual void initFiles() = 0;
		virtual MpcFile* newFile(std::string name) = 0;
		virtual std::string getDirectoryName() = 0;
		virtual bool moveBack() = 0;
		virtual bool moveForward(std::string directoryName) = 0;
		virtual bool deleteAllFiles(int dwGuiDelete) = 0;
		virtual bool newFolder(std::string newDirName) = 0;
		virtual bool deleteDir(MpcFile* f) = 0;
		virtual std::string getAbsolutePath() = 0;

	protected:
		virtual int getPathDepth() = 0;

	protected:
		AbstractDisk(std::weak_ptr<Store> store);
		virtual ~AbstractDisk();

	};
}
