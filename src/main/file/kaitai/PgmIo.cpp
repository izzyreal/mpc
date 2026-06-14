#include "file/kaitai/PgmIo.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "disk/MpcFile.hpp"
#include "engine/IndivFxMixer.hpp"
#include "engine/StereoMixer.hpp"
#include "disk/PgmFileToProgramConverter.hpp"
#include "file/kaitai/KaitaiIoUtil.hpp"
#include "file/kaitai/generated/mpc2000xl_pgm.h"
#include "mpc_fs.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "file/pgmwriter/PgmWriter.hpp"

using namespace mpc::file::kaitai;

program_or_error PgmIo::loadProgram(mpc::Mpc &mpc,
                                    const std::shared_ptr<mpc::disk::MpcFile> &file,
                                    const std::shared_ptr<mpc::sampler::Program> &program,
                                    std::vector<std::string> &soundNames)
{
    const auto fileBytes = file->getBytes();
    const auto canonicalBytes = parseRewrite<mpc2000xl_pgm_t>(fileBytes);

    const auto tempDirRes = mpc_fs::temp_directory_path();
    if (!tempDirRes)
    {
        return tl::make_unexpected(mpc::disk::mpc_io_error_msg{"Unable to resolve temp directory"});
    }

    auto tempPath = *tempDirRes / file->getName();
    auto tempFile = std::make_shared<mpc::disk::MpcFile>(
        std::variant<mpc_fs::path, std::shared_ptr<akaifat::fat::AkaiFatLfnDirectoryEntry>>(tempPath)
    );

    auto mutableBytes = canonicalBytes;
    tempFile->setFileData(mutableBytes);
    const auto result =
        mpc::disk::PgmFileToProgramConverter::loadFromFileAndConvert(
            mpc, tempFile, program, soundNames);
    (void)mpc_fs::remove(tempPath);
    return result;
}

std::vector<char> PgmIo::saveProgram(mpc::sampler::Program &program,
                                     const std::weak_ptr<mpc::sampler::Sampler> &sampler)
{
    const mpc::file::pgmwriter::PgmWriter handwrittenWriter(&program, sampler);
    const auto handwrittenBytes = handwrittenWriter.get();
    return parseRewrite<mpc2000xl_pgm_t>(handwrittenBytes);
}
