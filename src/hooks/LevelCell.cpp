#include "../ExtremesInBetween.hpp"
#include <Geode/binding/GJDifficultySprite.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <jasmine/hook.hpp>
#include <jasmine/setting.hpp>

using namespace geode::prelude;

class $modify(EIBLevelCell, LevelCell) {
    static void onModify(ModifyBase<ModifyDerive<EIBLevelCell, LevelCell>>& self) {
        auto mod = Loader::get()->getInstalledMod("hiimjustin000.demons_in_between");
        if (auto hook = jasmine::hook::get(self.m_hooks, "LevelCell::loadFromLevel", mod && mod->getSettingValue<bool>("enable-difficulties"))) {
            hook->setPriority(-10);

            SettingChangedEventV3("hiimjustin000.demons_in_between", "enable-difficulties").listen([hook](std::shared_ptr<SettingV3> setting) {
                jasmine::hook::toggle(hook, std::static_pointer_cast<BoolSettingV3>(std::move(setting))->getValue());
            }).leak();
        }
    }

    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        if (level->m_stars.value() < 10) return;

        auto difficultyContainer = m_mainLayer->getChildByID("difficulty-container");
        if (!difficultyContainer) difficultyContainer = m_mainLayer->getChildByID("grd-demon-icon-layer");
        if (!difficultyContainer) return;

        auto demon = ExtremesInBetween::demonForLevel(level->m_levelID.value());
        if (!demon) return;

        auto difficultySprite = static_cast<GJDifficultySprite*>(difficultyContainer->getChildByID("difficulty-sprite"));
        if (!difficultySprite) return;

        if (auto sprite = static_cast<CCSprite*>(difficultyContainer->getChildByID("hiimjustin000.demons_in_between/between-difficulty-sprite"))) {
            sprite->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(
                fmt::format("DIB_{:02d}_btn_001.png"_spr, demon->difficulty).c_str()));
            sprite->setPosition(difficultySprite->getPosition() + CCPoint { 0.25f, -0.75f });
        }
        else {
            auto grdDifficulty = difficultyContainer->getChildByID("grd-difficulty");
            if (!grdDifficulty) return;

            sprite = CCSprite::createWithSpriteFrameName(fmt::format("DIB_{:02d}_btn_001.png"_spr, demon->difficulty).c_str());
            sprite->setPosition(difficultySprite->getPosition() + CCPoint { 0.25f, -0.75f });
            sprite->setID("hiimjustin000.demons_in_between/between-difficulty-sprite");
            difficultyContainer->addChild(sprite, 3);

            grdDifficulty->setVisible(false);
            if (auto grdInfinity = difficultyContainer->getChildByID("grd-infinity")) grdInfinity->setVisible(false);
        }
    }
};
