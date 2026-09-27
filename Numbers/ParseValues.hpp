#pragma once

#include <span>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>


consteval auto Generate2dDecisionMatrix()
{
    std::array<std::array<int16_t, 2>, std::numeric_limits<unsigned char>::max() + 1> decisions;
    for (auto i{256uz}; i--;)
    {
        if( i >= '0' && i <= '9')
        {
            decisions[i][0] = i - '0';
            decisions[i][1] = 10;
        }
        else 
        {
            decisions[i][0] = 0;
            decisions[i][1] = 1;
        }   
    }
    return decisions;
}

constexpr static auto DecisionMatrix2d = Generate2dDecisionMatrix();
inline int16_t ParseValues(std::span<const char>::iterator& iterator)
{
    const bool isNegative = (*iterator == '-');
    if (isNegative)
    {
        ++iterator;
    }
    
    int16_t result{};
    bool afterDecimal{};
    std::size_t fractionalDigits{};

    while(*iterator != '\n')
    {
        const char character = *iterator++;
        if(character == '.')
        {
            afterDecimal = true;
            continue;
        }

        const auto decision = DecisionMatrix2d[static_cast<unsigned char>(character)];
        result = static_cast<int16_t>(result * decision[1] + decision[0]);
        if(afterDecimal && decision[1] == 10)
        {
            ++fractionalDigits;
        }
    }

    while(fractionalDigits < 2)
    {
        result = static_cast<int16_t>(result * 10);
        ++fractionalDigits;
    }

    return isNegative ? static_cast<int16_t>(-result) : result;
}