#include "EasyVulkanFontManager.h"

EasyVulkanFontManager::EasyVulkanFontManager():
	m_library(NULL) {}

EasyVulkanFontManager::~EasyVulkanFontManager()
{
	FT_Error err;

	for (size_t i = 0; i < m_fonts.size(); i++)
	{
		delete m_fonts[i];
		m_fonts[i] = NULL;
	}

	m_fonts.clear();

	if ((err = FT_Done_FreeType(m_library)) != FT_Err_Ok)
	{
		VkThrowErrorf("EasyVulkanFontManager:FT_Done_FreeType() Error %d\n", err);
	}
}

bool EasyVulkanFontManager::init()
{
	FT_Error err;

	if ((err = FT_Init_FreeType(&m_library)) != FT_Err_Ok)
	{
		VkThrowErrorf("EasyVulkanFontManager:FT_Init_FreeType() Error %d\n", err);
		return false;
	}

	return true;
}

void EasyVulkanFontManager::destory()
{
	FT_Error err;

	for (size_t i = 0; i < m_fonts.size(); i++)
	{
		delete m_fonts[i];
		m_fonts[i] = NULL;
	}

	m_fonts.clear();

	if ((err = FT_Done_FreeType(m_library)) != FT_Err_Ok)
	{
		VkThrowErrorf("EasyVulkanFontManager:FT_Done_FreeType() Error %d\n", err);
		throw std::runtime_error(message);
	}
}

VkFont::VkFont() :m_face(NULL),
m_rgba(NULL),
m_antialias(false),
m_bold(false),
m_tall(0)
{

}

VkFont::~VkFont()
{

}

bool VkFont::create(FT_Library library, const char* filePath, FT_Long face_index, int tall, bool bold, bool italic, bool antialias)
{
	FT_Error err;

	if (tall >= 65)
	{
		VkThrowErrorf("VkFont Create Failed:Bigger than a page size");
		return false;
	}

	if ((err = FT_New_Face(library, filePath, face_index, &face)) != FT_Err_Ok)
	{
		VkThrowErrorf("VkFont Create Failed:FT_New_Face() Error %d\n", err);
		return false;
	}

	if ((err = FT_Set_Pixel_Sizes(face, 0, tall)) != FT_Err_Ok)
	{
		VkThrowErrorf("VkFont Create Failed:FT_Set_Pixel_Sizes() Error %d\n", err);
		return false;
	}

	rgbaSize = (tall * 2) * tall * 4;

	rgba = new unsigned char[rgbaSize];

	if (rgba == NULL)
	{
		return false;
	}

	this->fontPath = filePath;
	this->library = library;
	this->antialias = antialias;
	this->bold = bold;
	this->tall = tall;
	this->italic = italic;
	this->fontName = face->family_name;

	return true;
}

void VkFont::release()
{
	FT_Error err;

	if (m_face)
	{
		if ((err = FT_Done_Face(m_face)) != FT_Err_Ok)
		{
			VkThrowErrorf("FT_Done_Face() Error %d\n", err);
		}
		m_face = NULL;
	}

	if (m_rgba)
	{
		delete[] m_rgba;
		m_rgba = NULL;
	}

	for (CharMap::iterator it = m_chars.begin(); it != m_chars.end(); it++)
	{
		delete it->second;
		it->second = NULL;
	}

	m_chars.clear();

	for (size_t i = 0; i < m_pages.size(); i++)
	{
		delete m_pages[i];
		m_pages[i] = NULL;
	}

	m_pages.clear();
}

bool VkFont::getCharInfo(int code, SingleText*& texture, glyphMetrics* metrics)
{
	unsigned char* bytes;
	glyphMetrics gm;
	CharMap::iterator it = chars.find(code);

	if (it != chars.end())
	{
		it->second->getInfo(metrics, bytes, coords);
		texture = texts[code];
		return true;
	}

	if (loadChar(code, &gm) == false)
	{
		return false;
	}

	VkFontChar* ch = new VkFontChar();

	ch->code = code;
	ch->setInfo(&gm);


	bytes = ch->bytes = page->getBytes();
	ch->getInfo(metrics, bytes, coords);
	chars.insert(CharMap::value_type(code, ch));

	SingleText* newTexture = new SingleText;
	newTexture->create(rgba, metrics->width, metrics->height, metrics->horiBearingX, metrics->horiBearingY, metrics->horiAdvance);
	texture = this->texts[code] = newTexture;
	return true;
}

bool VkFont::loadChar(int code, glyphMetrics* metrics)
{
	FT_Error err;

	FT_UInt glyph_index = FT_Get_Char_Index(face, (FT_ULong)code);

	if (glyph_index > 0)
	{
		if ((err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT)) == FT_Err_Ok)
		{
			FT_GlyphSlot glyph = face->glyph;

			FT_Render_Mode render_mode = antialias ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO;

			if (antialias && bold)
			{

				if ((err = FT_Outline_Embolden(&glyph->outline, FT_Pos(60)) != FT_Err_Ok))
				{
					//printf("FT_Outline_EmboldenXY() Error %d\n", err);
				}
			}

			if ((err = FT_Render_Glyph(glyph, render_mode)) == FT_Err_Ok)
			{
				FT_Bitmap* bitmap = &glyph->bitmap;

				switch (bitmap->pixel_mode)
				{
				case FT_PIXEL_MODE_MONO:
				{
					if (!antialias && bold)
					{
						if ((err = FT_Bitmap_Embolden(library, bitmap, 60, 0)) != FT_Err_Ok)
						{
							//printf("FT_Bitmap_Embolden() Error %d\n", err);
						}
					}
					ConvertMONOToRGBA(bitmap, rgba);
					break;
				}
				case FT_PIXEL_MODE_GRAY:
				{
					ConvertGRAYToRGBA(bitmap, rgba);
					break;
				}
				default:
				{
					memset(rgba, 0xFF, rgbaSize);
					break;
				}
				}

				metrics->width = bitmap->width;
				metrics->height = bitmap->rows;
				metrics->horiBearingX = glyph->bitmap_left;
				metrics->horiBearingY = glyph->bitmap_top;
				metrics->horiAdvance = glyph->advance.x >> 6;

				return true;
			}
			else
			{
				//printf("FT_Render_Glyph() Error %d\n", err);
				return false;
			}
		}
		else
		{
			//printf("FT_Load_Glyph() Error %d\n", err);
			return false;
		}
	}
}

