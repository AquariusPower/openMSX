// $Id$

#ifndef SDLFONT_HH
#define SDLFONT_HH

#include "Font.hh"
#include "noncopyable.hh"

struct SDL_Surface;

namespace openmsx {

class SDLFont : public Font, private noncopyable
{
public:
	SDLFont(const std::string& filename, SDL_Surface* surface);
	virtual ~SDLFont();

	virtual void drawText(const std::string& str, int x, int y, byte alpha);

private:
	SDL_Surface* outputScreen;
	SDL_Surface* fontSurface;
	SDL_Surface* workImage;
};

} // namespace openmsx

#endif
