#include "../ExtremesInBetween.hpp"
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/binding/GJDifficultySprite.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/loader/GameEvent.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <jasmine/hook.hpp>

using namespace geode::prelude;

constexpr std::array difficulties = {
    "Unknown Demon", "Free Demon", "Peaceful Demon", "Simple Demon", "Easy Demon", "Casual Demon", "Mild Demon",
    "Medium Demon", "Normal Demon", "Moderate Demon", "Tricky Demon", "Hard Demon", "Harder Demon", "Tough Demon",
    "Wild Demon", "Insane Demon", "Cruel Demon", "Crazy Demon", "Bizarre Demon", "Brutal Demon", "Extreme Demon",
    "Sapphire Demon", "Jade Demon", "Emerald Demon", "Ruby Demon", "Diamond Demon", "Pearl Demon", "Onyx Demon",
    "Amethyst Demon", "Azurite Demon", "Obsidian Demon"
};
constexpr std::array originalDifficulties = {
    "Hard Demon", "Unknown Demon", "Unknown Demon", "Easy Demon", "Medium Demon", "Insane Demon", "Extreme Demon"
};

class $modify(EIBLevelInfoLayer, LevelInfoLayer) {
    static void onModify(ModifyBase<ModifyDerive<EIBLevelInfoLayer, LevelInfoLayer>>& self) {
        (void)self.setHookPriority("LevelInfoLayer::init", -50);
    }

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (level->m_stars.value() < 10) return true;

        auto demon = ExtremesInBetween::demonForLevel(level->m_levelID.value());
        if (!demon) return true;

        if (auto menu = getChildByID("left-side-menu")) {
            if (auto button = static_cast<CCMenuItemSprite*>(menu->getChildByID("hiimjustin000.demons_in_between/demon-info-button"))) {
                static_cast<CCSprite*>(static_cast<CircleButtonSprite*>(button->getNormalImage())->getTopNode())->setDisplayFrame(
                    CCSpriteFrameCache::get()->spriteFrameByName(fmt::format("DIB_{:02d}_001.png"_spr, demon->difficulty).c_str()));
                button->setTarget(this, menu_selector(EIBLevelInfoLayer::onDemonInfo));
            }
        }

        if (auto sprite = static_cast<CCSprite*>(getChildByID("hiimjustin000.demons_in_between/between-difficulty-sprite"))) {
            sprite->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(
                fmt::format("DIB_{:02d}_btn2_001.png"_spr, demon->difficulty).c_str()));
            sprite->setPosition(sprite->getPosition() + CCPoint { 0.25f, 2.75f });
        }

        return true;
    }

    void onDemonInfo(CCObject* sender) {
        auto demon = ExtremesInBetween::demonForLevel(m_level->m_levelID.value());
        if (!demon) return;

        FLAlertLayer::create("Demon Info", fmt::format(
            "<cy>{}</c>\n"
            "<cg>Tier</c>: {}\n"
            "<cl>Enjoyment</c>: {}\n"
            "<cp>Difficulty</c>: {}\n"
            "<co>Original Difficulty</c>: {}",
            GEODE_ANDROID(std::string)(m_level->m_levelName),
            round(demon->tier * 100.0) / 100.0,
            demon->enjoyment >= 0.0 ? fmt::to_string(round(demon->enjoyment * 100.0) / 100.0) : "N/A",
            demon->difficulty < difficulties.size() ? difficulties[demon->difficulty] : "Unknown Demon",
            m_level->m_demonDifficulty < originalDifficulties.size() ? originalDifficulties[m_level->m_demonDifficulty] : "Unknown Demon"
        ), "OK")->show();
    }
};
