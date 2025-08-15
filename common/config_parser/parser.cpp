#include "parser.h"

namespace hft_ns
{
ConfigParser::ErrorCode ConfigParser::Parse(const std::string &file_path)
{
    std::ifstream input_stream(file_path);

    if (!input_stream)
    {
        return ErrorCode::ErrorLoadingFile;
    }

    std::string line;
    std::string current_category = "_GLOBAL_";

    while (getline(input_stream, line))
    {
        Trim(line);
        if (line.length() == 0)
            continue;

        std::string word;

        if (line[0] == '[')
        {
            if (line.back() != ']')
            {
                return ErrorCode::InvalidFormat;
            }
            // This word is a category header

            for (auto i = line.begin() + 1; i < line.end() - 1; i++)
            {
                word += *i;
            }

            current_category = word;
            Trim(current_category);
        }
        else
        {
            std::string key;
            bool        was_equal_found = false;
            for (const char &c : line)
            {
                if (c == '=')
                {
                    key             = std::move(word);
                    was_equal_found = true;
                    word.clear(); // to set the default value
                }
                else
                {
                    word += c;
                }
            }

            if (!was_equal_found)
            {
                return ErrorCode::InvalidFormat;
            }

            Trim(key);
            Trim(word);
            state_[current_category][key] = word;
        }
        word.clear();
    }
    return ErrorCode::NoError;
}

const std::optional<std::string> ConfigParser::GetValue(const std::string &key, const std::string &category) const
{
    std::optional<std::string> value = std::nullopt;

    auto category_itr = state_.find(category);
    if (category_itr == state_.end())
    {
        return std::nullopt;
    }
    auto data_itr = category_itr->second.find(key);
    if (data_itr == category_itr->second.end())
    {
        return std::nullopt;
    }

    value = data_itr->second;
    return value;
}

void ConfigParser::Trim(std::string &data)
{
    auto first_non_space_character_itr = data.begin();
    while (first_non_space_character_itr != data.end() && *first_non_space_character_itr == ' ')
    {
        first_non_space_character_itr++;
    }

    if (first_non_space_character_itr == data.end())
    {
        // all spaces
        data.clear();
        return;
    }

    auto last_non_space_character_itr = data.end() - 1;

    while (last_non_space_character_itr > first_non_space_character_itr && *last_non_space_character_itr == ' ')
    {
        last_non_space_character_itr--;
    }

    data.erase(data.begin(), first_non_space_character_itr);
    data.erase(last_non_space_character_itr + 1, data.end());
}
} // namespace hft_ns