// $Id$

#ifndef SDLRASTERIZER_HH
#define SDLRASTERIZER_HH

#include "Rasterizer.hh"
#include "Observer.hh"
#include "openmsx.hh"
#include "noncopyable.hh"
#include <memory>

namespace openmsx {

class Display;
class VDP;
class VDPVRAM;
class OutputSurface;
class VisibleSurface;
class RawFrame;
class RenderSettings;
class Setting;
class PostProcessor;
template <class Pixel> class CharacterConverter;
template <class Pixel> class BitmapConverter;
template <class Pixel> class SpriteConverter;

/** Rasterizer using a frame buffer approach: it writes pixels to a single
  * rectangular pixel buffer.
  */
template <class Pixel>
class SDLRasterizer : public Rasterizer, private noncopyable,
                      private Observer<Setting>
{
public:
	SDLRasterizer(
		VDP& vdp, Display& display, VisibleSurface& screen,
		std::auto_ptr<PostProcessor> postProcessor);
	virtual ~SDLRasterizer();

	// Rasterizer interface:
	virtual bool isActive();
	virtual void reset();
	virtual void frameStart(const EmuTime& time);
	virtual void frameEnd();
	virtual void setDisplayMode(DisplayMode mode);
	virtual void setPalette(int index, int grb);
	virtual void setBackgroundColour(int index);
	virtual void setTransparency(bool enabled);
	virtual void updateVRAMCache(int address);
	virtual void drawBorder(int fromX, int fromY, int limitX, int limitY);
	virtual void drawDisplay(
		int fromX, int fromY,
		int displayX, int displayY,
		int displayWidth, int displayHeight);
	virtual void drawSprites(
		int fromX, int fromY,
		int displayX, int displayY,
		int displayWidth, int displayHeight);
	virtual bool isRecording() const;

private:
	/** Translate from absolute VDP coordinates to screen coordinates:
	  * Note: In reality, there are only 569.5 visible pixels on a line.
	  *       Because it looks better, the borders are extended to 640.
	  * @param absoluteX Absolute VDP coordinate.
	  * @param narrow Is this a narrow (512 pixels wide) display mode?
	  */
	inline static int translateX(int absoluteX, bool narrow);

	inline void renderBitmapLine(Pixel* buf, unsigned vramLine);

	/** Get the pixel colour of a graphics 7 colour index.
	  */
	inline Pixel graphic7Colour(byte index);

	/** Reload entire palette from VDP.
	  */
	void resetPalette();

	/** Precalc palette values.
	  * For MSX1 VDPs, results go directly into palFg/palBg.
	  * For higher VDPs, results go into V9938_COLOURS and V9958_COLOURS.
	  */
	void precalcPalette();

	/** Precalc foreground colour index 0 (palFg[0]).
	  * @param mode Current display mode.
	  * @param transparency True iff transparency is enabled.
	  */
	void precalcColourIndex0(DisplayMode mode, bool transparency,
	                         byte bgcolorIndex);

	// Observer<Setting>
	virtual void update(const Setting& setting);

	/** The VDP of which the video output is being rendered.
	  */
	VDP& vdp;

	/** The VRAM whose contents are rendered.
	  */
	VDPVRAM& vram;

	/** Line to render at top of display.
	  * After all, our screen is 240 lines while display is 262 or 313.
	  */
	int lineRenderTop;

	/** Host colours corresponding to each VDP palette entry.
	  * palFg has entry 0 set to the current background colour.
	  *       The 16 first entries are for even pixels, the next 16 are for
	  *       odd pixels. Second part is only needed (and guaranteed to be
	  *       up-to-date) in Graphics5 mode.
	  * palBg has entry 0 set to black.
	  */
	Pixel palFg[16 * 2], palBg[16];

	/** Host colours corresponding to each Graphic 7 sprite colour.
	  */
	Pixel palGraphic7Sprites[16];

	/** Precalculated host colours corresponding to each possible V9938 colour.
	  * Used by updatePalette to adjust palFg and palBg.
	  */
	Pixel V9938_COLOURS[8][8][8];

	/** Host colours corresponding to the 256 colour palette of Graphic7.
	  * Used by BitmapConverter.
	  */
	Pixel PALETTE256[256];

	/** Host colours corresponding to each possible V9958 colour.
	  */
	Pixel V9958_COLOURS[32768];

	/** The surface which is visible to the user.
	  */
	OutputSurface& screen;

	/** The video post processor which displays the frames produced by this
	  *  rasterizer.
	  */
	std::auto_ptr<PostProcessor> postProcessor;

	/** The next frame as it is delivered by the VDP, work in progress.
	  */
	RawFrame* workFrame;

	/** The current renderer settings (gamma, brightness, contrast)
	  */
	RenderSettings& renderSettings;

	/** VRAM to pixels converter for character display modes.
	  */
	std::auto_ptr<CharacterConverter<Pixel> > characterConverter;

	/** VRAM to pixels converter for bitmap display modes.
	  */
	std::auto_ptr<BitmapConverter<Pixel> > bitmapConverter;

	/** VRAM to pixels converter for sprites.
	  */
	std::auto_ptr<SpriteConverter<Pixel> > spriteConverter;
};

} // namespace openmsx

#endif
