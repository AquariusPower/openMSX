// $Id$

#include "IconLayer.hh"
#include "FileContext.hh"
#include "Timer.hh"
#include "SDLImage.hh"
#include "IntegerSetting.hh"
#include "FilenameSetting.hh"
#include "CliComm.hh"
#include "Display.hh"
#include "IconStatus.hh"
#include "components.hh"
#ifdef COMPONENT_GL
#include "GLImage.hh"
#endif
#include <SDL.h>

using std::string;

namespace openmsx {

template <class IMAGE>
class IconSettingChecker : public SettingChecker<FilenameSetting::Policy>
{
public:
	IconSettingChecker(IconLayer<IMAGE>& iconLayer);
	virtual void check(SettingImpl<FilenameSetting::Policy>& setting,
	                   std::string& value);
private:
	IconLayer<IMAGE>& iconLayer;
};


template <class IMAGE>
IconLayer<IMAGE>::IconLayer(CommandController& commandController,
                            Display& display_, IconStatus& iconStatus_,
                            SDL_Surface* screen)
	// Just assume partial coverage and let paint() sort it out.
	: Layer(COVER_PARTIAL, Z_ICONS)
	, display(display_)
	, iconStatus(iconStatus_)
	, outputScreen(screen)
	, scaleFactor(outputScreen->w / 640.0)
	, iconSettingChecker(new IconSettingChecker<IMAGE>(*this))
{
	createSettings(commandController, LedEvent::POWER, "power");
	createSettings(commandController, LedEvent::CAPS,  "caps");
	createSettings(commandController, LedEvent::KANA,  "kana");
	createSettings(commandController, LedEvent::PAUSE, "pause");
	createSettings(commandController, LedEvent::TURBO, "turbo");
	createSettings(commandController, LedEvent::FDD,   "fdd");
}

template <class IMAGE>
void IconLayer<IMAGE>::createSettings(CommandController& commandController,
                                      LedEvent::Led led, const string& name)
{
	string icon_name = "icon." + name;
	ledInfo[led].xcoord.reset(new IntegerSetting(commandController,
		icon_name + ".xcoord", "X-coordinate for LED icon",
		((int)led) * 60, 0, 640));
	//Default is 640x480 and we want the default icons on the bottom
	ledInfo[led].ycoord.reset(new IntegerSetting(commandController,
		icon_name + ".ycoord", "Y-coordinate for LED icon",
		444, 0, 480));
	for (int i = 0; i < 2; ++i) {
		string tmp = icon_name + (i ? ".active" : ".non-active");
		ledInfo[led].name[i].reset(new FilenameSetting(
			commandController, tmp + ".image",
			"Image for active LED icon",
			"skins/set1/" +( i ?  name + "-on.png" : name + "-off.png")));
		ledInfo[led].fadeTime[i].reset(new IntegerSetting(
			commandController, tmp + ".fade-delay",
			"Time (in ms) after which the icons start to fade "
			"(0 means no fading)", 5000, 0, 1000000));
		ledInfo[led].fadeDuration[i].reset(new IntegerSetting(
			commandController, tmp + ".fade-duration",
			"Time (in ms) it takes for the icons the fade from "
			"completely opaque to completely transparent", 5000, 0, 1000000));

		try {
			ledInfo[led].name[i]->setChecker(iconSettingChecker.get());
		} catch (MSXException& e) {
			commandController.getCliComm().printWarning(e.getMessage());
		}
	}
}


template <class IMAGE>
IconLayer<IMAGE>::~IconLayer()
{
}

template <class IMAGE>
void IconLayer<IMAGE>::paint()
{
	for (int i = 0; i < LedEvent::NUM_LEDS; ++i) {
		LedInfo& led = ledInfo[i];
		int status = iconStatus.getStatus(i) ? 1 : 0;
		unsigned long long fadeTime =
			1000 * led.fadeTime[status]->getValue();
		unsigned long long fadeDuration =
			1000 * led.fadeDuration[status]->getValue();
		unsigned long long now = Timer::getTime();
		unsigned long long diff = now - iconStatus.getTime(i);
		byte alpha;
		if (fadeTime == 0) {
			// no fading, draw completely opaque
			alpha = 255;
		} else if (diff > (fadeTime + fadeDuration)) {
			// completely faded out, don't even bother to draw
			continue;
		} else if (diff < fadeTime) {
			// no fading yet
			alpha = 255;
			display.repaintDelayed(200000); // 5 fps
		} else {
			// fading out
			alpha = 255 - (255 * (diff - fadeTime) / fadeDuration);
			display.repaintDelayed(40000); // 25 fps
		}
		IMAGE* icon = led.icon[status].get();
		if (icon) {
			unsigned x = static_cast<unsigned>(
			                scaleFactor * led.xcoord->getValue());
			unsigned y = static_cast<unsigned>(
			                scaleFactor * led.ycoord->getValue());
			icon->draw(x, y, alpha);
		}
	}
}

template <class IMAGE>
const string& IconLayer<IMAGE>::getName()
{
	static const string NAME = "icon layer";
	return NAME;
}


// class IconSettingChecker

template <class IMAGE>
IconSettingChecker<IMAGE>::IconSettingChecker(IconLayer<IMAGE>& iconLayer_)
	: iconLayer(iconLayer_)
{
}

template <class IMAGE>
void IconSettingChecker<IMAGE>::check(SettingImpl<FilenameSetting::Policy>& setting,
                                      string& value)
{
	SystemFileContext context;
	for (int i = 0; i < LedEvent::NUM_LEDS; ++i) {
		for (int j = 0; j < 2; ++j) {
			if (&setting == iconLayer.ledInfo[i].name[j].get()) {
				if (value.empty()) {
					iconLayer.ledInfo[i].icon[j].reset();
				} else {
					iconLayer.ledInfo[i].icon[j].reset(
					    new IMAGE(iconLayer.outputScreen,
					              context.resolve(value),
					              iconLayer.scaleFactor));
				}
				break;
			}
		}
	}
}


// Force template instantiation
template class IconLayer<SDLImage>;
#ifdef COMPONENT_GL
template class IconLayer<GLImage>;
#endif

} // namespace openmsx
