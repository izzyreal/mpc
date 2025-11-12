#include "file/all/Defaults.hpp"

#include "Util.hpp"
#include "Mpc.hpp"
#include "file/all/AllParser.hpp"

#include "lcdgui/screens/UserScreen.hpp"

#include "file/ByteUtil.hpp"
#include <StrUtil.hpp>

using namespace mpc::file::all;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

std::vector<char> Defaults::UNKNOWN1 = {1, 0, 0, 1, 1, 0};
std::vector<char> Defaults::UNKNOWN2 = {
    0, 0, (char)0xFF, (char)0xFF, 1, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,
    0, 0, 0,          0,          0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,
    0, 0, 0,          0,          0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,
    0, 0, 0,          0,          0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,
    0, 0, 0,          0,          0, 0, 0, 0, 32, 32, 32, 32, 32, 32, 32, 32};

Defaults::Defaults(Mpc &mpc, const std::vector<char> &loadBytes) : mpc(mpc)
{
    parseNames(loadBytes);

    auto tempoBytes = std::vector{loadBytes[TEMPO_BYTE1_OFFSET],
                                  loadBytes[TEMPO_BYTE2_OFFSET]};
    tempo = ByteUtil::bytes2ushort(tempoBytes);
    timeSigNum = loadBytes[TIMESIG_NUM_OFFSET];
    timeSigDen = loadBytes[TIMESIG_DEN_OFFSET];

    auto barCountBytes = std::vector{loadBytes[BAR_COUNT_BYTE1_OFFSET],
                                     loadBytes[BAR_COUNT_BYTE2_OFFSET]};

    barCount = ByteUtil::bytes2ushort(barCountBytes);
    loopEnabled = loadBytes[LOOP_ENABLED_OFFSET] == 0x01;

    for (int i = 0; i < 64; i++)
    {
        devices[i] = loadBytes[DEVICES_OFFSET + i];
        busses[i] = loadBytes[BUSSES_OFFSET + i];
        pgms[i] = loadBytes[PGMS_OFFSET + i];
        trVelos[i] = loadBytes[TR_VELOS_OFFSET + i];
        status[i] = loadBytes[TR_STATUS_OFFSET + i];
    }
}

Defaults::Defaults(Mpc &mpc) : mpc(mpc)
{
    saveBytes = std::vector<char>(AllParser::DEFAULTS_LENGTH);

    setNames();

    for (int i = 0; i < UNKNOWN1.size(); i++)
    {
        saveBytes[UNKNOWN1_OFFSET + i] = UNKNOWN1[i];
    }

    setTempo();
    setTimeSig();
    setBarCount();
    setLastTick();

    auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
    auto lastBar = userScreen->lastBar;

    if (lastBar == 1)
    {
        saveBytes[LAST_TICK_BYTE1_OFFSET] = 0;
        saveBytes[LAST_TICK_BYTE2_OFFSET] = 0;
        lastBar = 0;
    }

    auto unknownNumberBytes = ByteUtil::uint2bytes((lastBar + 1) * 2000000);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            saveBytes[UNKNOWN32_BIT_INT_OFFSET + j + i * 4] =
                unknownNumberBytes[j];
        }
    }

    for (int i = 0; i < UNKNOWN2.size(); i++)
    {
        saveBytes[UNKNOWN2_OFFSET + i] = UNKNOWN2[i];
    }

    setTrackSettings();

    saveBytes[LOOP_ENABLED_OFFSET] = userScreen->loop ? 0x01 : 0x00;
}

void Defaults::parseNames(const std::vector<char> &loadBytes)
{
    std::vector<char> stringBuffer;

    stringBuffer =
        Util::vecCopyOfRange(loadBytes, DEF_SEQ_NAME_OFFSET,
                             DEF_SEQ_NAME_OFFSET + AllParser::NAME_LENGTH);

    defaultSeqName = "";

    for (char c : stringBuffer)
    {
        if (c == 0x00)
        {
            break;
        }

        defaultSeqName.push_back(c);
    }

    int offset;

    for (int i = 0; i < 33; i++)
    {
        offset = DEV_NAMES_OFFSET + i * AllParser::DEV_NAME_LENGTH;
        stringBuffer = Util::vecCopyOfRange(
            loadBytes, offset, offset + AllParser::DEV_NAME_LENGTH);
        std::string s;

        for (char c : stringBuffer)
        {
            if (c == 0x00)
            {
                break;
            }

            s.push_back(c);
        }

        devNames[i] = s;
    }

    for (int i = 0; i < 64; i++)
    {
        offset = TR_NAMES_OFFSET + i * AllParser::NAME_LENGTH;
        stringBuffer = Util::vecCopyOfRange(loadBytes, offset,
                                            offset + AllParser::NAME_LENGTH);
        std::string s;

        for (char c : stringBuffer)
        {
            if (c == 0x00)
            {
                break;
            }

            s.push_back(c);
        }
        trackNames[i] = s;
    }
}

