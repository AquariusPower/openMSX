// $Id$

/*
Original code: Copyright (C) 2001-2003 Andrea Mazzoleni
openMSX adaptation by Maarten ter Huurne

This file is based on code from the Scale2x project.
This modified version is licensed under GPL; the original code is dual-licensed
under GPL and under a custom license.

Visit the Scale2x site for info:
  http://scale2x.sourceforge.net/
*/

#include "Scale2xScaler.hh"
#include "FrameSource.hh"
#include "OutputSurface.hh"
#include "HostCPU.hh"
#include "openmsx.hh"
#include "build-info.hh"

namespace openmsx {

template <class Pixel>
Scale2xScaler<Pixel>::Scale2xScaler(const PixelOperations<Pixel>& pixelOps)
	: Scaler2<Pixel>(pixelOps)
{
}

template <class Pixel>
void Scale2xScaler<Pixel>::scaleLineHalf_1on2(Pixel* dst,
	const Pixel* src0, const Pixel* src1, const Pixel* src2,
	unsigned long srcWidth)
{
	//   n      m is expaned to a b
	// w m e                    c d
	//   s         a = (w == n) && (s != n) && (e != n) ? n : m
	//             b =   .. swap w/e
	//             c =   .. swap n/s
	//             d =   .. swap w/e  n/s

	#ifdef ASM_X86
	const HostCPU& cpu = HostCPU::getInstance();
	if ((sizeof(Pixel) == 4) && cpu.hasSSE()) {
		asm (
			"movq	(%0,%4), %%mm1;"     // m1 | e1  or  w2 | m2
			"pshufw	$238, %%mm1, %%mm0;" // xx | w1
			".p2align 4,,15;"
		"0:"
			"movq	(%1,%4), %%mm2;"     // n1 | n2
			"movq	%%mm2, %%mm3;"       // n1 | n2
			"pshufw	$68, %%mm2, %%mm5;"  // n1 | n1
			"pcmpeqd (%2,%4), %%mm3;"    // n1 = s1 | n2 = s2
			"movq	%%mm0, %%mm7;"
			"pshufw	$68, %%mm3, %%mm4;"  // n1 = s1 | n1 = s1
			"punpckhdq %%mm1, %%mm7;"    // w1 | e1
			"pcmpeqd %%mm5, %%mm7;"      // n1 = w1 | n1 = e1
			"pandn	%%mm7, %%mm4;"       // n1 = w1 & n1 != s1 |
						     // n1 = e1 & n1 != s1
			"pshufw $78, %%mm7, %%mm6;"  // n1 = e1 | n1 = w1
			"pandn	%%mm4, %%mm6;"       // n1 = w1 & n1 != s1 & n1 != e1 |
						     // n1 = e1 & n1 != s1 & n1 != w1
			"pshufw	$68, %%mm1, %%mm7;"  // m1 | m1
			"pand	%%mm6, %%mm5;"       // c  & n1
			"movq	8(%0,%4), %%mm0;"    // e2 | xx
			"pandn	%%mm7, %%mm6;"       // !c & m1
			"por	%%mm6, %%mm5;"       // c ? n1 : m1

			"pshufw	$238, %%mm2, %%mm7;" // n2 | n2
			"movq	%%mm1, %%mm2;"       // w2 | xx
			"pshufw	$238, %%mm3, %%mm4;" // n2 = s2 | n2 = s2
			"punpckldq %%mm0, %%mm2;"    // w2 | e2
			"pcmpeqd %%mm7, %%mm2;"      // n2 = w2 | n2 = e2
			"pandn	%%mm2, %%mm4;"       // n2 = w2 & n2 != s2 |
						     // n2 = e2 & n2 != s2
			"pshufw $78, %%mm2, %%mm6;"  // n2 = e2 | n2 = w2
			"pandn	%%mm4, %%mm6;"       // n2 = w2 & n2 != s2 & n2 != e2 |
						     // n2 = e2 & n2 != s2 & n2 != w2
			"pshufw	$238, %%mm1, %%mm3;" // m2 | m2
			"pand	%%mm6, %%mm7;"       // c  & n2
			"movq	%%mm0, %%mm2;"
			"pandn  %%mm3, %%mm6;"       // !c & m2
			"movq	%%mm1, %%mm0;"
			"por	%%mm6, %%mm7;"       // c ? n2 : m2
			"movq	%%mm2, %%mm1;"       // swap mm0,mm1

			"movntq	%%mm5,  (%3,%4,2);"
			"movntq	%%mm7, 8(%3,%4,2);"

			"add	$8, %4;"
			"jnz	0b;"

			// last pixel
			"movq	(%1), %%mm2;"        // n1 | n2
			"movq	%%mm2, %%mm3;"       // n1 | n2
			"pshufw	$78, %%mm2, %%mm5;"  // n1 | n1
			"pcmpeqd (%2), %%mm3;"       // n1 = s1 | n2 = s2
			"movq	%%mm0, %%mm7;"
			"pshufw	$78, %%mm3, %%mm4;"  // n1 = s1 | n1 = s1
			"punpckhdq %%mm1, %%mm7;"    // w1 | e1
			"pcmpeqd %%mm5, %%mm7;"      // n1 = w1 | n1 = e1
			"pandn	%%mm7, %%mm4;"       // n1 = w1 & n1 != s1 |
						     // n1 = e1 & n1 != s1
			"pshufw $78, %%mm7, %%mm6;"  // n1 = e1 | n1 = w1
			"pandn	%%mm4, %%mm6;"       // n1 = w1 & n1 != s1 & n1 != e1 |
						     // n1 = e1 & n1 != s1 & n1 != w1
			"pshufw	$68, %%mm1, %%mm7;"  // m1 | m1
			"pand	%%mm6, %%mm5;"       // c  & n1
			"pandn	%%mm7, %%mm6;"       // !c & m1
			"pshufw	$238, %%mm1, %%mm0;" // e2 | xx
			"por	%%mm6, %%mm5;"       // c ? n1 : m1

			"pshufw	$238, %%mm2, %%mm7;" // n2 | n2
			"movq	%%mm1, %%mm2;"       // w2 | xx
			"pshufw	$238, %%mm3, %%mm4;" // n2 = s2 | n2 = s2
			"punpckldq %%mm0, %%mm2;"    // w2 | e2
			"pcmpeqd %%mm7, %%mm2;"      // n2 = w2 | n2 = e2
			"pandn	%%mm2, %%mm4;"       // n2 = w2 & n2 != s2 |
						     // n2 = e2 & n2 != s2
			"pshufw $78, %%mm2, %%mm6;"  // n2 = e2 | n2 = w2
			"pandn	%%mm4, %%mm6;"       // n2 = w2 & n2 != s2 & n2 != e2 |
						     // n2 = e2 & n2 != s2 & n2 != w2
			"pshufw	$238, %%mm1, %%mm3;" // m2 | m2
			"pand	%%mm6, %%mm7;"       // c  & n2
			"pandn  %%mm3, %%mm6;"       // !c & m2
			"por	%%mm6, %%mm7;"       // c ? n2 : m2

			"movntq	%%mm5,  (%3);"
			"movntq	%%mm7, 8(%3);"

			"emms;"

			: // no output
			: "r" (src1 + srcWidth - 2) // 0
			, "r" (src0 + srcWidth - 2) // 1
			, "r" (src2 + srcWidth - 2) // 2
			, "r" (dst + 2 * (srcWidth - 2)) // 3
			, "r" (-4 * (srcWidth - 2)) // 4
			#ifdef __MMX__
			: "mm0", "mm1", "mm2", "mm3"
			, "mm4", "mm5", "mm6", "mm7"
			#endif
		);
		return;
	};
	if ((sizeof(Pixel) == 4) && cpu.hasMMX()) {
		asm (
			"movq	(%0,%4), %%mm1;"     // m1 | e1  or  w2 | m2
			"movq	%%mm1, %%mm0;"       // w1 | xx
			"punpckldq %%mm0, %%mm0;"    // xx | w1
			".p2align 4,,15;"
		"0:"
			"movq	(%1,%4), %%mm2;"     // n1 | n2
			"movq	%%mm2, %%mm3;"       // n1 | n2
			"movq	%%mm2, %%mm5;"       // n1 | xx
			"pcmpeqd (%2,%4), %%mm3;"    // n1 = s1 | n2 = s2
			"punpckldq %%mm5, %%mm5;"    // n1 | n1
			"movq	%%mm3, %%mm4;"       // n1 = s1 | n2 = s2
			"movq	%%mm0, %%mm7;"       // xx | w1
			"punpckldq %%mm4, %%mm4;"    // n1 = s1 | n1 = s1
			"punpckhdq %%mm1, %%mm7;"    // w1 | e1
			"movq	%%mm1, %%mm6;"       // xx | e1
			"pcmpeqd %%mm5, %%mm7;"      // n1 = w1 | n1 = e1
			"punpckhdq %%mm0, %%mm6;"    // e1 | w1
			"pandn	%%mm7, %%mm4;"       // n1 = w1 & n1 != s1 |
						     // n1 = e1 & n1 != s1
			"pcmpeqd %%mm5, %%mm6;"      // n1 = e1 | n1 = w1
			"movq	%%mm1, %%mm7;"       // m1 | xx
			"pandn	%%mm4, %%mm6;"       // n1 = w1 & n1 != s1 & n1 != e1 |
						     // n1 = e1 & n1 != s1 & n1 != w1
			"punpckldq %%mm7, %%mm7;"    // m1 | m1
			"pand	%%mm6, %%mm5;"       // c  & n1
			"movq	8(%0,%4), %%mm0;"    // e2 | xx
			"pandn	%%mm7, %%mm6;"       // !c & m1
			"movq	%%mm2, %%mm7;"       // xx | n2
			"por	%%mm6, %%mm5;"       // c ? n1 : m1

			"punpckhdq %%mm7, %%mm7;"    // n2 | n2
			"movq	%%mm1, %%mm2;"       // w2 | xx
			"punpckhdq %%mm3, %%mm3;"    // n2 = s2 | n2 = s2
			"punpckldq %%mm0, %%mm2;"    // w2 | e2
			"movq	%%mm0, %%mm6;"       // e2 | xx
			"pcmpeqd %%mm7, %%mm2;"      // n2 = w2 | n2 = e2
			"punpckldq %%mm1, %%mm6;"    // e2 | w2
			"pandn	%%mm2, %%mm3;"       // n2 = w2 & n2 != s2 |
						     // n2 = e2 & n2 != s2
			"pcmpeqd %%mm7, %%mm6;"      // n2 = e2 | n2 = w2
			"movq	%%mm1, %%mm4;"       // xx | m2
			"pandn	%%mm3, %%mm6;"       // n2 = w2 & n2 != s2 & n2 != e2 |
						     // n2 = e2 & n2 != s2 & n2 != w2
			"punpckhdq %%mm4, %%mm4;"    // m2 | m2
			"pand	%%mm6, %%mm7;"       // c  & n2
			"movq	%%mm0, %%mm2;"
			"pandn  %%mm4, %%mm6;"       // !c & m2
			"movq	%%mm1, %%mm0;"
			"por	%%mm6, %%mm7;"       // c ? n2 : m2
			"movq	%%mm2, %%mm1;"       // swap mm0,mm1

			"movq	%%mm5,  (%3,%4,2);"
			"movq	%%mm7, 8(%3,%4,2);"

			"add	$8, %4;"
			"jnz	0b;"

			// last pixel
			"movq	(%1), %%mm2;"        // n1 | n2
			"movq	%%mm2, %%mm3;"       // n1 | n2
			"movq	%%mm2, %%mm5;"       // n1 | xx
			"pcmpeqd (%2), %%mm3;"       // n1 = s1 | n2 = s2
			"punpckldq %%mm5, %%mm5;"    // n1 | n1
			"movq	%%mm3, %%mm4;"	     // n1 = s1 | n2 = s2
			"movq	%%mm0, %%mm7;"       // xx | w1
			"punpckldq %%mm4, %%mm4;"    // n1 = s1 | n1 = s1
			"punpckhdq %%mm1, %%mm7;"    // w1 | e1
			"movq	%%mm1, %%mm6;"       // xx | e1
			"pcmpeqd %%mm5, %%mm7;"      // n1 = w1 | n1 = e1
			"punpckhdq %%mm0, %%mm6;"    // e1 | w1
			"pandn	%%mm7, %%mm4;"       // n1 = w1 & n1 != s1 |
						     // n1 = e1 & n1 != s1
			"pcmpeqd %%mm5, %%mm6;"      // n1 = e1 | n1 = w1
			"movq	%%mm1, %%mm7;"       // m1 | xx
			"pandn	%%mm4, %%mm6;"       // n1 = w1 & n1 != s1 & n1 != e1 |
						     // n1 = e1 & n1 != s1 & n1 != w1
			"punpckldq %%mm7, %%mm7;"    // m1 | m1
			"pand	%%mm6, %%mm5;"       // c  & n1
			"movq	%%mm1, %%mm0;"       // xx | e2
			"pandn	%%mm7, %%mm6;"       // !c & m1
			"punpckhdq %%mm0, %%mm0;"    // e2 | xx
			"movq	%%mm2, %%mm7;"       // xx | n2
			"por	%%mm6, %%mm5;"       // c ? n1 : m1

			"punpckhdq %%mm7, %%mm7;"    // n2 | n2
			"movq	%%mm1, %%mm2;"       // w2 | xx
			"punpckhdq %%mm3, %%mm3;"    // n2 = s2 | n2 = s2
			"punpckldq %%mm0, %%mm2;"    // w2 | e2
			"movq	%%mm0, %%mm6;"       // e2 | xx
			"pcmpeqd %%mm7, %%mm2;"      // n2 = w2 | n2 = e2
			"punpckldq %%mm1, %%mm6;"    // e2 | w2
			"pandn	%%mm2, %%mm3;"       // n2 = w2 & n2 != s2 |
						     // n2 = e2 & n2 != s2
			"pcmpeqd %%mm7, %%mm6;"      // n2 = e2 | n2 = w2
			"movq	%%mm1, %%mm4;"       // xx | m2
			"pandn	%%mm3, %%mm6;"       // n2 = w2 & n2 != s2 & n2 != e2 |
						     // n2 = e2 & n2 != s2 & n2 != w2
			"punpckhdq %%mm4, %%mm4;"    // m2 | m2
			"pand	%%mm6, %%mm7;"       // c  & n2
			"pandn  %%mm4, %%mm6;"       // !c & m2
			"por	%%mm6, %%mm7;"       // c ? n2 : m2

			"movq	%%mm5,  (%3);"
			"movq	%%mm7, 8(%3);"

			"emms;"

			: // no output
			: "r" (src1 + srcWidth - 2) // 0
			, "r" (src0 + srcWidth - 2) // 1
			, "r" (src2 + srcWidth - 2) // 2
			, "r" (dst + 2 * (srcWidth - 2)) // 3
			, "r" (-4 * (srcWidth - 2)) // 4
			#ifdef __MMX__
			: "mm0", "mm1", "mm2", "mm3"
			, "mm4", "mm5", "mm6", "mm7"
			#endif
		);
		return;
	};
	if ((sizeof(Pixel) == 2) && cpu.hasSSE()) {
		//           mm2: abcd
		//mm0: xxx0  mm1: 1234  mm0: 5xxx
		//                efgh
		asm (
			"movq	(%0,%4), %%mm1;"     // 1 2 3 4
			"pshufw	$0, %%mm1, %%mm0;"   // x x x 1
			".p2align 4,,15;"
		"0:"
			"movq	(%1,%4), %%mm2;"     // a b c d
			"movq	%%mm2, %%mm3;"       // a b c d
			"pcmpeqw (%2,%4), %%mm3;"    // a=e b=f c=g d=h
			"pshufw	$33, %%mm1, %%mm7;"  // 2 1 3 x
			"movq	%%mm0, %%mm6;"       // x x x 0
			"pshufw	$80, %%mm3, %%mm4;"  // a=e a=e b=f b=f
			"psrlq	$48, %%mm6;"         // 0 x x x
			"psllq	$16, %%mm7;"         // x 2 1 3
			"pshufw	$80, %%mm2, %%mm5;"  // a a b b
			"por	%%mm7, %%mm6;"       // 0 2 1 3
			"pcmpeqw %%mm5, %%mm6;"      // 0=a 2=a 1=b 3=b
			"pshufw	$177, %%mm6, %%mm7;" // 2=a 0=a 3=b 1=b
			"pandn	%%mm6, %%mm4;"       // 0=a & a!=e  .. .. ..
			"pandn	%%mm4, %%mm7;"       // 0=a & a!=e & 2!=a .. .. ..
			"pshufw	$80, %%mm1, %%mm6;"  // 1 1 2 2
			"pand	%%mm7, %%mm5;"       //  cond & (a a b b)
			"pandn	%%mm6, %%mm7;"       // !cond & (1 1 2 2)
			"por	%%mm5, %%mm7;"       // cond ? (a a b b) : (1 1 2 2)

			"movq	8(%0,%4), %%mm0;"    // 5 x x x
			"pshufw	$180, %%mm1, %%mm4;" // x 2 4 3
			"movq	%%mm0, %%mm6;"       // 5 x x x
			"pshufw	$250, %%mm3, %%mm3;" // c=g c=g d=h d=h
			"psllq	$48,  %%mm6;"        // x x x 5
			"psrlq	$16,  %%mm4;"        // 2 4 3 x
			"pshufw	$250, %%mm2, %%mm5;" // c c d d
			"por	%%mm4, %%mm6;"       // 2 4 3 5
			"pcmpeqw %%mm5, %%mm6;"      // 2=c 4=c 3=d 5=d
			"pshufw	$177, %%mm6, %%mm4;" // 4=c 2=c 5=d 3=d
			"pandn	%%mm6, %%mm3;"       // 2=c & c!=g .. .. ..
			"pandn	%%mm3, %%mm4;"       // 2=c & c!=g & 4!=c
			"pshufw	$250, %%mm1, %%mm6;" // 3 3 4 4
			"pand	%%mm4, %%mm5;"       //  cond & (c c d d)
			"pandn  %%mm6, %%mm4;"       // !cond & (3 3 4 4)
			"por	%%mm5, %%mm4;"       // cond ? (c c d d) : (3 3 4 4)

			"movntq	%%mm7,  (%3,%4,2);"
			"movntq	%%mm4, 8(%3,%4,2);"

			"movq	%%mm0, %%mm2;"
			"movq	%%mm1, %%mm0;"
			"movq	%%mm2, %%mm1;"       // swap mm0,mm1

			"add	$8, %4;"
			"jnz	0b;"

			// last pixel
			"movq	(%1), %%mm2;"        // a b c d
			"movq	%%mm2, %%mm3;"       // a b c d
			"pcmpeqw (%2), %%mm3;"       // a=e b=f c=g d=h
			"pshufw	$33, %%mm1, %%mm7;"  // 2 1 3 x
			"pshufw	$80, %%mm3, %%mm4;"  // a=e a=e b=f b=f
			"movq	%%mm0, %%mm6;"       // x x x 0
			"psrlq	$48, %%mm6;"         // 0 x x x
			"psllq	$16, %%mm7;"         // x 2 1 3
			"pshufw	$80, %%mm2, %%mm5;"  // a a b b
			"por	%%mm7, %%mm6;"       // 0 2 1 3
			"pcmpeqw %%mm5, %%mm6;"      // 0=a 2=a 1=b 3=b
			"pshufw	$177, %%mm6, %%mm7;" // 2=a 0=a 3=b 1=b
			"pandn	%%mm6, %%mm4;"       // 0=a & a!=e  .. .. ..
			"pandn	%%mm4, %%mm7;"       // 0=a & a!=e & 2!=a .. .. ..
			"pshufw	$80, %%mm1, %%mm6;"  // 1 1 2 2
			"pand	%%mm7, %%mm5;"       //  cond & (a a b b)
			"pandn	%%mm6, %%mm7;"       // !cond & (1 1 2 2)
			"por	%%mm5, %%mm7;"       // cond ? (a a b b) : (1 1 2 2)

			"pshufw $255, %%mm1, %%mm0;" // 5 x x x
			"pshufw	$180, %%mm1, %%mm4;" // x 2 4 3
			"pshufw	$175, %%mm3, %%mm3;" // c=g c=g d=h d=h
			"movq	%%mm0, %%mm6;"       // 5 x x x
			"psllq	$48,  %%mm6;"        // x x x 5
			"psrlq	$16,  %%mm4;"        // 2 4 3 x
			"pshufw	$175, %%mm2, %%mm5;" // c c d d
			"por	%%mm4, %%mm6;"       // 2 4 3 5
			"pcmpeqw %%mm5, %%mm6;"      // 2=c 4=c 3=d 5=d
			"pshufw	$177, %%mm6, %%mm4;" // 4=c 2=c 5=d 3=d
			"pandn	%%mm6, %%mm3;"       // 2=c & c!=g .. .. ..
			"pandn	%%mm3, %%mm4;"       // 2=c & c!=g & 4!=c
			"pshufw	$175, %%mm1, %%mm6;" // 3 3 4 4
			"pand	%%mm4, %%mm5;"       //  cond & (c c d d)
			"pandn  %%mm6, %%mm4;"       // !cond & (3 3 4 4)
			"por	%%mm5, %%mm4;"       // cond ? (c c d d) : (3 3 4 4)

			"movntq	%%mm7,  (%3);"
			"movntq	%%mm4, 8(%3);"

			"emms;"

			: // no output
			: "r" (src1 + srcWidth - 4) // 0
			, "r" (src0 + srcWidth - 4) // 1
			, "r" (src2 + srcWidth - 4) // 2
			, "r" (dst + 2 * (srcWidth - 4)) // 3
			, "r" (-2 * (srcWidth - 4)) // 4
			#ifdef __MMX__
			: "mm0", "mm1", "mm2", "mm3"
			, "mm4", "mm5", "mm6", "mm7"
			#endif
		);
		return;
	};
	if ((sizeof(Pixel) == 2) && cpu.hasMMX()) {
		//           mm2: abcd
		//mm0: xxx0  mm1: 1234  mm0: 5xxx
		//                efgh
		asm (
			"movq	(%0,%4), %%mm1;"     // 1 2 3 4
			"movq	%%mm1, %%mm0;"       // 1 x x x
			"psllq	$48, %%mm0;"         // x x x 1
			".p2align 4,,15;"
		"0:"
			"movq	(%1,%4), %%mm2;"     // a b c d
			"movq	%%mm2, %%mm3;"       // a b c d
			"pcmpeqw (%2,%4), %%mm3;"    // a=e b=f c=g d=h

			"movq	%%mm0, %%mm6;"       // x x x 0
			"psrlq	$48, %%mm6;"         // 0 x x x
			"movq	%%mm1, %%mm7;"       // 1 2 3 4
			"psllq	$16, %%mm7;"         // x 1 2 3
			"pxor	%%mm4, %%mm4;"       // x x x x
			"punpckhwd %%mm7, %%mm4;"    // x 2 x 3
			"pxor	%%mm5, %%mm5;"       // x x x x
			"punpcklwd %%mm5, %%mm7;"    // x x 1 x
			"por	%%mm4, %%mm7;"       // x 2 1 3
			"por	%%mm7, %%mm6;"       // 0 2 1 3
			"movq	%%mm2, %%mm5;"       // a b c d
			"punpcklwd %%mm5, %%mm5;"    // a a b b
			"pcmpeqw %%mm5, %%mm6;"      // 0=a 2=a 1=b 3=b

			"movq	%%mm1, %%mm7;"       // 1 2 3 4
			"psllq	$16, %%mm7;"         // x 1 2 3
			"pxor	%%mm5, %%mm5;"       // x x x x
			"punpcklwd %%mm7, %%mm5;"    // x x x 1
			"pxor	%%mm4, %%mm4;"       // x x x x
			"punpckhwd %%mm4, %%mm7;"    // 2 x 3 x
			"por	%%mm5, %%mm7;"       // 2 x 3 1
			"movq	%%mm0, %%mm4;"       // . . . 0
			"psrlq	$48, %%mm4;"         // 0 x x x
			"psllq	$16, %%mm4;"         // x 0 x x
			"por	%%mm4, %%mm7;"       // 2 0 3 1
			"movq	%%mm2, %%mm5;"       // a b c d
			"punpcklwd %%mm5, %%mm5;"    // a a b b
			"pcmpeqw %%mm5, %%mm7;"      // 2=a 0=a 3=b 1=b

			"movq	%%mm3, %%mm4;"       // a=e b=f c=g d=h
			"punpcklwd %%mm4, %%mm4;"    // a=e a=e b=f b=f
			"pandn	%%mm6, %%mm4;"       // 0=a & a!=e  .. .. ..
			"pandn	%%mm4, %%mm7;"       // 0=a & a!=e & 2!=a .. .. ..
			"movq	%%mm1, %%mm6;"       // 1 2 3 4
			"punpcklwd %%mm6, %%mm6;"    // 1 1 2 2
			"pand	%%mm7, %%mm5;"       //  cond & (a a b b)
			"pandn	%%mm6, %%mm7;"       // !cond & (1 1 2 2)
			"por	%%mm5, %%mm7;"       // cond ? (a a b b) : (1 1 2 2)
			"movq	%%mm7,  (%3,%4,2);"

			"movq	8(%0,%4), %%mm0;"    // 5 x x x
			"punpckhwd %%mm3, %%mm3;"    // c=g c=g d=h d=h

			"movq	%%mm0, %%mm6;"       // 5 x x x
			"psllq	$48,  %%mm6;"        // x x x 5
			"movq	%%mm1, %%mm7;"       // 1 2 3 4
			"psrlq	$16, %%mm7;"         // 2 3 4 x
			"pxor	%%mm4, %%mm4;"       // x x x x
			"punpckhwd %%mm7, %%mm4;"    // x 4 x x
			"pxor	%%mm5, %%mm5;"       // x x x x
			"punpcklwd %%mm5, %%mm7;"    // 2 x 3 x
			"por	%%mm4, %%mm7;"       // 2 4 3 x
			"por	%%mm7, %%mm6;"       // 2 4 3 5
			"movq	%%mm2, %%mm5;"       // a b c d
			"punpckhwd %%mm5, %%mm5;"    // c c d d
			"pcmpeqw %%mm5, %%mm6;"      // 2=c 4=c 3=d 5=d

			"movq	%%mm1, %%mm4;"       // 1 2 3 4
			"psrlq	$16, %%mm4;"         // 2 3 4 x
			"pxor	%%mm5, %%mm5;"       // x x x x
			"punpcklwd %%mm4, %%mm5;"    // x 2 x 3
			"pxor	%%mm7, %%mm7;"       // x x x x
			"punpckhwd %%mm7, %%mm4;"    // 4 x x x
			"por	%%mm5, %%mm4;"       // 4 2 x 3
			"movq	%%mm0, %%mm7;"       // 5 . . .
			"psllq	$48, %%mm7;"         // x x x 5
			"psrlq	$16, %%mm7;"         // x x 5 x
			"por	%%mm7, %%mm4;"       // 4 2 5 3
			"movq	%%mm2, %%mm5;"       // a b c d
			"punpckhwd %%mm5, %%mm5;"    // c c d d
			"pcmpeqw %%mm5, %%mm4;"      // 4=c 2=c 5=d 3=d

			"pandn	%%mm6, %%mm3;"       // 2=c & c!=g .. .. ..
			"pandn	%%mm3, %%mm4;"       // 2=c & c!=g & 4!=c
			"movq	%%mm1, %%mm6;"       // 1 2 3 4
			"punpckhwd %%mm6, %%mm6;"    // 3 3 4 4
			"pand	%%mm4, %%mm5;"       //  cond & (c c d d)
			"pandn  %%mm6, %%mm4;"       // !cond & (3 3 4 4)
			"por	%%mm5, %%mm4;"       // cond ? (c c d d) : (3 3 4 4)

			"movq	%%mm4, 8(%3,%4,2);"

			"movq	%%mm0, %%mm2;"
			"movq	%%mm1, %%mm0;"
			"movq	%%mm2, %%mm1;"       // swap mm0,mm1

			"add	$8, %4;"
			"jnz	0b;"

			// last pixel
			"movq	(%1), %%mm2;"        // a b c d
			"movq	%%mm2, %%mm3;"       // a b c d
			"pcmpeqw (%2), %%mm3;"       // a=e b=f c=g d=h

			"movq	%%mm0, %%mm6;"       // x x x 0
			"psrlq	$48, %%mm6;"         // 0 x x x
			"movq	%%mm1, %%mm7;"       // 1 2 3 4
			"psllq	$16, %%mm7;"         // x 1 2 3
			"pxor	%%mm4, %%mm4;"       // x x x x
			"punpckhwd %%mm7, %%mm4;"    // x 2 x 3
			"pxor	%%mm5, %%mm5;"       // x x x x
			"punpcklwd %%mm5, %%mm7;"    // x x 1 x
			"por	%%mm4, %%mm7;"       // x 2 1 3
			"por	%%mm7, %%mm6;"       // 0 2 1 3
			"movq	%%mm2, %%mm5;"       // a b c d
			"punpcklwd %%mm5, %%mm5;"    // a a b b
			"pcmpeqw %%mm5, %%mm6;"      // 0=a 2=a 1=b 3=b

			"movq	%%mm1, %%mm7;"       // 1 2 3 4
			"psllq	$16, %%mm7;"         // x 1 2 3
			"pxor	%%mm5, %%mm5;"       // x x x x
			"punpcklwd %%mm7, %%mm5;"    // x x x 1
			"pxor	%%mm4, %%mm4;"       // x x x x
			"punpckhwd %%mm4, %%mm7;"    // 2 x 3 x
			"por	%%mm5, %%mm7;"       // 2 x 3 1
			"movq	%%mm0, %%mm4;"       // . . . 0
			"psrlq	$48, %%mm4;"         // 0 x x x
			"psllq	$16, %%mm4;"         // x 0 x x
			"por	%%mm4, %%mm7;"       // 2 0 3 1
			"movq	%%mm2, %%mm5;"       // a b c d
			"punpcklwd %%mm5, %%mm5;"    // a a b b
			"pcmpeqw %%mm5, %%mm7;"      // 2=a 0=a 3=b 1=b

			"movq	%%mm3, %%mm4;"       // a=e b=f c=g d=h
			"punpcklwd %%mm4, %%mm4;"    // a=e a=e b=f b=f
			"pandn	%%mm6, %%mm4;"       // 0=a & a!=e  .. .. ..
			"pandn	%%mm4, %%mm7;"       // 0=a & a!=e & 2!=a .. .. ..
			"movq	%%mm1, %%mm6;"       // 1 2 3 4
			"punpcklwd %%mm6, %%mm6;"    // 1 1 2 2
			"pand	%%mm7, %%mm5;"       //  cond & (a a b b)
			"pandn	%%mm6, %%mm7;"       // !cond & (1 1 2 2)
			"por	%%mm5, %%mm7;"       // cond ? (a a b b) : (1 1 2 2)
			"movq	%%mm7,  (%3);"

			"movq	%%mm1, %%mm0;"       // x x x 5
			"psrlq  $48, %%mm0;"         // 5 x x x
			"punpckhwd %%mm3, %%mm3;"    // c=g c=g d=h d=h

			"movq	%%mm0, %%mm6;"       // 5 x x x
			"psllq	$48,  %%mm6;"        // x x x 5
			"movq	%%mm1, %%mm7;"       // 1 2 3 4
			"psrlq	$16, %%mm7;"         // 2 3 4 x
			"pxor	%%mm4, %%mm4;"       // x x x x
			"punpckhwd %%mm7, %%mm4;"    // x 4 x x
			"pxor	%%mm5, %%mm5;"       // x x x x
			"punpcklwd %%mm5, %%mm7;"    // 2 x 3 x
			"por	%%mm4, %%mm7;"       // 2 4 3 x
			"por	%%mm7, %%mm6;"       // 2 4 3 5
			"movq	%%mm2, %%mm5;"       // a b c d
			"punpckhwd %%mm5, %%mm5;"    // c c d d
			"pcmpeqw %%mm5, %%mm6;"      // 2=c 4=c 3=d 5=d

			"movq	%%mm1, %%mm4;"       // 1 2 3 4
			"psrlq	$16, %%mm4;"         // 2 3 4 x
			"pxor	%%mm5, %%mm5;"       // x x x x
			"punpcklwd %%mm4, %%mm5;"    // x 2 x 3
			"pxor	%%mm7, %%mm7;"       // x x x x
			"punpckhwd %%mm7, %%mm4;"    // 4 x x x
			"por	%%mm5, %%mm4;"       // 4 2 x 3
			"movq	%%mm0, %%mm7;"       // 5 . . .
			"psllq	$48, %%mm7;"         // x x x 5
			"psrlq	$16, %%mm7;"         // x x 5 x
			"por	%%mm7, %%mm4;"       // 4 2 5 3
			"movq	%%mm2, %%mm5;"       // a b c d
			"punpckhwd %%mm5, %%mm5;"    // c c d d
			"pcmpeqw %%mm5, %%mm4;"      // 4=c 2=c 5=d 3=d

			"pandn	%%mm6, %%mm3;"       // 2=c & c!=g .. .. ..
			"pandn	%%mm3, %%mm4;"       // 2=c & c!=g & 4!=c
			"movq	%%mm1, %%mm6;"       // 1 2 3 4
			"punpckhwd %%mm6, %%mm6;"    // 3 3 4 4
			"pand	%%mm4, %%mm5;"       //  cond & (c c d d)
			"pandn  %%mm6, %%mm4;"       // !cond & (3 3 4 4)
			"por	%%mm5, %%mm4;"       // cond ? (c c d d) : (3 3 4 4)

			"movq	%%mm4, 8(%3);"

			"emms;"

			: // no output
			: "r" (src1 + srcWidth - 4) // 0
			, "r" (src0 + srcWidth - 4) // 1
			, "r" (src2 + srcWidth - 4) // 2
			, "r" (dst + 2 * (srcWidth - 4)) // 3
			, "r" (-2 * (srcWidth - 4)) // 4
			#ifdef __MMX__
			: "mm0", "mm1", "mm2", "mm3"
			, "mm4", "mm5", "mm6", "mm7"
			#endif
		);
		return;
	};
	#endif

	// First pixel.
	Pixel mid   = src1[0];
	Pixel right = src1[1];
	dst[0] = mid;
	dst[1] = (right == src0[0] && src2[0] != src0[0]) ? src0[0] : mid;

	// Central pixels.
	for (unsigned x = 1; x < srcWidth - 1; ++x) {
		Pixel left = mid;
		mid   = right;
		right = src1[x + 1];
		Pixel top = src0[x];
		Pixel bot = src2[x];
		dst[2 * x + 0] = (left  == top && right != top && bot != top) ? top : mid;
		dst[2 * x + 1] = (right == top && left  != top && bot != top) ? top : mid;
	}

	// Last pixel.
	dst[2 * srcWidth - 2] =
		(mid == src0[srcWidth - 1] && src2[srcWidth - 1] != src0[srcWidth - 1])
		? src0[srcWidth - 1] : right;
	dst[2 * srcWidth - 1] =
		src1[srcWidth - 1];
}

template <class Pixel>
void Scale2xScaler<Pixel>::scaleLineHalf_1on1(Pixel* dst,
	const Pixel* src0, const Pixel* src1, const Pixel* src2,
	unsigned long srcWidth)
{
	//    ab ef
	// x0 12 34 5x
	//    cd gh

	#ifdef ASM_X86
	const HostCPU& cpu = HostCPU::getInstance();
	if ((sizeof(Pixel) == 4) && cpu.hasSSE()) {
		asm (
			"movq	(%0,%4), %%mm0;"     // 1 2
			"pshufw	$68, %%mm0, %%mm2;"  // 1 1
			".p2align 4,,15;"
		"0:"
			"movq	(%1,%4), %%mm4;"     // a b
			"pshufw	$238, %%mm0, %%mm3;" // 2 2
			"pcmpeqd %%mm4, %%mm2;"      // a=0 b=1
			"movq	(%2,%4), %%mm5;"     // c d
			"movq	%%mm4, %%mm6;"       // a b
			"pcmpeqd %%mm4, %%mm5;"      // a=c b=d
			"movq	8(%0,%4), %%mm1;"    // 3 4
			"pandn	%%mm2, %%mm5;"       // a=0 & a!=c
			                             // b=1 & b!=d
			"punpckldq %%mm1, %%mm3;"    // 2 3
			"pcmpeqd %%mm3, %%mm6;"      // a=2 b=3
			"pandn	%%mm5, %%mm6;"       // a=0 & a!=c & a!=2
			                             // b=1 & b!=d & b!=3
			"pand	%%mm6, %%mm4;"       //  cond & (a b)
			"pandn	%%mm0, %%mm6;"       // !cond & (1 2)
			"por	%%mm4, %%mm6;"       // cond ? (a b) : (1 2)

			"movq	8(%1,%4), %%mm4;"    // e f
			"pshufw	$238, %%mm1, %%mm2;" // 4 4
			"pcmpeqd %%mm4, %%mm3;"      // e=2 f=3
			"movq	8(%2,%4), %%mm5;"    // g h
			"movq	%%mm4, %%mm7;"       // e f
			"pcmpeqd %%mm4, %%mm5;"      // e=g f=h
			"movq	16(%0,%4), %%mm0;"   // 5 6
			"pandn	%%mm3, %%mm5;"       // e=2 & e!=g
			                             // f=3 & f!=h
			"punpckldq %%mm0, %%mm2;"    // 4 5
			"pcmpeqd %%mm2, %%mm7;"      // e=4 f=5
			"pandn	%%mm5, %%mm7;"       // e=2 & e!=g & e!=4
			                             // f=3 & f!=h & f!=5
			"pand	%%mm7, %%mm4;"       //  cond & (e f)
			"pandn	%%mm1, %%mm7;"       // !cond & (3 4)
			"por	%%mm4, %%mm7;"       // cond ? (e f) : (3 4)

			"movntq	%%mm6,  (%3,%4);"
			"movntq	%%mm7, 8(%3,%4);"

			"add	$16, %4;"
			"jnz	0b;"

			"movq	(%1), %%mm4;"        // a b
			"pshufw	$238, %%mm0, %%mm3;" // 2 2
			"pcmpeqd %%mm4, %%mm2;"      // a=0 b=1
			"movq	(%2), %%mm5;"        // c d
			"movq	%%mm4, %%mm6;"       // a b
			"pcmpeqd %%mm4, %%mm5;"      // a=c b=d
			"movq	8(%0), %%mm1;"       // 3 4
			"pandn	%%mm2, %%mm5;"       // a=0 & a!=c
			                             // b=1 & b!=d
			"punpckldq %%mm1, %%mm3;"    // 2 3
			"pcmpeqd %%mm3, %%mm6;"      // a=2 b=3
			"pandn	%%mm5, %%mm6;"       // a=0 & a!=c & a!=2
			                             // b=1 & b!=d & b!=3
			"pand	%%mm6, %%mm4;"       //  cond & (a b)
			"pandn	%%mm0, %%mm6;"       // !cond & (1 2)
			"por	%%mm4, %%mm6;"       // cond ? (a b) : (1 2)

			"movq	8(%1), %%mm4;"       // e f
			"pshufw	$238, %%mm1, %%mm2;" // 4 4
			"pcmpeqd %%mm4, %%mm3;"      // e=2 f=3
			"movq	8(%2), %%mm5;"       // g h
			"movq	%%mm4, %%mm7;"       // e f
			"pcmpeqd %%mm4, %%mm5;"      // e=g f=h
			"movq	%%mm2, %%mm0;"       // 4 4
			"pandn	%%mm3, %%mm5;"       // e=2 & e!=g
			                             // f=3 & f!=h
			"punpckldq %%mm0, %%mm2;"    // 4 5
			"pcmpeqd %%mm2, %%mm7;"      // e=4 f=5
			"pandn	%%mm5, %%mm7;"       // e=2 & e!=g & e!=4
			                             // f=3 & f!=h & f!=5
			"pand	%%mm7, %%mm4;"       //  cond & (e f)
			"pandn	%%mm1, %%mm7;"       // !cond & (3 4)
			"por	%%mm4, %%mm7;"       // cond ? (e f) : (3 4)

			"movntq	%%mm6,  (%3);"
			"movntq	%%mm7, 8(%3);"

			"emms;"

			: // no output
			: "r" (src1 + srcWidth - 4) // 0
			, "r" (src0 + srcWidth - 4) // 1
			, "r" (src2 + srcWidth - 4) // 2
			, "r" (dst  + srcWidth - 4) // 3
			, "r" (-4 * (srcWidth - 4)) // 4
			#ifdef __MMX__
			: "mm0", "mm1", "mm2", "mm3"
			, "mm4", "mm5", "mm6", "mm7"
			#endif
		);
		return;
	};
	if ((sizeof(Pixel) == 4) && cpu.hasMMX()) {
		asm (
			"movq	(%0,%4), %%mm0;"     // 1 2
			"movq	%%mm0, %%mm2;"       // 1 2
			"punpckldq %%mm2, %%mm2;"    // 1 1
			".p2align 4,,15;"
		"0:"
			"movq	(%1,%4), %%mm4;"     // a b
			"movq	%%mm0, %%mm3;"       // 1 2
			"punpckhdq %%mm3, %%mm3;"    // 2 2
			"pcmpeqd %%mm4, %%mm2;"      // a=0 b=1
			"movq	(%2,%4), %%mm5;"     // c d
			"movq	%%mm4, %%mm6;"       // a b
			"pcmpeqd %%mm4, %%mm5;"      // a=c b=d
			"movq	8(%0,%4), %%mm1;"    // 3 4
			"pandn	%%mm2, %%mm5;"       // a=0 & a!=c
			                             // b=1 & b!=d
			"punpckldq %%mm1, %%mm3;"    // 2 3
			"pcmpeqd %%mm3, %%mm6;"      // a=2 b=3
			"pandn	%%mm5, %%mm6;"       // a=0 & a!=c & a!=2
			                             // b=1 & b!=d & b!=3
			"pand	%%mm6, %%mm4;"       //  cond & (a b)
			"pandn	%%mm0, %%mm6;"       // !cond & (1 2)
			"por	%%mm4, %%mm6;"       // cond ? (a b) : (1 2)

			"movq	8(%1,%4), %%mm4;"    // e f
			"movq	%%mm1, %%mm2;"       // 3 4
			"punpckldq %%mm2, %%mm2;"    // 4 4
			"pcmpeqd %%mm4, %%mm3;"      // e=2 f=3
			"movq	8(%2,%4), %%mm5;"    // g h
			"movq	%%mm4, %%mm7;"       // e f
			"pcmpeqd %%mm4, %%mm5;"      // e=g f=h
			"movq	16(%0,%4), %%mm0;"   // 5 6
			"pandn	%%mm3, %%mm5;"       // e=2 & e!=g
			                             // f=3 & f!=h
			"punpckldq %%mm0, %%mm2;"    // 4 5
			"pcmpeqd %%mm2, %%mm7;"      // e=4 f=5
			"pandn	%%mm5, %%mm7;"       // e=2 & e!=g & e!=4
			                             // f=3 & f!=h & f!=5
			"pand	%%mm7, %%mm4;"       //  cond & (e f)
			"pandn	%%mm1, %%mm7;"       // !cond & (3 4)
			"por	%%mm4, %%mm7;"       // cond ? (e f) : (3 4)

			"movq	%%mm6,  (%3,%4);"
			"movq	%%mm7, 8(%3,%4);"

			"add	$16, %4;"
			"jnz	0b;"

			"movq	(%1), %%mm4;"        // a b
			"movq	%%mm0, %%mm3;"       // 1 2
			"punpckhdq %%mm3, %%mm3;"    // 2 2
			"pcmpeqd %%mm4, %%mm2;"      // a=0 b=1
			"movq	(%2), %%mm5;"        // c d
			"movq	%%mm4, %%mm6;"       // a b
			"pcmpeqd %%mm4, %%mm5;"      // a=c b=d
			"movq	8(%0), %%mm1;"       // 3 4
			"pandn	%%mm2, %%mm5;"       // a=0 & a!=c
			                             // b=1 & b!=d
			"punpckldq %%mm1, %%mm3;"    // 2 3
			"pcmpeqd %%mm3, %%mm6;"      // a=2 b=3
			"pandn	%%mm5, %%mm6;"       // a=0 & a!=c & a!=2
			                             // b=1 & b!=d & b!=3
			"pand	%%mm6, %%mm4;"       //  cond & (a b)
			"pandn	%%mm0, %%mm6;"       // !cond & (1 2)
			"por	%%mm4, %%mm6;"       // cond ? (a b) : (1 2)

			"movq	8(%1), %%mm4;"       // e f
			"movq	%%mm1, %%mm2;"       // 3 4
			"punpckldq %%mm2, %%mm2;"    // 4 4
			"pcmpeqd %%mm4, %%mm3;"      // e=2 f=3
			"movq	8(%2), %%mm5;"       // g h
			"movq	%%mm4, %%mm7;"       // e f
			"pcmpeqd %%mm4, %%mm5;"      // e=g f=h
			"movq	%%mm2, %%mm0;"       // 4 4
			"pandn	%%mm3, %%mm5;"       // e=2 & e!=g
			                             // f=3 & f!=h
			"punpckldq %%mm0, %%mm2;"    // 4 5
			"pcmpeqd %%mm2, %%mm7;"      // e=4 f=5
			"pandn	%%mm5, %%mm7;"       // e=2 & e!=g & e!=4
			                             // f=3 & f!=h & f!=5
			"pand	%%mm7, %%mm4;"       //  cond & (e f)
			"pandn	%%mm1, %%mm7;"       // !cond & (3 4)
			"por	%%mm4, %%mm7;"       // cond ? (e f) : (3 4)

			"movq	%%mm6,  (%3);"
			"movq	%%mm7, 8(%3);"

			"emms;"

			: // no output
			: "r" (src1 + srcWidth - 4) // 0
			, "r" (src0 + srcWidth - 4) // 1
			, "r" (src2 + srcWidth - 4) // 2
			, "r" (dst  + srcWidth - 4) // 3
			, "r" (-4 * (srcWidth - 4)) // 4
			#ifdef __MMX__
			: "mm0", "mm1", "mm2", "mm3"
			, "mm4", "mm5", "mm6", "mm7"
			#endif
		);
		return;
	};
	//      aceg ikmo
	// ...0 1234 5678 9...
	//      bdfh jlnp
	if ((sizeof(Pixel) == 2) && cpu.hasSSE()) {
		asm (
			"movq	(%0,%4), %%mm1;"     // 1234
			"pshufw	$0, %%mm1, %%mm0;"   // ...0
			".p2align 4,,15;"
		"0:"
			"movq	(%1,%4), %%mm4;"     // aceg
			"psrlq	$48, %%mm0;"         // 0...
			"movq	(%2,%4), %%mm5;"     // bdfh
			"movq	%%mm1, %%mm2;"       // 1234
			"pcmpeqw %%mm4, %%mm5;"      // a=b c=d e=f g=h
			"psllq	$16, %%mm2;"         // .123
			"movq	%%mm1, %%mm6;"       // 1234
			"por	%%mm0, %%mm2;"       // 0123
			"psrlq	$16, %%mm6;"         // 234.
			"pcmpeqw %%mm4, %%mm2;"      // a=0 c=1 e=2 g=3
			"movq	8(%0,%4), %%mm0;"    // 5678
			"movq	%%mm0, %%mm3;"       // 5678
			"psllq	$48, %%mm3;"         // ...5
			"pandn	%%mm2, %%mm5;"       // a=0 & a!=b .. .. ..
			"por	%%mm3, %%mm6;"       // 2345
			"pcmpeqw %%mm4, %%mm6;"      // a=2 c=3 e=4 g=5
			"pandn	%%mm5, %%mm6;"       // a=0 & a!=b & a!=2 .. .. ..
			"pand	%%mm6, %%mm4;"       //  cond & aceg
			"pandn	%%mm1, %%mm6;"       // !cond & 1234
			"por	%%mm4, %%mm6;"       // cond ? aceg : 1234

			"movq	8(%1,%4), %%mm4;"    // ikmo
			"psrlq	$48, %%mm1;"         // 4...
			"movq	8(%2,%4), %%mm5;"    // jlnp
			"movq	%%mm0, %%mm2;"       // 5678
			"pcmpeqw %%mm4, %%mm5;"      // i=j k=l m=n o=p
			"psllq	$16, %%mm2;"         // .567
			"movq	%%mm0, %%mm7;"       // 5678
			"por	%%mm1, %%mm2;"       // 4567
			"psrlq	$16, %%mm7;"         // 678.
			"pcmpeqw %%mm4, %%mm2;"      // i=4 k=5 m=6 o=7
			"movq	16(%0,%4), %%mm1;"   // 9 10 11 12
			"movq	%%mm1, %%mm3;"       // 9 10 11 12
			"psllq	$48, %%mm3;"         // ...9
			"pandn	%%mm2, %%mm5;"       // i=4 & i!=j .. .. ..
			"por	%%mm3, %%mm7;"       // 6789
			"pcmpeqw %%mm4, %%mm7;"      // i=6 k=7 m=8 o=9
			"pandn	%%mm5, %%mm7;"       // i=4 & i!=j & i!=6 .. .. ..
			"pand	%%mm7, %%mm4;"       //  cond & ikmo
			"pandn	%%mm0, %%mm7;"       // !cond & 5678
			"por	%%mm4, %%mm7;"       // cond ? ikmo : 5678

			"movntq	%%mm6,  (%3,%4);"
			"movntq	%%mm7, 8(%3,%4);"

			"add	$16, %4;"
			"jnz	0b;"

			"movq	(%1), %%mm4;"        // aceg
			"psrlq	$48, %%mm0;"         // 0...
			"movq	(%2), %%mm5;"        // bdfh
			"movq	%%mm1, %%mm2;"       // 1234
			"pcmpeqw %%mm4, %%mm5;"      // a=b c=d e=f g=h
			"psllq	$16, %%mm2;"         // .123
			"movq	%%mm1, %%mm6;"       // 1234
			"por	%%mm0, %%mm2;"       // 0123
			"psrlq	$16, %%mm6;"         // 234.
			"pcmpeqw %%mm4, %%mm2;"      // a=0 c=1 e=2 g=3
			"movq	8(%0), %%mm0;"       // 5678
			"movq	%%mm0, %%mm3;"       // 5678
			"psllq	$48, %%mm3;"         // ...5
			"pandn	%%mm2, %%mm5;"       // a=0 & a!=b .. .. ..
			"por	%%mm3, %%mm6;"       // 2345
			"pcmpeqw %%mm4, %%mm6;"      // a=2 c=3 e=4 g=5
			"pandn	%%mm5, %%mm6;"       // a=0 & a!=b & a!=2 .. .. ..
			"pand	%%mm6, %%mm4;"       //  cond & aceg
			"pandn	%%mm1, %%mm6;"       // !cond & 1234
			"por	%%mm4, %%mm6;"       // cond ? aceg : 1234

			"movq	8(%1), %%mm4;"       // ikmo
			"psrlq	$48, %%mm1;"         // 4...
			"movq	8(%2), %%mm5;"       // jlnp
			"movq	%%mm0, %%mm2;"       // 5678
			"pcmpeqw %%mm4, %%mm5;"      // i=j k=l m=n o=p
			"psllq	$16, %%mm2;"         // .567
			"movq	%%mm0, %%mm7;"       // 5678
			"por	%%mm1, %%mm2;"       // 4567
			"psrlq	$16, %%mm7;"         // 678.
			"pcmpeqw %%mm4, %%mm2;"      // i=4 k=5 m=6 o=7
			"pshufw	$255, %%mm0, %%mm1;" // 9...
			"movq	%%mm1, %%mm3;"       // 9...
			"psllq	$48, %%mm3;"         // ...9
			"pandn	%%mm2, %%mm5;"       // i=4 & i!=j .. .. ..
			"por	%%mm3, %%mm7;"       // 6789
			"pcmpeqw %%mm4, %%mm7;"      // i=6 k=7 m=8 o=9
			"pandn	%%mm5, %%mm7;"       // i=4 & i!=j & i!=6 .. .. ..
			"pand	%%mm7, %%mm4;"       //  cond & ikmo
			"pandn	%%mm0, %%mm7;"       // !cond & 5678
			"por	%%mm4, %%mm7;"       // cond ? ikmo : 5678

			"movntq	%%mm6,  (%3);"
			"movntq	%%mm7, 8(%3);"

			"emms;"

			: // no output
			: "r" (src1 + srcWidth - 8) // 0
			, "r" (src0 + srcWidth - 8) // 1
			, "r" (src2 + srcWidth - 8) // 2
			, "r" (dst  + srcWidth - 8) // 3
			, "r" (-2 * (srcWidth - 8)) // 4
			#ifdef __MMX__
			: "mm0", "mm1", "mm2", "mm3"
			, "mm4", "mm5", "mm6", "mm7"
			#endif
		);
		return;
	};
	if ((sizeof(Pixel) == 2) && cpu.hasMMX()) {
		asm (
			"movq	(%0,%4), %%mm1;"     // 1234
			"movq	%%mm1, %%mm0;"       // 1234
			"psllq	$48, %%mm0;"         // ...0
			".p2align 4,,15;"
		"0:"
			"movq	(%1,%4), %%mm4;"     // aceg
			"psrlq	$48, %%mm0;"         // 0...
			"movq	(%2,%4), %%mm5;"     // bdfh
			"movq	%%mm1, %%mm2;"       // 1234
			"pcmpeqw %%mm4, %%mm5;"      // a=b c=d e=f g=h
			"psllq	$16, %%mm2;"         // .123
			"movq	%%mm1, %%mm6;"       // 1234
			"por	%%mm0, %%mm2;"       // 0123
			"psrlq	$16, %%mm6;"         // 234.
			"pcmpeqw %%mm4, %%mm2;"      // a=0 c=1 e=2 g=3
			"movq	8(%0,%4), %%mm0;"    // 5678
			"movq	%%mm0, %%mm3;"       // 5678
			"psllq	$48, %%mm3;"         // ...5
			"pandn	%%mm2, %%mm5;"       // a=0 & a!=b .. .. ..
			"por	%%mm3, %%mm6;"       // 2345
			"pcmpeqw %%mm4, %%mm6;"      // a=2 c=3 e=4 g=5
			"pandn	%%mm5, %%mm6;"       // a=0 & a!=b & a!=2 .. .. ..
			"pand	%%mm6, %%mm4;"       //  cond & aceg
			"pandn	%%mm1, %%mm6;"       // !cond & 1234
			"por	%%mm4, %%mm6;"       // cond ? aceg : 1234

			"movq	8(%1,%4), %%mm4;"    // ikmo
			"psrlq	$48, %%mm1;"         // 4...
			"movq	8(%2,%4), %%mm5;"    // jlnp
			"movq	%%mm0, %%mm2;"       // 5678
			"pcmpeqw %%mm4, %%mm5;"      // i=j k=l m=n o=p
			"psllq	$16, %%mm2;"         // .567
			"movq	%%mm0, %%mm7;"       // 5678
			"por	%%mm1, %%mm2;"       // 4567
			"psrlq	$16, %%mm7;"         // 678.
			"pcmpeqw %%mm4, %%mm2;"      // i=4 k=5 m=6 o=7
			"movq	16(%0,%4), %%mm1;"   // 9 10 11 12
			"movq	%%mm1, %%mm3;"       // 9 10 11 12
			"psllq	$48, %%mm3;"         // ...9
			"pandn	%%mm2, %%mm5;"       // i=4 & i!=j .. .. ..
			"por	%%mm3, %%mm7;"       // 6789
			"pcmpeqw %%mm4, %%mm7;"      // i=6 k=7 m=8 o=9
			"pandn	%%mm5, %%mm7;"       // i=4 & i!=j & i!=6 .. .. ..
			"pand	%%mm7, %%mm4;"       //  cond & ikmo
			"pandn	%%mm0, %%mm7;"       // !cond & 5678
			"por	%%mm4, %%mm7;"       // cond ? ikmo : 5678

			"movq	%%mm6,  (%3,%4);"
			"movq	%%mm7, 8(%3,%4);"

			"add	$16, %4;"
			"jnz	0b;"

			"movq	(%1), %%mm4;"        // aceg
			"psrlq	$48, %%mm0;"         // 0...
			"movq	(%2), %%mm5;"        // bdfh
			"movq	%%mm1, %%mm2;"       // 1234
			"pcmpeqw %%mm4, %%mm5;"      // a=b c=d e=f g=h
			"psllq	$16, %%mm2;"         // .123
			"movq	%%mm1, %%mm6;"       // 1234
			"por	%%mm0, %%mm2;"       // 0123
			"psrlq	$16, %%mm6;"         // 234.
			"pcmpeqw %%mm4, %%mm2;"      // a=0 c=1 e=2 g=3
			"movq	8(%0), %%mm0;"       // 5678
			"movq	%%mm0, %%mm3;"       // 5678
			"psllq	$48, %%mm3;"         // ...5
			"pandn	%%mm2, %%mm5;"       // a=0 & a!=b .. .. ..
			"por	%%mm3, %%mm6;"       // 2345
			"pcmpeqw %%mm4, %%mm6;"      // a=2 c=3 e=4 g=5
			"pandn	%%mm5, %%mm6;"       // a=0 & a!=b & a!=2 .. .. ..
			"pand	%%mm6, %%mm4;"       //  cond & aceg
			"pandn	%%mm1, %%mm6;"       // !cond & 1234
			"por	%%mm4, %%mm6;"       // cond ? aceg : 1234

			"movq	8(%1), %%mm4;"       // ikmo
			"psrlq	$48, %%mm1;"         // 4...
			"movq	8(%2), %%mm5;"       // jlnp
			"movq	%%mm0, %%mm2;"       // 5678
			"pcmpeqw %%mm4, %%mm5;"      // i=j k=l m=n o=p
			"psllq	$16, %%mm2;"         // .567
			"movq	%%mm0, %%mm7;"       // 5678
			"por	%%mm1, %%mm2;"       // 4567
			"psrlq	$16, %%mm7;"         // 678.
			"pcmpeqw %%mm4, %%mm2;"      // i=4 k=5 m=6 o=7
			"movq	%%mm0, %%mm3;"       // 5678
			"psrlq	$48, %%mm3;"         // 9...
			"psllq	$48, %%mm3;"         // ...9
			"pandn	%%mm2, %%mm5;"       // i=4 & i!=j .. .. ..
			"por	%%mm3, %%mm7;"       // 6789
			"pcmpeqw %%mm4, %%mm7;"      // i=6 k=7 m=8 o=9
			"pandn	%%mm5, %%mm7;"       // i=4 & i!=j & i!=6 .. .. ..
			"pand	%%mm7, %%mm4;"       //  cond & ikmo
			"pandn	%%mm0, %%mm7;"       // !cond & 5678
			"por	%%mm4, %%mm7;"       // cond ? ikmo : 5678

			"movq	%%mm6,  (%3);"
			"movq	%%mm7, 8(%3);"

			"emms;"

			: // no output
			: "r" (src1 + srcWidth - 8) // 0
			, "r" (src0 + srcWidth - 8) // 1
			, "r" (src2 + srcWidth - 8) // 2
			, "r" (dst  + srcWidth - 8) // 3
			, "r" (-2 * (srcWidth - 8)) // 4
			#ifdef __MMX__
			: "mm0", "mm1", "mm2", "mm3"
			, "mm4", "mm5", "mm6", "mm7"
			#endif
		);
		return;
	};
	#endif

	// First pixel.
	Pixel mid =   src1[0];
	Pixel right = src1[1];
	dst[0] = mid;

	// Central pixels.
	for (unsigned x = 1; x < srcWidth - 1; ++x) {
		Pixel left = mid;
		mid   = right;
		right = src1[x + 1];
		Pixel top = src0[x];
		Pixel bot = src2[x];
		dst[x] = (left == top && right != top && bot != top) ? top : mid;
	}

	// Last pixel.
	dst[srcWidth - 1] =
		(mid == src0[srcWidth - 1] && src2[srcWidth - 1] != src0[srcWidth - 1])
		? src0[srcWidth - 1] : right;
}

template <class Pixel>
void Scale2xScaler<Pixel>::scale1x1to2x2(FrameSource& src,
	unsigned srcStartY, unsigned /*srcEndY*/, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	dst.lock();
	int srcY = srcStartY;
	const Pixel* srcPrev = src.getLinePtr<Pixel>(srcY - 1, srcWidth);
	const Pixel* srcCurr = src.getLinePtr<Pixel>(srcY + 0, srcWidth);
	for (unsigned dstY = dstStartY; dstY < dstEndY; srcY += 1, dstY += 2) {
		const Pixel* srcNext = src.getLinePtr<Pixel>(srcY + 1, srcWidth);
		Pixel* dstUpper = dst.getLinePtrDirect<Pixel>(dstY + 0);
		scaleLineHalf_1on2(dstUpper, srcPrev, srcCurr, srcNext, srcWidth);
		Pixel* dstLower = dst.getLinePtrDirect<Pixel>(dstY + 1);
		scaleLineHalf_1on2(dstLower, srcNext, srcCurr, srcPrev, srcWidth);
		srcPrev = srcCurr;
		srcCurr = srcNext;
	}
}

template <class Pixel>
void Scale2xScaler<Pixel>::scale1x1to1x2(FrameSource& src,
	unsigned srcStartY, unsigned /*srcEndY*/, unsigned srcWidth,
	OutputSurface& dst, unsigned dstStartY, unsigned dstEndY)
{
	dst.lock();
	int srcY = srcStartY;
	const Pixel* srcPrev = src.getLinePtr<Pixel>(srcY - 1, srcWidth);
	const Pixel* srcCurr = src.getLinePtr<Pixel>(srcY + 0, srcWidth);
	for (unsigned dstY = dstStartY; dstY < dstEndY; srcY += 1, dstY += 2) {
		const Pixel* srcNext = src.getLinePtr<Pixel>(srcY + 1, srcWidth);
		Pixel* dstUpper = dst.getLinePtrDirect<Pixel>(dstY + 0);
		scaleLineHalf_1on1(dstUpper, srcPrev, srcCurr, srcNext, srcWidth);
		Pixel* dstLower = dst.getLinePtrDirect<Pixel>(dstY + 1);
		scaleLineHalf_1on1(dstLower, srcNext, srcCurr, srcPrev, srcWidth);
		srcPrev = srcCurr;
		srcCurr = srcNext;
	}
}


// Force template instantiation.
#if HAVE_16BPP
template class Scale2xScaler<word>;
#endif
#if HAVE_32BPP
template class Scale2xScaler<unsigned>;
#endif

} // namespace openmsx
