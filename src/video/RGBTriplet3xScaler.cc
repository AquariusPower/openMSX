// $Id$

#include "RGBTriplet3xScaler.hh"
#include "LineScalers.hh"
#include "FrameSource.hh"
#include "OutputSurface.hh"
#include "RenderSettings.hh"
#include "vla.hh"
#include "build-info.hh"

namespace openmsx {

template <class Pixel>
RGBTriplet3xScaler<Pixel>::RGBTriplet3xScaler(
		const PixelOperations<Pixel>& pixelOps_,
		const RenderSettings& renderSettings)
	: Scaler3<Pixel>(pixelOps_)
	, pixelOps(pixelOps_)
	, scanline(pixelOps_)
	, settings(renderSettings)
{
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::recalcBlur()
{
	c1 = settings.getBlurFactor();
	c2 = (3 * 256) - (2 * c1);
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::calcSpil(unsigned x, unsigned& r, unsigned& s)
{
	r = (c2 * x) >> 8;
	s = (c1 * x) >> 8;
	if (r > 255) {
		s += (r - 255) / 2;
		r = 255;
	}
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::rgbify(
		const Pixel* __restrict in, Pixel* __restrict out, unsigned inwidth)
{
	unsigned r, g, b, rs, gs, bs;
	unsigned i = 0;

	calcSpil(pixelOps.red256  (in[i + 0]), r, rs);
	calcSpil(pixelOps.green256(in[i + 0]), g, gs);
	out[3 * i + 0] = pixelOps.combine256(r , gs, 0 );
	calcSpil(pixelOps.blue256 (in[i + 0]), b, bs);
	out[3 * i + 1] = pixelOps.combine256(rs, g , bs);
	calcSpil(pixelOps.red256  (in[i + 1]), r, rs);
	out[3 * i + 2] = pixelOps.combine256(rs, gs, b );

	for (++i; i < (inwidth - 1); ++i) {
		calcSpil(pixelOps.green256(in[i + 0]), g, gs);
		out[3 * i + 0] = pixelOps.combine256(r , gs, bs);
		calcSpil(pixelOps.blue256 (in[i + 0]), b, bs);
		out[3 * i + 1] = pixelOps.combine256(rs, g , bs);
		calcSpil(pixelOps.red256  (in[i + 1]), r, rs);
		out[3 * i + 2] = pixelOps.combine256(rs, gs, b );
	}

	calcSpil(pixelOps.green256(in[i + 0]), g, gs);
	out[3 * i + 0] = pixelOps.combine256(r , gs, bs);
	calcSpil(pixelOps.blue256 (in[i + 0]), b, bs);
	out[3 * i + 1] = pixelOps.combine256(rs, g , bs);
	out[3 * i + 2] = pixelOps.combine256(0 , gs, b );
}

template <typename Pixel>
template <typename ScaleOp>
void RGBTriplet3xScaler<Pixel>::scaleLine(
		const Pixel* srcLine, Pixel* dstLine, ScaleOp scale,
		unsigned tmpWidth)
{
	if (IsTagged<ScaleOp, Copy>::result) {
		rgbify(srcLine, dstLine, tmpWidth);
	} else {
		VLA(Pixel, tmp, tmpWidth);
		scale(srcLine, tmp, tmpWidth);
		rgbify(tmp, dstLine, tmpWidth);
	}
}

// Note: the idea is that this method RGBifies a line that is first scaled
// to output-width / 3. So, when calling this, keep this in mind and pass a
// scale functor that scales the input with correctly.
template <typename Pixel>
template <typename ScaleOp>
void RGBTriplet3xScaler<Pixel>::doScale1(FrameSource& src,
	unsigned srcStartY, unsigned /*srcEndY*/, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY,
	ScaleOp scale)
{
	dst.lock();
	recalcBlur();

	unsigned tmpWidth = dst.getWidth() / 3;
	int scanlineFactor = settings.getScanlineFactor();
	unsigned y = dstStartY;
	const Pixel* srcLine = src.getLinePtr<Pixel>(srcStartY++, srcWidth);
	Pixel* prevDstLine0 = dst.getLinePtrDirect<Pixel>(y + 0);
	scaleLine(srcLine, prevDstLine0, scale, tmpWidth);

	Scale_1on1<Pixel> copy;
	Pixel* dstLine1 = dst.getLinePtrDirect<Pixel>(y + 1);
	copy(prevDstLine0, dstLine1, dst.getWidth());

	for (/* */; (y + 4) < dstEndY; y += 3, srcStartY += 1) {
		srcLine = src.getLinePtr<Pixel>(srcStartY, srcWidth);
		Pixel* dstLine0 = dst.getLinePtrDirect<Pixel>(y + 3);
		scaleLine(srcLine, dstLine0, scale, tmpWidth);

		dstLine1 = dst.getLinePtrDirect<Pixel>(y + 4);
		copy(dstLine0, dstLine1, dst.getWidth());

		Pixel* dstLine2 = dst.getLinePtrDirect<Pixel>(y + 2);
		scanline.draw(prevDstLine0, dstLine0, dstLine2,
		              scanlineFactor, dst.getWidth());

		prevDstLine0 = dstLine0;
	}

	srcLine = src.getLinePtr<Pixel>(srcStartY, srcWidth);
	VLA(Pixel, buf, dst.getWidth());
	scaleLine(srcLine, buf, scale, tmpWidth);
	Pixel* dstLine2 = dst.getLinePtrDirect<Pixel>(y + 2);
	scanline.draw(prevDstLine0, buf, dstLine2,
	              scanlineFactor, dst.getWidth());
}

template <typename Pixel>
template <typename ScaleOp>
void RGBTriplet3xScaler<Pixel>::doScale2(FrameSource& src,
	unsigned srcStartY, unsigned /*srcEndY*/, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY,
	ScaleOp scale)
{
	dst.lock();
	recalcBlur();

	unsigned tmpWidth = dst.getWidth() / 3;
	int scanlineFactor = settings.getScanlineFactor();
	for (unsigned srcY = srcStartY, dstY = dstStartY; dstY < dstEndY;
	     srcY += 2, dstY += 3) {
		const Pixel* srcLine0 = src.getLinePtr<Pixel>(srcY + 0, srcWidth);
		Pixel* dstLine0 = dst.getLinePtrDirect<Pixel>(dstY + 0);
		scaleLine(srcLine0, dstLine0, scale, tmpWidth);

		const Pixel* srcLine1 = src.getLinePtr<Pixel>(srcY + 1, srcWidth);
		Pixel* dstLine2 = dst.getLinePtrDirect<Pixel>(dstY + 2);
		scaleLine(srcLine1, dstLine2, scale, tmpWidth);

		Pixel* dstLine1 = dst.getLinePtrDirect<Pixel>(dstY + 1);
		scanline.draw(dstLine0, dstLine2, dstLine1,
		              scanlineFactor, dst.getWidth());
	}
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scale2x1to9x3(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale1(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	         Scale_2on3<Pixel>(pixelOps));
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scale2x2to9x3(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale2(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	         Scale_2on3<Pixel>(pixelOps));
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scale1x1to3x3(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale1(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	         Scale_1on1<Pixel>());
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scale1x2to3x3(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale2(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	         Scale_1on1<Pixel>());
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scale4x1to9x3(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale1(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	         Scale_4on3<Pixel>(pixelOps));
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scale4x2to9x3(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale2(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	         Scale_4on3<Pixel>(pixelOps));
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scale2x1to3x3(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale1(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	         Scale_2on1<Pixel>(pixelOps));
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scale2x2to3x3(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale2(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	         Scale_2on1<Pixel>(pixelOps));
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scale8x1to9x3(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale1(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	         Scale_8on3<Pixel>(pixelOps));
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scale8x2to9x3(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale2(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	         Scale_8on3<Pixel>(pixelOps));
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scale4x1to3x3(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale1(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	         Scale_4on1<Pixel>(pixelOps));
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scale4x2to3x3(FrameSource& src,
	unsigned srcStartY, unsigned srcEndY, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	doScale2(src, srcStartY, srcEndY, srcWidth, dst, dstStartY, dstEndY,
	         Scale_4on1<Pixel>(pixelOps));
}

template <typename Pixel>
static void fillLoop(const Pixel* __restrict in, Pixel* __restrict out,
                     unsigned dstWidth)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
	for (unsigned x = 3; x < (dstWidth - 3); x += 3) {
		out[x + 0] = in[3];
		out[x + 1] = in[4];
		out[x + 2] = in[5];
	}
	out[dstWidth - 3] = in[6];
	out[dstWidth - 2] = in[7];
	out[dstWidth - 1] = in[8];
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scaleBlank1to3(
		FrameSource& src, unsigned srcStartY, unsigned srcEndY,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	dst.lock();
	recalcBlur();
	int scanlineFactor = settings.getScanlineFactor();

	unsigned stopDstY = (dstEndY == dst.getHeight())
	                  ? dstEndY : dstEndY - 3;
	unsigned srcY = srcStartY, dstY = dstStartY;
	for (/* */; dstY < stopDstY; srcY += 1, dstY += 3) {
		Pixel color = src.getLinePtr<Pixel>(srcY)[0];

		Pixel inNormal [3];
		Pixel outNormal[3 * 3];
		inNormal[0] = inNormal[1] = inNormal[2] = color;
		rgbify(inNormal, outNormal, 3);
		Pixel outScanline[3 * 3];
		for (int i = 0; i < (3 * 3); ++i) {
			outScanline[i] = scanline.darken(
					outNormal[i], scanlineFactor);
		}

		fillLoop(outNormal,   dst.getLinePtrDirect<Pixel>(dstY + 0),
		         dst.getWidth());
		fillLoop(outNormal,   dst.getLinePtrDirect<Pixel>(dstY + 1),
		         dst.getWidth());
		fillLoop(outScanline, dst.getLinePtrDirect<Pixel>(dstY + 2),
		         dst.getWidth());
	}
	if (dstY != dst.getHeight()) {
		unsigned nextLineWidth = src.getLineWidth(srcY + 1);
		assert(src.getLineWidth(srcY) == 1);
		assert(nextLineWidth != 1);
		this->scaleImage(src, srcY, srcEndY, nextLineWidth,
		                 dst, dstY, dstEndY);
	}
}

template <class Pixel>
void RGBTriplet3xScaler<Pixel>::scaleBlank2to3(
		FrameSource& src, unsigned srcStartY, unsigned /*srcEndY*/,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	dst.lock();
	recalcBlur();
	int scanlineFactor = settings.getScanlineFactor();
	for (unsigned srcY = srcStartY, dstY = dstStartY;
	     dstY < dstEndY; srcY += 2, dstY += 3) {
		Pixel color0 = src.getLinePtr<Pixel>(srcY + 0)[0];
		Pixel color1 = src.getLinePtr<Pixel>(srcY + 1)[0];

		Pixel inNormal [3];
		Pixel out0Normal[3 * 3];
		Pixel out1Normal[3 * 3];
		Pixel outScanline[3 * 3];

		inNormal[0] = inNormal[1] = inNormal[2] = color0;
		rgbify(inNormal, out0Normal, 3);
		inNormal[0] = inNormal[1] = inNormal[2] = color1;
		rgbify(inNormal, out1Normal, 3);

		for (int i = 0; i < (3 * 3); ++i) {
			outScanline[i] = scanline.darken(
				out0Normal[i], out1Normal[i],
				scanlineFactor);
		}

		fillLoop(out0Normal,  dst.getLinePtrDirect<Pixel>(dstY + 0),
		         dst.getWidth());
		fillLoop(outScanline, dst.getLinePtrDirect<Pixel>(dstY + 1),
		         dst.getWidth());
		fillLoop(out1Normal,  dst.getLinePtrDirect<Pixel>(dstY + 2),
		         dst.getWidth());
	}
}

// Force template instantiation.
#if HAVE_16BPP
template class RGBTriplet3xScaler<word>;
#endif
#if HAVE_32BPP
template class RGBTriplet3xScaler<unsigned>;
#endif

} // namespace openmsx
