// $Id$

/*  Written By: Garrett Banuk <mongoose@mongeese.org>
 *  Adapted to C++ and openMSX needs by David Heremans
 *  This is free, just be sure to give me credit when using it
 *  in any of your programs.
 */

#include "SDLConsole.hh"
#include "SDLImage.hh"
#include "SDLFont.hh"
#include "Console.hh"
#include "File.hh"
#include "Timer.hh"
#include "MSXException.hh"
#include <SDL.h>

using std::string;

namespace openmsx {

SDLConsole::SDLConsole(Reactor& reactor, SDL_Surface* screen)
	: OSDConsoleRenderer(reactor)
	, outputScreen(screen)
{
	initConsole();
}

void SDLConsole::updateConsoleRect()
{
	if (OSDConsoleRenderer::updateConsoleRect()) {
		try {
			loadBackground(backgroundName);
		} catch (MSXException& e) {
			// ignore
		}
	}
}

void SDLConsole::paint()
{
	byte visibility = getVisibility();
	if (!visibility) return;

	updateConsoleRect();

	// draw the background image if there is one
	if (!backgroundImage.get()) {
		// no background image, try to create an empty one
		try {
			backgroundImage.reset(new SDLImage(outputScreen,
				destW, destH, CONSOLE_ALPHA));
		} catch (MSXException& e) {
			// nothing
		}
	}
	if (backgroundImage.get()) {
		backgroundImage->draw(destX, destY, visibility);
	}

	Console& console = getConsole();
	int screenlines = destH / font->getHeight();
	for (int loop = 0; loop < screenlines; ++loop) {
		int num = loop + console.getScrollBack();
		font->drawText(console.getLine(num),
			destX + CHAR_BORDER,
			destY + destH - (1 + loop) * font->getHeight(),
			visibility);
	}

	// Check if the blink period is over
	unsigned long long now = Timer::getTime();
	if (lastBlinkTime < now) {
		lastBlinkTime = now + BLINK_RATE;
		blink = !blink;
	}

	unsigned cursorX, cursorY;
	console.getCursorPosition(cursorX, cursorY);
	if ((cursorX != lastCursorX) || (cursorY != lastCursorY)) {
		blink = true; // force cursor
		lastBlinkTime = now + BLINK_RATE; // maximum time
		lastCursorX = cursorX;
		lastCursorY = cursorY;
	}
	if (console.getScrollBack() == 0) {
		if (blink) {
			font->drawText(string("_"),
				destX + CHAR_BORDER + cursorX * font->getWidth(),
				destY + destH - (font->getHeight() * (cursorY + 1)),
				visibility);
		}
	}
}

const string& SDLConsole::getName()
{
	static const string NAME = "SDLConsole";
	return NAME;
}

// Adds background image to the console
void SDLConsole::loadBackground(const string& filename)
{
	if (filename.empty()) {
		backgroundImage.reset();
		return;
	}
	backgroundImage.reset(
		new SDLImage(outputScreen, filename, destW, destH));
	backgroundName = filename;
}

void SDLConsole::loadFont(const string& filename)
{
	font.reset(new SDLFont(filename, outputScreen));
}

unsigned SDLConsole::getScreenW() const
{
	return outputScreen->w;
}

unsigned SDLConsole::getScreenH() const
{
	return outputScreen->h;
}

} // namespace openmsx
