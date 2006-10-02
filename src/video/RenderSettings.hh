// $Id$

#ifndef RENDERSETTINGS_HH
#define RENDERSETTINGS_HH

#include "RendererFactory.hh"
#include "noncopyable.hh"
#include <string>
#include <memory>

namespace openmsx {

class CommandController;
class IntegerSetting;
class FloatSetting;
class BooleanSetting;
class StringSetting;
class VideoSourceSetting;
class ColorMatrixChecker;

/** Class containing all settings for renderers.
  * Keeping the settings here makes sure they are preserved when the user
  * switches to another renderer.
  */
class RenderSettings : private noncopyable
{
public:
	/** Render accuracy: granularity of the rendered area.
	  */
	enum Accuracy { ACC_SCREEN, ACC_LINE, ACC_PIXEL };

	/** Scaler algorithm
	  */
	enum ScaleAlgorithm {
		SCALER_SIMPLE, SCALER_SAI, SCALER_SCALE,
		SCALER_HQ, SCALER_HQLITE, SCALER_RGBTRIPLET, SCALER_TV
	};

	enum DisplayDeform {
		DEFORM_NORMAL, DEFORM_3D, DEFORM_HOR_STRETCH
	};

	explicit RenderSettings(CommandController& commandController);
	~RenderSettings();

	/** Accuracy [screen, line, pixel]. */
	EnumSetting<Accuracy>& getAccuracy() const { return *accuracy; }

	/** Deinterlacing [on, off]. */
	BooleanSetting& getDeinterlace() const { return *deinterlace; }

	/** The current max frameskip. */
	IntegerSetting& getMaxFrameSkip() const { return *maxFrameSkip; }

	/** The current min frameskip. */
	IntegerSetting& getMinFrameSkip() const { return *minFrameSkip; }

	/** Full screen [on, off]. */
	BooleanSetting& getFullScreen() const { return *fullScreen; }

	/** The amount of gamma correction. */
	FloatSetting& getGamma() const { return *gamma; }

	/** Brightness video setting. */
	FloatSetting& getBrightness() const { return *brightness; }

	/** Contrast video setting. */
	FloatSetting& getContrast() const { return *contrast; }

	/** Contrast video setting. */
	StringSetting& getColorMatrix() const { return *colorMatrix; }

	/** The amount of glow [0..100]. */
	IntegerSetting& getGlow() const { return *glow; }

	/** The amount of noise to add to the frame. */
	FloatSetting& getNoise() const { return *noise; }

	/** The amount of horizontal blur [0..256]. */
	int getBlurFactor() const;

	/** The alpha value [0..255] of the scanlines. */
	int getScanlineFactor() const;

	/** The current renderer. */
	RendererFactory::RendererSetting& getRenderer() const { return *renderer; }

	/** The current scaling algorithm. */
	EnumSetting<ScaleAlgorithm>& getScaleAlgorithm() const {
		return *scaleAlgorithm;
	}

	/** The current scaling factor. */
	IntegerSetting& getScaleFactor() const { return *scaleFactor; }

	/** The video source to display on the screen. */
	VideoSourceSetting& getVideoSource() const { return *videoSource; }

	/** Limit number of sprites per line?
	  * If true, limit number of sprites per line as real VDP does.
	  * If false, display all sprites.
	  * For accurate emulation, this setting should be on.
	  * Turning it off can improve games with a lot of flashing sprites,
	  * such as Aleste. */
	BooleanSetting& getLimitSprites() { return *limitSprites; }

	/** CmdTiming [real, broken].
	  * This setting is intended for debugging only, not for users. */
	EnumSetting<bool>& getCmdTiming() { return *cmdTiming; }

	/**  */
	EnumSetting<DisplayDeform>& getDisplayDeform() const { return *displayDeform; }

	/** Apply brightness, contrast and gamma transformation on the input
	  * color. The R, G and B component are expected to be in the range
	  * [0.0  1.0] but it's not an error if a component lays outside of
	  * this range. After transformation it's guaranteed all components
	  * lay inside this range.
	  */
	void transformRGB(double& r, double& g, double& b);

private:
	void parseColorMatrix(const std::string& value);

	// Please keep the settings ordered alphabetically.
	std::auto_ptr<EnumSetting<Accuracy> > accuracy;
	std::auto_ptr<EnumSetting<bool> > cmdTiming;
	std::auto_ptr<BooleanSetting> deinterlace;
	std::auto_ptr<BooleanSetting> fullScreen;
	std::auto_ptr<FloatSetting> gamma;
	std::auto_ptr<FloatSetting> brightness;
	std::auto_ptr<FloatSetting> contrast;
	std::auto_ptr<ColorMatrixChecker> colorMatrixChecker;
	std::auto_ptr<StringSetting> colorMatrix;
	std::auto_ptr<IntegerSetting> glow;
	std::auto_ptr<FloatSetting> noise;
	std::auto_ptr<IntegerSetting> horizontalBlur;
	std::auto_ptr<BooleanSetting> limitSprites;
	std::auto_ptr<IntegerSetting> maxFrameSkip;
	std::auto_ptr<IntegerSetting> minFrameSkip;
	std::auto_ptr<RendererFactory::RendererSetting> renderer;
	std::auto_ptr<EnumSetting<ScaleAlgorithm> > scaleAlgorithm;
	std::auto_ptr<IntegerSetting> scaleFactor;
	std::auto_ptr<IntegerSetting> scanlineAlpha;
	std::auto_ptr<VideoSourceSetting> videoSource;
	std::auto_ptr<EnumSetting<DisplayDeform> > displayDeform;

	CommandController& commandController;
	double cm[3][3]; // parsed color matrix, should always be in sync with
	                 // colorMatrix setting

	friend class ColorMatrixChecker;
};

} // namespace openmsx

#endif
