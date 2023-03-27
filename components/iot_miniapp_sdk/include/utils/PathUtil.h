#pragma once
#include <string>
#include <vector>

#define FILE_PATH_SEP_UNIX '/'
#define FILE_PATH_SEP_WIN '\\'
#define FILE_PATH_SEP_ARRARY_SIZE 2

#ifdef _WIN32
    #define FILE_PATH_SEP FILE_PATH_SEP_WIN
    #define FILE_PATH_ALTSEP FILE_PATH_SEP_UNIX
#else
    #define FILE_PATH_SEP FILE_PATH_SEP_UNIX
    #define FILE_PATH_ALTSEP FILE_PATH_SEP_WIN
#endif

namespace JQuick {

namespace _file {
inline std::string const& to_string(std::string const& s) { return s; }
}

template<typename... Args>
std::string pathjoin(Args const&... args)
{
    std::string result;
    using JQuick::_file::to_string;
    using std::to_string;
    int unpack[]{0, (
            (result.size() == 0 || *result.rbegin() == FILE_PATH_SEP_UNIX
                || *result.rbegin() == FILE_PATH_SEP_WIN) ?
            (result.append(to_string(args)))
            : (result.append(FILE_PATH_SEP + to_string(args)))
    , 0)...};
    static_cast<void>(unpack);
    return result;
}

std::string pathjoin(const std::vector<std::string> &slices);

std::string basename(const std::string &path);

std::string dirname(const std::string &path);

std::string realpath(const std::string &fpath);

bool exists(const std::string &fname);

bool mkdirs(const std::string& dpath);

}  // namespace JQuick
