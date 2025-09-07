#pragma once
#include <Geode/Geode.hpp>
#include "../utils/shared.hpp"

using namespace geode::prelude;

class GravHazOptPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override;
    void onToggle(CCObject* object);
    void onEnumDecrease(CCObject* object);
    void onEnumIncrease(CCObject* object);
    void save(CCObject*);

    Mod* mod = Mod::get();

    CCLabelBMFont* m_portalDiffSelected;
    CCLabelBMFont* m_portalInputSelected;

    CCLabelBMFont* m_spikeWavesizeSelected;

    int8_t m_gpIndex = mod->getSavedValue<int8_t>("opt-0-grav-portals");
    int8_t m_gpIndexLen = static_cast<int8_t>(JFPGen::DifficultiesLabel.size());
    int8_t m_piIndex = mod->getSavedValue<int8_t>("opt-0-portal-input-types");
    int8_t m_piIndexLen = static_cast<int8_t>(JFPGen::PortalInputsLabel.size());
    bool m_portalsToggled = mod->getSavedValue<bool>("opt-0-using-grav-portals");
    bool m_fakesToggled = mod->getSavedValue<bool>("opt-0-fake-grav-portals");
    bool m_upsideDownToggled = mod->getSavedValue<bool>("opt-0-grav-portal-start");
    bool m_portalsWithinSpams = mod->getSavedValue<bool>("opt-0-remove-portals-in-spams");

    int8_t m_spIndex = mod->getSavedValue<int8_t>("opt-0-spike-placement-types");
    int8_t m_spIndexLen = static_cast<int8_t>(JFPGen::PlacementBySizeLabel.size());
    bool m_spikesToggled = mod->getSavedValue<bool>("opt-0-corridor-spikes");
    bool m_fuzzToggled = mod->getSavedValue<bool>("opt-0-corridor-fuzz");
public:
    static GravHazOptPopup* create(std::string const& text);
};
