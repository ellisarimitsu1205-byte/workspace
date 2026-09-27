#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <string_view>
#include <ranges>
#include <spanstream>

#include "MappedFile.hpp"
#include "ParseValues.hpp"
#include "StringHasher.hpp"
#include "TimeExecution.hpp"
#include "openAddressHashMap.hpp"

namespace {
    const std::string FileName = "gas_station.csv";

    struct Record
    {
        std::size_t Count{};
        int16_t Min{}, Max{};
        int64_t Sum{};
    };

    using StationRecords = OpenAddressingHashMap<
        std::string,
        Record,
        StringStringViewHasher>;

    void Version2()
    {
        MappedFile file(FileName);
        std::ispanstream stream(file.Data());
        StationRecords records;

        auto view = stream.span();
        auto begin = view.begin();
        while(begin != view.end())
        {
            const auto stationEnd = std::ranges::find(begin, std::unreachable_sentinel, ',');
            const std::string_view station{begin, stationEnd};
            begin = stationEnd + 1;

            const auto endLine = std::ranges::find(begin, std::unreachable_sentinel, '\n');
            const auto reading = ParseValues(begin);
            begin = endLine + 1;

            if(Record* existing = records.find(station))
            {
                existing->Count += 1;
                existing->Min = std::min(existing->Min, reading);
                existing->Max = std::max(existing->Max, reading);
                existing->Sum += reading;
            }
            else
            {
                records.insert(std::string(station), Record{1, reading, reading, reading});
            }
        }
    }
}

int main()
{
    TimeExecution(Version2);
    return 0;
}