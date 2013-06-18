#include "Font.h"
#include <common/hgeall.h>

bool Font::start()
{
	m_font = new hgeFont("../LucidaConsole10px.fnt");
	return true;
}

void Font::stop()
{
	delete m_font;
}
