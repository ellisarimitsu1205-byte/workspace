#pragma once

#include <string>
#include <string_view>
#include <functional>   

struct StringStringViewHasher
{
    using is_transparent = void;

    std::size_t operator()(std::string_view str) const
    {
        return std::hash<std::string_view>{}(str);
    }
    std::size_t operator()(const std::string& str) const
    {
        return std::hash<std::string_view>{}(str);
    }
};