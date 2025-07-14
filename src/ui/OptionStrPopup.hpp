#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// specify parameters for the setup function in the Popup<...> template
class OptionStrPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override;
    void onClose(CCObject*) override;
    virtual ~OptionStrPopup();

    TextInput* m_inputOptTxt;

public:
    static OptionStrPopup* create(std::string const& text);

    void clickImport(CCObject*);
};