std::string Defaults::getDefaultSeqName()
{
    return defaultSeqName;
}

int Defaults::getTempo() const
{
    return tempo;
}

int Defaults::getTimeSigNum() const
{
    return timeSigNum;
}

int Defaults::getTimeSigDen() const
{
    return timeSigDen;
}

int Defaults::getBarCount() const
{
    return barCount;
}

bool Defaults::isLoopEnabled() const
{
    return loopEnabled;
}

std::vector<std::string> Defaults::getDefaultDevNames()
{
    return devNames;
}

std::vector<std::string> Defaults::getDefaultTrackNames()
{
    return trackNames;
}

std::vector<int> Defaults::getDevices()
{
    return devices;
}

std::vector<int> Defaults::getBusses()
{
    return busses;
}

std::vector<int> Defaults::getPgms()
{
    return pgms;
}

std::vector<int> Defaults::getTrVelos()
{
    return trVelos;
}

void Defaults::setTrackSettings()
{
    auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
    for (int i = 0; i < 64; i++)
    {
        saveBytes[DEVICES_OFFSET + i] = userScreen->device;
        saveBytes[BUSSES_OFFSET + i] = userScreen->bus;
        saveBytes[PGMS_OFFSET + i] = userScreen->pgm;
        saveBytes[TR_VELOS_OFFSET + i] = userScreen->velo;
        saveBytes[TR_STATUS_OFFSET + i] = userScreen->getTrackStatus();
    }
}

void Defaults::setLastTick()
{
    auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
    auto lastTick = (userScreen->lastBar + 1) * 384;

    auto b = ByteUtil::ushort2bytes(lastTick);

    saveBytes[LAST_TICK_BYTE1_OFFSET] = b[0];
    saveBytes[LAST_TICK_BYTE2_OFFSET] = b[1];
}

void Defaults::setBarCount()
{
    auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
    auto ba = ByteUtil::ushort2bytes(userScreen->lastBar + 1);
    saveBytes[BAR_COUNT_BYTE1_OFFSET] = ba[0];
    saveBytes[BAR_COUNT_BYTE2_OFFSET] = ba[1];
}

void Defaults::setTimeSig()
{
    auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
    saveBytes[TIMESIG_NUM_OFFSET] = userScreen->timeSig.getNumerator();
    saveBytes[TIMESIG_DEN_OFFSET] = userScreen->timeSig.getDenominator();
}

void Defaults::setNames()
{
    auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
    auto const defSeqName = StrUtil::padRight(userScreen->sequenceName, " ",
                                              AllParser::NAME_LENGTH);

    for (int i = 0; i < 16; i++)
    {
        saveBytes[DEF_SEQ_NAME_OFFSET + i] = defSeqName[i];
    }

    std::string stringBuffer;

    for (int i = 0; i < 33; i++)
    {
        auto const defDevName = userScreen->getDeviceName(i);
        stringBuffer =
            StrUtil::padRight(defDevName, " ", AllParser::DEV_NAME_LENGTH);
        auto offset = DEV_NAMES_OFFSET + i * AllParser::DEV_NAME_LENGTH;

        for (int j = offset; j < offset + AllParser::DEV_NAME_LENGTH; j++)
        {
            saveBytes[j] = stringBuffer[j - offset];
        }
    }
    for (int i = 0; i < 64; i++)
    {
        auto const defTrackName = userScreen->getTrackName(i);
        stringBuffer =
            StrUtil::padRight(defTrackName, " ", AllParser::NAME_LENGTH);
        auto offset = TR_NAMES_OFFSET + i * AllParser::NAME_LENGTH;

        for (int j = offset; j < offset + AllParser::NAME_LENGTH; j++)
        {
            saveBytes[j] = stringBuffer[j - offset];
        }
    }
}

void Defaults::setTempo()
{
    auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
    auto tempoBytes =
        ByteUtil::ushort2bytes(static_cast<int>(userScreen->tempo * 10.0));
    saveBytes[TEMPO_BYTE1_OFFSET] = tempoBytes[0];
    saveBytes[TEMPO_BYTE2_OFFSET] = tempoBytes[1];
}

std::vector<char> &Defaults::getBytes()
{
    return saveBytes;
}
