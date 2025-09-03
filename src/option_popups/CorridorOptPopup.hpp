#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class CorridorOptPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override;
    void onToggle(CCObject* object);
    void save(CCObject*);

    TextInput* m_seedInput;
public:
    static CorridorOptPopup* create(std::string const& text);
};
