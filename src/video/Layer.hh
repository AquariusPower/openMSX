// $Id$

#ifndef LAYER_HH
#define LAYER_HH

#include <string>

namespace openmsx {

class LayerListener;

/** Interface for display layers.
  */
class Layer
{
public:
	/** Determines stacking order of layers:
	  * layers with higher Z-indices are closer to the viewer.
	  */
	enum ZIndex {
		Z_DUMMY = -1,
		Z_BACKGROUND = 0,
		Z_MSX_PASSIVE = 30,
		Z_MSX_ACTIVE = 40,
		Z_OSDGUI = 50,
		Z_ICONS = 90,
		Z_CONSOLE = 100
	};

	virtual ~Layer();

	/** Paint this layer.
	  */
	virtual void paint() = 0;

	/** Returns the name of this layer. Used for debugging.
	  */
	virtual const std::string& getName() = 0;

	/** Query the Z-index of this layer.
	  */
	ZIndex getZ() const;

protected:
	/** Describes how much of the screen is currently covered by a particular
	  * layer.
	  */
	enum Coverage {
		/** Layer fully covers the screen: any underlying layers are invisible.
		  */
		COVER_FULL,
		/** Layer partially covers the screen: it may cover only part of the
		  * screen area, or it may be (semi-)transparent in places.
		  */
		COVER_PARTIAL,
		/** Layer is not visible, that is completely transparent.
		  */
		COVER_NONE
	};

	/** Construct a layer. */
	explicit Layer(Coverage coverage = COVER_NONE, ZIndex z = Z_DUMMY);

	/** Changes the current coverage of this layer.
	  */
	void setCoverage(Coverage coverage);

	/** Changes the current Z-index of this layer.
	  */
	void setZ(ZIndex z);

private:
	friend class LayerListener;
	friend class Display;

	/** The display this layer is part of. */
	LayerListener* display;

	/** Inspected by Display to determine which layers to paint. */
	Coverage coverage;

	/** Inspected by Display to determine the order in which layers are
	  * painted.
	  */
	ZIndex z;
};

} // namespace openmsx

#endif
