#include "EasyVulkanFontManager.h"

QMap<char, Character*> EasyVulkanFontManager::m_CharMap;

bool EasyVulkanFontManager::Init(QString fontFile, int size, bool bold, bool italic)
{
	if (!QFile::exists(fontFile))
	{
		return false;
	}
	TTF_Init();
	TTF_Font* font = TTF_OpenFont(fontFile.toStdString().c_str(), 40);

	int style = TTF_STYLE_NORMAL;
	style &= (bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL);
	style &= (italic ? TTF_STYLE_ITALIC : TTF_STYLE_NORMAL);

	SDL_Color color = { 0, 0, 0 };
	SDL_Surface* face = NULL;

	//14 is a magic number...
	for (int c = 32; c < 128; c++)
	{
		char tmpChar =(char)c;
		string tmp(1, tmpChar);

		face = TTF_RenderText_Blended(font, tmp.c_str(), color);

		QImage::Format format;
		if (face->format->BytesPerPixel == 3) { // RGB 24bit
			format = QImage::Format::Format_RGB888;
		}
		else if (face->format->BytesPerPixel == 4) { // RGBA 32bit
			format = QImage::Format::Format_RGBA8888;
		}
		else {
			SDL_FreeSurface(face);
			return false;
		}


		int minx=0, maxx = 0, miny = 0, maxy = 0, advance = 0;
		//int a = TTF_GlyphMetrics(font, *(tmp.c_str()), &minx, &maxx, &miny, &maxy, &advance);
		VkAssert(TTF_GlyphMetrics(font, *(tmp.c_str()), &minx, &maxx, &miny, &maxy, &advance)  == 0, "TTF_GlyphMetrics");

		Texture* texture = EasyVulkanTextureManager::CreateTexture((uchar*)face->pixels, face->w, face->h, format);
		Character *character = new Character{
			texture,
			QVector2D(face->w, face->h),
			advance,
			maxx,
			minx,
			maxy,
			miny
		};
		m_CharMap[tmpChar] = character; 
	}
	return true;
}

Character* EasyVulkanFontManager::GetCharInfo(char ch)
{
	if (!m_CharMap.contains(ch))
	{
		return NULL;
	}
	return m_CharMap[ch];
}
