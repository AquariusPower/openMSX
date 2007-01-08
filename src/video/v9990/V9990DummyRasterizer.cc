// $Id$

#include "V9990DummyRasterizer.hh"

namespace openmsx {

bool V9990DummyRasterizer::isActive()
{
	return false;
}

void V9990DummyRasterizer::reset()
{
}

void V9990DummyRasterizer::frameStart()
{
}

void V9990DummyRasterizer::frameEnd(const EmuTime& /*time*/)
{
}

void V9990DummyRasterizer::setDisplayMode(V9990DisplayMode /*displayMode*/)
{
}

void V9990DummyRasterizer::setColorMode(V9990ColorMode /*colorMode*/)
{
}

void V9990DummyRasterizer::setPalette(
	int /*index*/, byte /*r*/, byte /*g*/, byte /*b*/)
{
}

void V9990DummyRasterizer::drawBorder(int /*fromX*/, int /*fromY*/,
                                      int /*limitX*/, int /*limitY*/)
{
}

void V9990DummyRasterizer::drawDisplay(int /*fromX*/, int /*fromY*/,
                                       int /*displayX*/, int /*displayY*/,
                                       int /*displayWidth*/, int /*displayHeight*/)
{
}

bool V9990DummyRasterizer::isRecording() const
{
	return false;
}

} // namespace openmsx
