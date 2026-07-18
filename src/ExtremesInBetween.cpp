#include "ExtremesInBetween.hpp"
#include <jasmine/web.hpp>

using namespace geode::prelude;

std::map<int, LadderDemon> ExtremesInBetween::gddl;

$on_mod(Loaded) {
    DemonsInBetweenLoadedEvent().listen([](const std::map<int, LadderDemon>& gddl) {
        constexpr std::array difficulties = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 12, 13, 14, 14, 15, 15, 16, 17, 18, 19, 20,
            21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29
        };

        for (auto& [id, demon] : gddl) {
            if (demon.difficulty > 19) {
                int roundedTier = round(demon.tier);
                auto difficulty = roundedTier < difficulties.size() ? difficulties[roundedTier] : 29;
                if (difficulty > 20) ExtremesInBetween::gddl.emplace(demon.id, LadderDemon { demon.id, demon.tier, demon.enjoyment, difficulty });
            }
        }

        spawn(web::WebRequest().get("https://api.aredl.net/v2/api/aredl/levels"), [](web::WebResponse res) {
            if (!res.ok()) return log::error("Failed to fetch AREDL data: HTTP {}", res.code());

            auto arr = jasmine::web::getArray(res);
            if (arr.empty()) return log::error("Failed to fetch AREDL data: Empty response");

            auto id = arr[0].get<int>("level_id");
            if (!id.isOk()) return log::error("Failed to fetch AREDL data: Unexpected response format");

            if (auto it = ExtremesInBetween::gddl.find(id.unwrap()); it != ExtremesInBetween::gddl.end()) it->second.difficulty = 30;

            log::info("Loaded AREDL data");
        });
    }).leak();
}

LadderDemon* ExtremesInBetween::demonForLevel(int levelID) {
    auto demon = gddl.find(levelID);
    return demon != gddl.end() ? &demon->second : nullptr;
}
