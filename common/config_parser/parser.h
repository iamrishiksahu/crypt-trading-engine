#pragma once
#include <fstream>
#include <optional>
#include <string>
#include <unordered_map>

namespace hft_ns
{
class ConfigParser
{
  public:
    enum class ErrorCode
    {
        NoError,
        InvalidFormat,
        ErrorLoadingFile,
        KeyNotFound,
        CategoryNotFound
    };

  public:
    ErrorCode Parse(const std::string &file_path);

    const std::optional<std::string> GetValue(const std::string &key, const std::string &category = "_GLOBAL_") const;

  private:
    void Trim(std::string &data);

  private:
    std::unordered_map<std::string /*Category*/, std::unordered_map<std::string /*Key*/, std::string /*Value*/>> state_;
};

inline std::ostream &operator<<(std::ostream &os, ConfigParser::ErrorCode ec)
{
    switch (ec)
    {
    case ConfigParser::ErrorCode::NoError:
        return os << "NoError";
    case ConfigParser::ErrorCode::InvalidFormat:
        return os << "InvalidFormat";
    case ConfigParser::ErrorCode::ErrorLoadingFile:
        return os << "ErrorLoadingFile";
    case ConfigParser::ErrorCode::KeyNotFound:
        return os << "KeyNotFound";
    case ConfigParser::ErrorCode::CategoryNotFound:
        return os << "CategoryNotFound";
    }
    return os << "UnknownError";
}
} // namespace hft_ns