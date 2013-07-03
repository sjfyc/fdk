#ifndef __FONT_H
#define __FONT_H
#include <fdk/Module.h>
class hgeFont;

class Font
	: public fdk::Module<Font>
{
	friend class fdk::Module<Font>;
public:
	static const char* NAME() { return "Font"; }
	bool start();
	void stop();
	hgeFont& getFont() { return *m_font; }
private:
	hgeFont* m_font;
};

#define g_Font (Font::instance().getFont())
#endif