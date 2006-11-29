// $Id$

#include "Scaler3.hh"
#include "LineScalers.hh"
#include "FrameSource.hh"
#include "OutputSurface.hh"
#include "MemoryOps.hh"
#include "openmsx.hh"

namespace openmsx {

template <class Pixel>
Scaler3<Pixel>::Scaler3(const PixelOperations<Pixel>& pixelOps_)
	: pixelOps(pixelOps_)
{
}

template <class Pixel>
void Scaler3<Pixel>::scaleBlank1to3(
		FrameSource& src, unsigned srcStartY, unsigned /*srcEndY*/,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	MemoryOps::MemSet<Pixel, MemoryOps::STREAMING> memset;
	for (unsigned srcY = srcStartY, dstY = dstStartY;
	     dstY < dstEndY; srcY += 1, dstY += 3) {
		Pixel* dummy = 0;
		Pixel color = src.getLinePtr(srcY, dummy)[0];
		Pixel* dstLine0 = dst.getLinePtr(dstY + 0, dummy);
		memset(dstLine0, dst.getWidth(), color);
		Pixel* dstLine1 = dst.getLinePtr(dstY + 1, dummy);
		memset(dstLine1, dst.getWidth(), color);
		Pixel* dstLine2 = dst.getLinePtr(dstY + 2, dummy);
		memset(dstLine2, dst.getWidth(), color);
	}
}

template <class Pixel>
void Scaler3<Pixel>::scaleBlank2to3(
		FrameSource& src, unsigned srcStartY, unsigned /*srcEndY*/,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	MemoryOps::MemSet<Pixel, MemoryOps::STREAMING> memset;
	for (unsigned srcY = srcStartY, dstY = dstStartY;
	     dstY < dstEndY; srcY += 2, dstY += 3) {
		Pixel* dummy = 0;
		Pixel color0 = src.getLinePtr(srcY + 0, dummy)[0];
		Pixel color1 = src.getLinePtr(srcY + 1, dummy)[0];
		Pixel color01 = pixelOps.template blend<1, 1>(color0, color1);
		Pixel* dstLine0 = dst.getLinePtr(dstY + 0, dummy);
		memset(dstLine0, dst.getWidth(), color0);
		Pixel* dstLine1 = dst.getLinePtr(dstY + 1, dummy);
		memset(dstLine1, dst.getWidth(), color01);
		Pixel* dstLine2 = dst.getLinePtr(dstY + 2, dummy);
		memset(dstLine2, dst.getWidth(), color1);
	}
}

template <typename Pixel, typename ScaleOp>
static void doScale1(FrameSource& src,
	unsigned srcStartY, unsigned /*srcEndY*/, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY,
	ScaleOp scale)
{
	Scale_1on1<Pixel> copy;
	for (unsigned dstY = dstStartY; dstY < dstEndY; dstY += 3, ++srcStartY) {
		Pixel* dummy = 0;
		const Pixel* srcLine = src.getLinePtr(srcStartY, srcWidth, dummy);
		Pixel* dstLine0 = dst.getLinePtr(dstY + 0, dummy);
		scale(srcLine, dstLine0, dst.getWidth());

		Pixel* dstLine1 = dst.getLinePtr(dstY + 1, dummy);
		if (IsTagged<ScaleOp, Streaming>::result) {
			scale(srcLine, dstLine1, dst.getWidth());
		} else {
			copy(dstLine0, dstLine1, dst.getWidth());
		}

		Pixel* dstLine2 = dst.getLinePtr(dstY + 2, dummy);
		if (IsTagged<ScaleOp, Streaming>::result) {
			scale(srcLine, dstLine2, dst.getWidth());
		} else {
			copy(dstLine0, dstLine2, dst.getWidth());
		}
	}
}

template <typename Pixel, typename ScaleOp>
static void doScaleDV(FrameSource& src,
	unsigned srcStartY, unsigned /*srcEndY*/, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY,
	PixelOperations<Pixel> ops, ScaleOp scale)
{
	BlendLines<Pixel> blend(ops);
	for (unsigned srcY = srcStartY, dstY = dstStartY; dstY < dstEndY;
	     srcY += 2, dstY += 3) {
		Pixel* dummy = 0;
		const Pixel* srcLine0 = src.getLinePtr(srcY + 0, srcWidth, dummy);
		Pixel* dstLine0 = dst.getLinePtr(dstY + 0, dummy);
		scale(srcLine0, dstLine0, dst.getWidth());

		const Pixel* srcLine1 = src.getLinePtr(srcY + 1, srcWidth, dummy);
		Pixel* dstLine2 = dst.getLinePtr(dstY + 2, dummy);
		scale(srcLine1, dstLine2, dst.getWidth());

		Pixel* dstLine1 = dst.getLinePtr(dstY + 1, dummy);
		blend(dstLine0, dstLine2, dstLine1, dst.getWidth());
	}
}

template <class Pixel>
void Scaler3<Pixel>::scale2x1to9x3(FrameSource& src,
		unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale1<Pixel>(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	                Scale_2on9<Pixel>(pixelOps));
}

template <class Pixel>
void Scaler3<Pixel>::scale2x2to9x3(FrameSource& src,
		unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScaleDV<Pixel>(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	                 pixelOps, Scale_2on9<Pixel>(pixelOps));
}

// TODO: See comment for Scaler2::scale256.
template <class Pixel>
void Scaler3<Pixel>::scale1x1to3x3(FrameSource& src,
		unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale1<Pixel>(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	                Scale_1on3<Pixel>());
}

template <class Pixel>
void Scaler3<Pixel>::scale1x2to3x3(FrameSource& src,
		unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScaleDV<Pixel>(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	                 pixelOps, Scale_1on3<Pixel>());
}

template <class Pixel>
void Scaler3<Pixel>::scale4x1to9x3(FrameSource& src,
		unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale1<Pixel>(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	                Scale_4on9<Pixel>(pixelOps));
}

template <class Pixel>
void Scaler3<Pixel>::scale4x2to9x3(FrameSource& src,
		unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScaleDV<Pixel>(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	                 pixelOps, Scale_4on9<Pixel>(pixelOps));
}

template <class Pixel>
void Scaler3<Pixel>::scale2x1to3x3(FrameSource& src,
		unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale1<Pixel>(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	                Scale_2on3<Pixel>(pixelOps));
}

template <class Pixel>
void Scaler3<Pixel>::scale2x2to3x3(FrameSource& src,
		unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScaleDV<Pixel>(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	                 pixelOps, Scale_2on3<Pixel>(pixelOps));
}

template <class Pixel>
void Scaler3<Pixel>::scale8x1to9x3(FrameSource& src,
		unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale1<Pixel>(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	                Scale_8on9<Pixel>(pixelOps));
}

template <class Pixel>
void Scaler3<Pixel>::scale8x2to9x3(FrameSource& src,
		unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScaleDV<Pixel>(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	                 pixelOps, Scale_8on9<Pixel>(pixelOps));
}

template <class Pixel>
void Scaler3<Pixel>::scale4x1to3x3(FrameSource& src,
		unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale1<Pixel>(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	                Scale_4on3<Pixel>(pixelOps));
}

template <class Pixel>
void Scaler3<Pixel>::scale4x2to3x3(FrameSource& src,
		unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScaleDV<Pixel>(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	                 pixelOps, Scale_4on3<Pixel>(pixelOps));
}

// TODO: This method doesn't have any dependency on the pixel format, so is it
//       possible to move it to a class without the Pixel template parameter?
template <class Pixel>
void Scaler3<Pixel>::scaleImage(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	if (src.getHeight() == 240) {
		switch (srcWidth) {
		case 1:
			scaleBlank1to3(src, srcStartY, srcEndY,
			              dst, dstStartY, dstEndY);
			break;
		case 213:
			scale2x1to9x3(src, srcStartY, srcEndY, srcWidth,
			              dst, dstStartY, dstEndY);
			break;
		case 320:
			scale1x1to3x3(src, srcStartY, srcEndY, srcWidth,
			              dst, dstStartY, dstEndY);
			break;
		case 426:
			scale4x1to9x3(src, srcStartY, srcEndY, srcWidth,
			              dst, dstStartY, dstEndY);
			break;
		case 640:
			scale2x1to3x3(src, srcStartY, srcEndY, srcWidth,
			              dst, dstStartY, dstEndY);
			break;
		case 853:
			scale8x1to9x3(src, srcStartY, srcEndY, srcWidth,
			              dst, dstStartY, dstEndY);
			break;
		case 1280:
			scale4x1to3x3(src, srcStartY, srcEndY, srcWidth,
			              dst, dstStartY, dstEndY);
			break;
		default:
			assert(false);
			break;
		}
	} else {
		assert(src.getHeight() == 480);
		switch (srcWidth) {
		case 1:
			scaleBlank2to3(src, srcStartY, srcEndY,
			              dst, dstStartY, dstEndY);
			break;
		case 213:
			scale2x2to9x3(src, srcStartY, srcEndY, srcWidth,
			              dst, dstStartY, dstEndY);
			break;
		case 320:
			scale1x2to3x3(src, srcStartY, srcEndY, srcWidth,
			              dst, dstStartY, dstEndY);
			break;
		case 426:
			scale4x2to9x3(src, srcStartY, srcEndY, srcWidth,
			              dst, dstStartY, dstEndY);
			break;
		case 640:
			scale2x2to3x3(src, srcStartY, srcEndY, srcWidth,
			              dst, dstStartY, dstEndY);
			break;
		case 853:
			scale8x2to9x3(src, srcStartY, srcEndY, srcWidth,
			              dst, dstStartY, dstEndY);
			break;
		case 1280:
			scale4x2to3x3(src, srcStartY, srcEndY, srcWidth,
			              dst, dstStartY, dstEndY);
			break;
		default:
			assert(false);
			break;
		}
	}
}


// Force template instantiation.
template class Scaler3<word>;
template class Scaler3<unsigned>;

} // namespace openmsx
