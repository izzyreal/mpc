#include "ShortName.hpp"

#include <StrUtil.hpp>

using namespace mpc::disk;
using namespace std;

void ShortName::init() const
{
    // ASCII_ = ::java::nio::charset::Charset::forName("ASCII"_j);
    ILLEGAL_CHARS_ = vector<char>(
        {34, 42, 43, 44, 46, 47, 58, 59, 60, 61, 62, 63, 91, 92, 93, 124});
}

ShortName::ShortName(string nameExt)
{
    if (nameExt.length() > 12)
    {
        string error = "name too long";
        return;
    }

    size_t i = nameExt.find('.');
    string nameString;
    string extString;
    if (i == string::npos)
    {
        nameString = StrUtil::toUpper(nameExt);
        extString = "";
    }
    else
    {
        nameString = StrUtil::toUpper(nameExt.substr(0, i - 0));
        extString = StrUtil::toUpper(nameExt.substr(i + 1));
    }
    this->nameBytes = toCharArray_(nameString, extString);
    checkValidChars(nameBytes);
}

ShortName::ShortName(const string &name, const string &ext)
{
    this->nameBytes = toCharArray_(name, ext);
}

/*
charset::Charset*& ShortName::ASCII()
{
    return ASCII_;
}
*/
// charset::Charset* ShortName::ASCII_;

vector<char> ShortName::ILLEGAL_CHARS()
{
    // init();
    return ILLEGAL_CHARS_;
}
vector<char> ShortName::ILLEGAL_CHARS_;

const char ShortName::ASCII_SPACE;

ShortName ShortName::DOT()
{
    return DOT_;
}
ShortName ShortName::DOT_ = ShortName(".", "");

ShortName ShortName::DOT_DOT()
{
    return DOT_DOT_;
}
ShortName ShortName::DOT_DOT_ = ShortName("..", "");

vector<char> ShortName::toCharArray_(const string &name, const string &ext)
{
    checkValidName(name);
    checkValidExt(ext);
    vector<char> result = vector<char>(11);
    // Arrays::fill(result, ASCII_SPACE);
    // arraycopy(name->getBytes(ASCII_), 0, result, 0, name.length());
    // arraycopy(ext->getBytes(ASCII_), 0, result, 8, ext.length());

    for (int i = 0; i < 8; i++)
    {
        if (i >= name.length())
        {
            result[i] = ASCII_SPACE;
        }
        else
        {
            result[i] = name[i];
        }
    }

    for (int i = 8; i < 11; i++)
    {
        if (i - 8 >= ext.length())
        {
            result[i] = ASCII_SPACE;
        }
        else
        {
            result[i] = ext[i - 8];
        }
    }
    return result;
}

int8_t ShortName::checkSum() const
{
    auto const dest = new vector<char>(11);
    for (auto i = 0; i < 11; i++)
    {
        (*dest)[i] = static_cast<int8_t>(nameBytes[i]);
    }

    int sum = (*dest)[0];
    for (auto i = 1; i < 11; i++)
    {
        sum = (*dest)[i] + (((sum & 1) << 7) + ((sum & 254) >> 1));
    }
    return static_cast<int8_t>(sum & 255);
}

ShortName ShortName::get(const string &name)
{
    if (name.compare(".") == 0)
    {
        return DOT_;
    }
    if (name.compare("..") == 0)
    {
        return DOT_DOT_;
    }
    return ShortName(name);
}

bool ShortName::canConvert(const string &nameExt)
{
    try
    {
        get(nameExt);
        return true;
    }
    catch (exception e)
    {
        return false;
    }
}

ShortName ShortName::parse(vector<char> data)
{
    /*
    auto const nameArr = new ::char16_tArray(8);
    for (auto i = 0; i < npc(nameArr)->length; i++) {
            (*nameArr)[i] = static_cast< char16_t >(LittleEndian::getUInt8(data,
    i));
    }
    if(LittleEndian::getUInt8(data, 0) == 5) {
            (*nameArr)[0] = static_cast< char16_t >(229);
    }
    auto const extArr = new ::char16_tArray(3);
    for (auto i = 0; i < npc(extArr)->length; i++) {
            (*extArr)[i] = static_cast< char16_t >(LittleEndian::getUInt8(data,
    8 + i));
    }
    return new ShortName((new ::String(nameArr))->trim(), (new
    ::String(extArr))->trim());
    */
    return ShortName("parsedum", "my");
}

void ShortName::write(vector<char> dest) const
{
    // arraycopy(nameBytes, 0, dest, 0, nameBytes.size());
}

string ShortName::asSimpleString() const
{
    // auto const name = (new ::String(this->nameBytes, 0, 8, ASCII_))->trim();
    // auto const ext = (new ::String(this->nameBytes, 8, 3, ASCII_))->trim();
    // return npc(ext)->isEmpty() ? name :
    // ::StringBuilder().append(name)->append("."_j)
    //     ->append(ext)->toString();
    string name = "";
    for (int i = 0; i < 8; i++)
    {
        name.push_back(nameBytes[i]);
    }
    name = StrUtil::trim(name);
    string ext = "";
    for (int i = 0; i < 3; i++)
    {
        ext.push_back(nameBytes[i + 8]);
    }
    ext = StrUtil::trim(ext);
    string result = name;
    if (ext.length() != 0)
    {
        result = name + "." + ext;
    }
    return result;
}

void ShortName::checkValidName(const string &name)
{
    checkString(name, "name", 1, 8);
}

void ShortName::checkValidExt(const string &ext)
{
    checkString(ext, "extension", 0, 3);
}

void ShortName::checkString(string str, string strType, int minLength,
                            int maxLength)
{
    /*
    if(str == nullptr)
            throw new
    ::IllegalArgumentException(::StringBuilder().append(strType)->append(" is
    null"_j);

    if(npc(str)->length() < minLength)
            throw new
    ::IllegalArgumentException(::StringBuilder().append(strType)->append(" must
    have at least "_j)
                    ->append(minLength)
                    ->append(" characters: "_j)
                    ->append(str);

    if(npc(str)->length() > maxLength)
            throw new
    ::IllegalArgumentException(::StringBuilder().append(strType)->append(" has
    more than "_j)
                    ->append(maxLength)
                    ->append(" characters: "_j)
                    ->append(str);
    */
}

int ShortName::hashCode() const
{
    // return Arrays::hashCode(this->nameBytes);
    return 0;
}

void ShortName::checkValidChars(const vector<char> &chars)
{
    if (chars[0] == 32)
    {
        string error = "0x20 can not be the first character";
        return;
    }

    for (int i = 0; i < chars.size(); i++)
    {
        if ((chars[i] & 255) != chars[i])
        {
            string error = "multi-byte character at " + to_string(i);
            return;
        }

        char toTest = chars[i] & 255;
        if (toTest < 32 && toTest != 5)
        {
            string error = "character < 0x20 at" + to_string(i);
        }

        for (int j = 0; j < ILLEGAL_CHARS_.size(); j++)
        {
            if (toTest == ILLEGAL_CHARS_[j])
            {
                string error = "illegal character " +
                               string({ILLEGAL_CHARS_[j]}) + " at " +
                               to_string(i);
                return;
            }
        }
    }
}
