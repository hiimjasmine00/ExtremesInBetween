#include "ExtremesInBetween.hpp"
#include <Geode/loader/GameEvent.hpp>
#include <jasmine/mod.hpp>
#include <jasmine/string.hpp>
#include <jasmine/web.hpp>

using namespace geode::prelude;
using namespace jasmine::mod;

std::map<int, LadderDemon> ExtremesInBetween::gddl;

$on_game(Loaded) {
    spawn(
        web::WebRequest().get("https://docs.google.com/spreadsheets/d/1qKlWKpDkOpU1ZF6V6xGfutDY2NvcA8MNPnsv6GBkKPQ/gviz/tq?tqx=out:csv&sheet=GDDL"),
        [](web::WebResponse res) {
            if (!res.ok()) return log::error("Failed to fetch extreme GDDL data: HTTP {}", res.code());

            constexpr std::array difficulties = {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 12, 13, 14, 14, 15, 15, 16, 17, 18, 19, 20,
                21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29
            };
            constexpr std::array mainLevels = { 0, 14, 18, 20 };

            auto str = jasmine::web::getString(res);
            auto index = str.find('\n');
            if (index == std::string::npos) index = str.size();
            auto keys = string::splitView(std::string_view(str.data() + 1, index - 2), "\",\"");
            for (auto line : jasmine::string::SplitIterator(std::string_view(str.data() + index + 1, str.size() - index - 1), "\n")) {
                auto values = string::splitView(line.substr(1, line.size() - 2), "\",\"");
                LadderDemon demon;
                for (size_t j = 0; j < keys.size() && j < values.size(); j++) {
                    auto key = keys[j];
                    auto value = values[j];
                    if (key == "ID") {
                        if (auto num = numFromString<int>(value)) demon.id = num.unwrap();
                        if (demon.id < mainLevels.size()) demon.id = mainLevels[demon.id];
                        if (demon.id < 1) break;
                    }
                    else if (key == "Tier") {
                        if (auto num = numFromString<double>(value)) demon.tier = num.unwrap();
                        int roundedTier = round(demon.tier);
                        demon.difficulty = roundedTier < difficulties.size() ? difficulties[roundedTier] : 29;
                        if (demon.difficulty < 1) break;
                    }
                    else if (key == "Enjoyment") {
                        if (auto num = numFromString<double>(value)) demon.enjoyment = num.unwrap();
                    }
                }
                if (demon.id > 0 && demon.difficulty > 20) ExtremesInBetween::gddl.emplace(demon.id, demon);
            }

            log::info("Loaded extreme GDDL data with {} demons", ExtremesInBetween::gddl.size());

            spawn(web::WebRequest().get("https://api.aredl.net/v2/api/aredl/levels"), [](web::WebResponse res) {
                if (!res.ok()) return log::error("Failed to fetch AREDL data: HTTP {}", res.code());

                auto arr = jasmine::web::getArray(res);
                if (arr.empty()) return log::error("Failed to fetch AREDL data: Empty response");

                auto id = arr[0].get<int>("level_id");
                if (!id.isOk()) return log::error("Failed to fetch AREDL data: Unexpected response format");

                if (auto it = ExtremesInBetween::gddl.find(id.unwrap()); it != ExtremesInBetween::gddl.end()) it->second.difficulty = 30;

                log::info("Loaded AREDL data");
            });
        }
    );
}

LadderDemon* ExtremesInBetween::demonForLevel(int levelID) {
    auto demon = gddl.find(levelID);
    return demon != gddl.end() ? &demon->second : nullptr;
}
