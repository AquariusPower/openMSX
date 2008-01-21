// $Id$

#ifndef SDLTEXTURE_HH
#define SDLTEXTURE_HH

#include "openmsx.hh"
#include "noncopyable.hh"
#include <string>

class SDL_Surface;

namespace openmsx {

class OutputSurface;

class SDLImage : private noncopyable
{
public:
	SDLImage(OutputSurface& output, const std::string& filename);
	SDLImage(OutputSurface& output, const std::string& filename,
	         double scaleFactor);
	SDLImage(OutputSurface& output, const std::string& filename,
	         unsigned width, unsigned height);
	SDLImage(OutputSurface& output,
	         unsigned width, unsigned height, byte alpha);
	~SDLImage();

	void draw(unsigned x, unsigned y, byte alpha = 255);

private:
	void init(const std::string& filename);

	OutputSurface& output;
	SDL_Surface* image;
	SDL_Surface* workImage;

public:
	static SDL_Surface* loadImage(const std::string& filename);
	static SDL_Surface* loadImage(const std::string& filename,
	                              double scaleFactor);
	static SDL_Surface* loadImage(const std::string& filename,
	                              unsigned width, unsigned height);
	static SDL_Surface* readImage(const std::string& filename);
	static SDL_Surface* scaleImage32(SDL_Surface* input,
	                                 unsigned width, unsigned height);
	static SDL_Surface* convertToDisplayFormat(SDL_Surface* input);
	static void zoomSurface(SDL_Surface* src, SDL_Surface* dst, bool smooth);
};

} // namespace openmsx

#endif
