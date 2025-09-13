// Copyright 2025 GDSRWave
#include "ThemeSelect.hpp"
#include "../utils/StringGen.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/binding/CustomListView.hpp>

// Currently an unused UI element, with introduction planned for a future JFP
// update. Feel free to mess with it as you want. -M
ThemeSelectLayer* ThemeSelectLayer::create() {
    auto ret = new ThemeSelectLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool ThemeSelectLayer::init() {
    if (!CCLayer::init()) {
        return false;
    }

    CCArray* themes = CCArray::create();

    CustomListView* listView = CustomListView::create(
        themes, BoomListType::CustomSong, 220.0f, 356.0f);
    if (!listView) {
        return false;
    }
    listView->setPosition({0, 0});
    listView->setAnchorPoint({0.5f, 0.5f});
    listView->setAnchorPoint({0.5f, 0.5f});

    GJListLayer* listLayer = GJListLayer::create(
        listView, "", {191, 114, 62, 255}, 356.0, 240.0, 1.0f);
    listLayer->setID("theme-select-list-layer"_spr);
    this->addChild(listLayer);

    return true;
}

CCScene* ThemeSelectLayer::scene() {
    auto layer = ThemeSelectLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

ThemeSelectOuterMenu* ThemeSelectOuterMenu::create() {
    auto ret = new ThemeSelectOuterMenu();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool ThemeSelectOuterMenu::init() {
    if (!CCLayer::init()) {
        return false;
    }

    this->setID("theme-select-outer-menu"_spr);
    this->setContentSize({640, 480});
    this->setAnchorPoint({0.5f, 0.5f});
    this->setPosition({0, 0});

    auto bg = CCLayerColor::create({0, 0, 0, 150});
    this->addChild(bg);

    auto titleLabel = CCLabelBMFont::create("Select Theme", "bigFont.fnt");
    titleLabel->setPosition({320, 400});
    this->addChild(titleLabel);

    auto themeSelectLayer = ThemeSelectLayer::create();
    this->addChild(themeSelectLayer);

    return true;
}
