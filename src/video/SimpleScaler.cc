// $Id$

#include "SimpleScaler.hh"
#include "LineScalers.hh"
#include "FrameSource.hh"
#include "OutputSurface.hh"
#include "RenderSettings.hh"
#include "MemoryOps.hh"
#include "HostCPU.hh"
#include "openmsx.hh"
#include "vla.hh"
#include "build-info.hh"
#include <cassert>

namespace openmsx {

// class SimpleScaler

template <class Pixel>
SimpleScaler<Pixel>::SimpleScaler(
		const PixelOperations<Pixel>& pixelOps,
		RenderSettings& renderSettings)
	: Scaler2<Pixel>(pixelOps)
	, settings(renderSettings)
	, mult1(pixelOps.format)
	, mult2(pixelOps.format)
	, mult3(pixelOps.format)
	, scanline(pixelOps)
{
}

template <class Pixel>
void SimpleScaler<Pixel>::scaleBlank1to2(
		FrameSource& src, unsigned srcStartY, unsigned srcEndY,
		OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	dst.lock();
	int scanlineFactor = settings.getScanlineFactor();

	unsigned stopDstY = (dstEndY == dst.getHeight())
	                  ? dstEndY : dstEndY - 2;
	unsigned srcY = srcStartY, dstY = dstStartY;
	MemoryOps::MemSet<Pixel, MemoryOps::STREAMING> memset;
	for (/* */; dstY < stopDstY; srcY += 1, dstY += 2) {
		Pixel color0 = src.getLinePtr<Pixel>(srcY)[0];
		Pixel* dstLine0 = dst.getLinePtrDirect<Pixel>(dstY + 0);
		memset(dstLine0, dst.getWidth(), color0);
		Pixel color1 = scanline.darken(color0, scanlineFactor);
		Pixel* dstLine1 = dst.getLinePtrDirect<Pixel>(dstY + 1);
		memset(dstLine1, dst.getWidth(), color1);
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
void SimpleScaler<Pixel>::blur1on2(const Pixel* pIn, Pixel* pOut, unsigned alpha,
                                   unsigned long srcWidth)
{
	/* This routine is functionally equivalent to the following:
	 *
	 * void blur1on2(const Pixel* pIn, Pixel* pOut, unsigned alpha)
	 * {
	 *         unsigned c1 = alpha;
	 *         unsigned c2 = 256 - c1;
	 *
	 *         Pixel prev, curr, next;
	 *         prev = curr = pIn[0];
	 *
	 *         unsigned x;
	 *         for (x = 0; x < (srcWidth - 1); ++x) {
	 *                 pOut[2 * x + 0] = (c1 * prev + c2 * curr) >> 8;
	 *                 Pixel next = pIn[x + 1];
	 *                 pOut[2 * x + 1] = (c1 * next + c2 * curr) >> 8;
	 *                 prev = curr;
	 *                 curr = next;
	 *         }
	 *
	 *         pOut[2 * x + 0] = (c1 * prev + c2 * curr) >> 8;
	 *         next = curr;
	 *         pOut[2 * x + 1] = (c1 * next + c2 * curr) >> 8;
	 * }
	 *
	 * The loop is 2x unrolled and all common subexpressions and redundant
	 * assignments have been eliminated. 1 loop iteration processes 4
	 * (output) pixels.
	 */

	if (alpha == 0) {
		Scale_1on2<Pixel, false> scale; // no streaming stores
		scale(pIn, pOut, 2 * srcWidth);
		return;
	}

	assert(alpha <= 256);
	unsigned c1 = alpha / 4;
	unsigned c2 = 256 - c1;

	#ifdef ASM_X86
	#ifdef _MSC_VER
	// TODO - VC++ ASM implementation
	#else
	const HostCPU& cpu = HostCPU::getInstance();
	if ((sizeof(Pixel) == 4) && cpu.hasMMX()) { // Note: not hasMMXEXT()
		// MMX routine, 32bpp
		assert(((srcWidth * 4) % 8) == 0);
		asm (
			"movd	%2, %%mm5;"
			"punpcklwd %%mm5, %%mm5;"
			"punpckldq %%mm5, %%mm5;"	// mm5 = c1
			"movd	%3, %%mm6;"
			"punpcklwd %%mm6, %%mm6;"
			"punpckldq %%mm6, %%mm6;"	// mm6 = c2
			"pxor	%%mm7, %%mm7;"

			"movd	(%0,%4), %%mm0;"
			"punpcklbw %%mm7, %%mm0;"	// p0 = pIn[0]
			"movq	%%mm0, %%mm2;"
			"pmullw	%%mm5, %%mm2;"		// f0 = multiply(p0, c1)
			"movq	%%mm2, %%mm3;"		// f1 = f0

			".p2align 4,,15;"
		"1:"
			"pmullw	%%mm6, %%mm0;"
			"movq	%%mm0, %%mm4;"		// tmp = multiply(p0, c2)
			"paddw	%%mm3, %%mm0;"
			"psrlw	$8, %%mm0;"		// f1 + tmp

			"movd	4(%0,%4), %%mm1;"
			"punpcklbw %%mm7, %%mm1;"	// p1 = pIn[x + 1]
			"movq	%%mm1, %%mm3;"
			"pmullw	%%mm5, %%mm3;"		// f1 = multiply(p1, c1)
			"paddw	%%mm3, %%mm4;"
			"psrlw	$8, %%mm4;"		// f1 + tmp
			"packuswb %%mm4, %%mm0;"
			"movq	%%mm0, (%1,%4,2);"	// pOut[2*x+0] = ..  pOut[2*x+1] = ..

			"pmullw	%%mm6, %%mm1;"
			"movq	%%mm1, %%mm4;"		// tmp = multiply(p1, c2)
			"paddw	%%mm2, %%mm1;"
			"psrlw	$8, %%mm1;"		// f0 + tmp

			"movd	8(%0,%4), %%mm0;"
			"punpcklbw %%mm7, %%mm0;"	// p0 = pIn[x + 2]
			"movq	%%mm0, %%mm2;"
			"pmullw %%mm5, %%mm2;"		// f0 = multiply(p0, c1)
			"paddw	%%mm2, %%mm4;"
			"psrlw	$8, %%mm4;"		// f0 + tmp
			"packuswb %%mm4, %%mm1;"
			"movq	%%mm1, 8(%1,%4,2);"	// pOut[2*x+2] = ..  pOut[2*x+3] = ..

			"add	$8, %4;"
			"jnz	1b;"

			"pmullw	%%mm6, %%mm0;"
			"movq	%%mm0, %%mm4;"		// tmp = multiply(p0, c2)
			"paddw	%%mm3, %%mm0;"
			"psrlw	$8, %%mm0;"		// f1 + tmp

			"movd	4(%0), %%mm1;"
			"punpcklbw %%mm7, %%mm1;"	// p1 = pIn[x + 1]
			"movq	%%mm1, %%mm3;"
			"pmullw	%%mm5, %%mm3;"		// f1 = multiply(p1, c1)
			"paddw	%%mm3, %%mm4;"
			"psrlw	$8, %%mm4;"		// f1 + tmp
			"packuswb %%mm4, %%mm0;"
			"movq	%%mm0, (%1);"		// pOut[2*x+0] = ..  pOut[2*x+1] = ..

			"movq	%%mm1, %%mm4;"
			"pmullw	%%mm6, %%mm1;"		// tmp = multiply(p1, c2)
			"paddw	%%mm2, %%mm1;"
			"psrlw	$8, %%mm1;"		// f0 + tmp

			"packuswb %%mm4, %%mm1;"
			"movq	%%mm1, 8(%1);"		// pOut[2*x+0] = ..  pOut[2*x+1] = ..

			"emms;"

			: // no output
			: "r" (pIn  +     (srcWidth - 2)) // 0
			, "r" (pOut + 2 * (srcWidth - 2)) // 1
			, "r" (c1)                         // 2
			, "r" (c2)                         // 3
			, "r" (-4 * (srcWidth - 2))       // 4
			#ifdef __MMX__
			: "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7"
			#endif
		);
		return;
	}
	#endif
	#endif

	// non-MMX routine, both 16bpp and 32bpp
	mult1.setFactor32(c1);
	mult2.setFactor32(c2);

	Pixel p0 = pIn[0];
	Pixel p1;
	unsigned f0 = mult1.mul32(p0);
	unsigned f1 = f0;
	unsigned tmp;

	unsigned x;
	for (x = 0; x < (srcWidth - 2); x += 2) {
		tmp = mult2.mul32(p0);
		pOut[2 * x + 0] = mult1.conv32(f1 + tmp);

		p1 = pIn[x + 1];
		f1 = mult1.mul32(p1);
		pOut[2 * x + 1] = mult1.conv32(f1 + tmp);

		tmp = mult2.mul32(p1);
		pOut[2 * x + 2] = mult1.conv32(f0 + tmp);

		p0 = pIn[x + 2];
		f0 = mult1.mul32(p0);
		pOut[2 * x + 3] = mult1.conv32(f0 + tmp);
	}

	tmp = mult2.mul32(p0);
	pOut[2 * x + 0] = mult1.conv32(f1 + tmp);

	p1 = pIn[x + 1];
	f1 = mult1.mul32(p1);
	pOut[2 * x + 1] = mult1.conv32(f1 + tmp);

	tmp = mult2.mul32(p1);
	pOut[2 * x + 2] = mult1.conv32(f0 + tmp);

	pOut[2 * x + 3] = p1;
}

template <class Pixel>
void SimpleScaler<Pixel>::blur1on1(const Pixel* pIn, Pixel* pOut, unsigned alpha,
                                   unsigned long srcWidth)
{
	/* This routine is functionally equivalent to the following:
	 *
	 * void blur1on1(const Pixel* pIn, Pixel* pOut, unsigned alpha)
	 * {
	 *         unsigned c1 = alpha / 2;
	 *         unsigned c2 = 256 - alpha;
	 *
	 *         Pixel prev, curr, next;
	 *         prev = curr = pIn[0];
	 *
	 *         unsigned x;
	 *         for (x = 0; x < (srcWidth - 1); ++x) {
	 *                 next = pIn[x + 1];
	 *                 pOut[x] = (c1 * prev + c2 * curr + c1 * next) >> 8;
	 *                 prev = curr;
	 *                 curr = next;
	 *         }
	 *
	 *         next = curr;
	 *         pOut[x] = c1 * prev + c2 * curr + c1 * next;
	 * }
	 *
	 * The loop is 2x unrolled and all common subexpressions and redundant
	 * assignments have been eliminated. 1 loop iteration processes 2
	 * pixels.
	 */

	if (alpha == 0) {
		Scale_1on1<Pixel, false> copy; // no streaming stores
		copy(pIn, pOut, srcWidth);
		return;
	}

	unsigned c1 = alpha / 4;
	unsigned c2 = 256 - alpha / 2;

	#ifdef ASM_X86
	#ifdef _MSC_VER
	// TODO - VC++ ASM implementation
	#else
	const HostCPU& cpu = HostCPU::getInstance();
	if ((sizeof(Pixel) == 4) && cpu.hasMMX()) { // Note: not hasMMXEXT()
		// MMX routine, 32bpp
		assert(((srcWidth * 4) % 8) == 0);
		asm (
			"movd	%2, %%mm5;"
			"punpcklwd %%mm5, %%mm5;"
			"punpckldq %%mm5, %%mm5;"	// mm5 = c1
			"movd	%3, %%mm6;"
			"punpcklwd %%mm6, %%mm6;"
			"punpckldq %%mm6, %%mm6;"	// mm6 = c2
			"pxor	%%mm7, %%mm7;"

			"movd	(%0,%4), %%mm0;"
			"punpcklbw %%mm7, %%mm0;"	// p0 = pIn[0]
			"movq	%%mm0, %%mm2;"
			"pmullw	%%mm5, %%mm2;"		// f0 = multiply(p0, c1)
			"movq	%%mm2, %%mm3;"		// f1 = f0

			".p2align 4,,15;"
		"1:"
			"movd	4(%0,%4), %%mm1;"
			"pxor	%%mm7, %%mm7;"
			"punpcklbw %%mm7, %%mm1;"	// p1 = pIn[x + 1]
			"movq	%%mm0, %%mm4;"
			"pmullw	%%mm6, %%mm4;"		// t = multiply(p0, c2)
			"movq	%%mm1, %%mm0;"
			"pmullw	%%mm5, %%mm0;"		// t0 = multiply(p1, c1)
			"paddw	%%mm2, %%mm4;"
			"paddw  %%mm0, %%mm4;"
			"psrlw	$8, %%mm4;"		// f0 + t + t0
			"movq	%%mm0, %%mm2;"		// f0 = t0

			"movd	8(%0,%4), %%mm0;"
			"punpcklbw %%mm7, %%mm0;"
			"movq	%%mm1, %%mm7;"
			"pmullw	%%mm6, %%mm7;"		// t = multiply(p1, c2)
			"movq	%%mm0, %%mm1;"
			"pmullw %%mm5, %%mm1;"		// t1 = multiply(p0, c1)
			"paddw	%%mm3, %%mm7;"
			"paddw	%%mm1, %%mm7;"
			"psrlw	$8, %%mm7;"		// f1 + t + t1
			"movq	%%mm1, %%mm3;"		// f1 = t1
			"packuswb %%mm7, %%mm4;"
			"movq	%%mm4, (%1,%4);"	// pOut[x] = ..  pOut[x+1] = ..

			"add	$8, %4;"
			"jnz	1b;"

			"movd	4(%0), %%mm1;"
			"pxor	%%mm7, %%mm7;"
			"punpcklbw %%mm7, %%mm1;"	// p1 = pIn[x + 1]
			"movq	%%mm0, %%mm4;"
			"pmullw	%%mm6, %%mm4;"		// t = multiply(p0, c2)
			"movq	%%mm1, %%mm0;"
			"pmullw	%%mm5, %%mm0;"		// t0 = multiply(p1, c1)
			"paddw	%%mm2, %%mm4;"
			"paddw  %%mm0, %%mm4;"
			"psrlw	$8, %%mm4;"		// f0 + t + t0

			"pmullw	%%mm6, %%mm1;"		// t = multiply(p1, c2)
			"paddw	%%mm3, %%mm1;"
			"paddw	%%mm0, %%mm1;"
			"psrlw	$8, %%mm1;"		// f1 + t + t1
			"packuswb %%mm1, %%mm4;"
			"movq	%%mm4, (%1);"		// pOut[x] = ..  pOut[x+1] = ..

			"emms;"

			: // no output
			: "r" (pIn  + srcWidth - 2) // 0
			, "r" (pOut + srcWidth - 2) // 1
			, "r" (c1)                   // 2
			, "r" (c2)                   // 3
			, "r" (-4 * (srcWidth - 2)) // 4
			#ifdef __MMX__
			: "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7"
			#endif
		);
		return;
	}
	#endif
	#endif

	mult1.setFactor32(c1);
	mult3.setFactor32(c2);

	Pixel p0 = pIn[0];
	Pixel p1;
	unsigned f0 = mult1.mul32(p0);
	unsigned f1 = f0;

	unsigned x;
	for (x = 0; x < (srcWidth - 2); x += 2) {
		p1 = pIn[x + 1];
		unsigned t0 = mult1.mul32(p1);
		pOut[x] = mult1.conv32(f0 + mult3.mul32(p0) + t0);
		f0 = t0;

		p0 = pIn[x + 2];
		unsigned t1 = mult1.mul32(p0);
		pOut[x + 1] = mult1.conv32(f1 + mult3.mul32(p1) + t1);
		f1 = t1;
	}

	p1 = pIn[x + 1];
	unsigned t0 = mult1.mul32(p1);
	pOut[x] = mult1.conv32(f0 + mult3.mul32(p0) + t0);

	pOut[x + 1] = mult1.conv32(f1 + mult3.mul32(p1) + t0);
}

template <class Pixel>
void SimpleScaler<Pixel>::drawScanline(
		const Pixel* in1, const Pixel* in2, Pixel* out, int factor,
		unsigned dstWidth)
{
	if (factor != 255) {
		scanline.draw(in1, in2, out, factor, dstWidth);
	} else {
		Scale_1on1<Pixel> scale;
		scale(in1, out, dstWidth);
	}
}

template <class Pixel>
void SimpleScaler<Pixel>::scale1x1to2x2(FrameSource& src,
	unsigned srcStartY, unsigned /*srcEndY*/, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	dst.lock();
	int blur = settings.getBlurFactor();
	int scanlineFactor = settings.getScanlineFactor();

	unsigned dstY = dstStartY;
	const Pixel* srcLine = src.getLinePtr<Pixel>(srcStartY++, srcWidth);
	Pixel* prevDstLine0 = dst.getLinePtrDirect<Pixel>(dstY++);
	blur1on2(srcLine, prevDstLine0, blur, srcWidth);

	while (dstY < dstEndY - 1) {
		srcLine = src.getLinePtr<Pixel>(srcStartY++, srcWidth);
		Pixel* dstLine0 = dst.getLinePtrDirect<Pixel>(dstY + 1);
		blur1on2(srcLine, dstLine0, blur, srcWidth);

		Pixel* dstLine1 = dst.getLinePtrDirect<Pixel>(dstY);
		drawScanline(prevDstLine0, dstLine0, dstLine1, scanlineFactor,
		             2 * srcWidth);

		prevDstLine0 = dstLine0;
		dstY += 2;
	}

	srcLine = src.getLinePtr<Pixel>(srcStartY++, srcWidth);
	VLA(Pixel, buf, 2 * srcWidth);
	blur1on2(srcLine, buf, blur, srcWidth);

	Pixel* dstLine1 = dst.getLinePtrDirect<Pixel>(dstY);
	drawScanline(prevDstLine0, buf, dstLine1, scanlineFactor, 2 * srcWidth);
}

template <class Pixel>
void SimpleScaler<Pixel>::scale1x1to1x2(FrameSource& src,
	unsigned srcStartY, unsigned /*srcEndY*/, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	dst.lock();
	int blur = settings.getBlurFactor();
	int scanlineFactor = settings.getScanlineFactor();

	unsigned dstY = dstStartY;
	const Pixel* srcLine = src.getLinePtr<Pixel>(srcStartY++, srcWidth);
	Pixel* prevDstLine0 = dst.getLinePtrDirect<Pixel>(dstY++);
	blur1on1(srcLine, prevDstLine0, blur, srcWidth);

	while (dstY < dstEndY - 1) {
		srcLine = src.getLinePtr<Pixel>(srcStartY++, srcWidth);
		Pixel* dstLine0 = dst.getLinePtrDirect<Pixel>(dstY + 1);
		blur1on1(srcLine, dstLine0, blur, srcWidth);

		Pixel* dstLine1 = dst.getLinePtrDirect<Pixel>(dstY + 0);
		drawScanline(prevDstLine0, dstLine0, dstLine1, scanlineFactor,
		             srcWidth);

		prevDstLine0 = dstLine0;
		dstY += 2;
	}

	srcLine = src.getLinePtr<Pixel>(srcStartY++, srcWidth);
	VLA(Pixel, buf, srcWidth);
	blur1on1(srcLine, buf, blur, srcWidth);

	Pixel* dstLine1 = dst.getLinePtrDirect<Pixel>(dstY);
	drawScanline(prevDstLine0, buf, dstLine1, scanlineFactor, srcWidth);
}

// Force template instantiation.
#if HAVE_16BPP
template class SimpleScaler<word>;
#endif
#if HAVE_32BPP
template class SimpleScaler<unsigned>;
#endif

} // namespace openmsx
