#include <QtCore/QCoreApplication>
#include <ft2build.h>

#include <stdio.h>

#include <QImage>
#include <QMap>
#include <QTime>

#include <iostream>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_OUTLINE_H

#define CONVERT_FONT_INDEX(x) (((x) < 1 || (x) > (int)m_fonts.size()) ? -1 : (x) - 1)

#define VkThrowErrorf(msg,...) char* message = new char[127]{ NULL };sprintf(message,msg,__VA_ARGS__); throw std::runtime_error(message);

#pragma comment(lib,"freetype.lib")

void ConvertMONOToRGBA(FT_Bitmap* source, unsigned char* rgba)
{
	unsigned char* s = source->buffer;
	unsigned char* t = rgba;

	for (size_t y = source->rows; y > 0; y--)
	{
		unsigned char* ss = s;
		unsigned char* tt = t;

		for (size_t x = source->width >> 3; x > 0; x--)
		{
			size_t val = *ss;

			for (size_t i = 8; i > 0; i--)
			{
				tt[0] = tt[1] = tt[2] = tt[3] = (val & (1 << (i - 1))) ? 0xFF : 0x00;
				tt += 4;
			}

			ss += 1;
		}

		size_t rem = source->width & 7;

		if (rem > 0)
		{
			size_t val = *ss;

			for (size_t x = rem; x > 0; x--)
			{
				tt[0] = tt[1] = tt[2] = tt[3] = (val & 0x80) ? 0xFF : 0x00;
				tt += 4;
				val <<= 1;
			}
		}

		s += source->pitch;
		t += source->width * 4;    //pitch
	}
}
void ConvertGRAYToRGBA(FT_Bitmap* source, unsigned char* rgba)
{
	for (uint32_t y = 0; y < source->rows; y++)
	{
		for (uint32_t x = 0; x < source->width; x++)
		{
			unsigned char* s = &(source->buffer[(y * source->pitch) + x]);
			unsigned char* t = &(rgba[((y * source->pitch) + x) * 4]);

			t[0] = t[1] = t[2] = 0x00;
			t[3] = *s;
		}
	}
}

using namespace std;

struct glyphMetrics
{
	int    width;
	int    height;
	int    horiBearingX;
	int    horiBearingY;
	int    horiAdvance;
};

class EasyVulkanFontManager;

class VkFont
{
public:
	void  Release();
	bool  GetCharInfo(int code, glyphMetrics* metrics, unsigned char*& bytes, float coords[]);
	int   GetFontTall();
	char* GetFontName();
private:
	bool Create(FT_Library library, const char* filename, FT_Long face_index, int tall, bool bold, bool italic, bool antialias);
	VkFont();
	~VkFont();
	bool LoadChar(int code, glyphMetrics* metrics);;

	class VkFontChar
	{
	public:
		void setInfo(glyphMetrics* metrics);
		void getInfo(glyphMetrics* metrics, unsigned char*& rgba, float coords[]);

	public:
		int                m_code;
		unsigned char*     m_data;
		float              m_coords[4];    // left top right bottom

	private:
		glyphMetrics       m_metrics;
	};

	class VkFontPage
	{
	public:
		VkFontPage();
		~VkFontPage();

		bool append(int wide, int tall, unsigned char* rgba, float coords[]);
		unsigned char* GetBytes();

	private:
		unsigned char* m_data;
		int            m_wide;
		int            m_tall;
		int            m_posx;
		int            m_posy;
		int            m_maxCharTall;
	};

	typedef std::map<int, VkFontChar*> CharMap;

	FT_Library                 m_library;
	FT_Face                    m_face;
	bool                       m_antialias;
	bool                       m_bold;
	int                        m_tall;
	int                        m_rgbaSize;
	unsigned char*             m_rgba;
	CharMap                    m_chars;
	std::vector<VkFontPage*>   m_pages;

	friend class EasyVulkanFontManager;
};
class EasyVulkanFontManager
{
public:

	EasyVulkanFontManager();
	~EasyVulkanFontManager();
	bool Init();
	void Destory();
	VkFont* LoadFont(const char* filename, int face, int tall, bool bold, bool italic, bool antialias)
	{
		VkFont* font = new VkFont();

		if (font->Create(m_library, filename, face, tall, bold, italic, antialias) != true)
		{
			delete font;
			return NULL;
		}

		return font;
	}
	bool GetFontInfo(VkFont* font, int code, int& wide, int& tall, int& horiBearingX, int& horiBearingY, int& horiAdvance, unsigned char* &bytes, float* coords)
	{
		glyphMetrics metrics;

		if (font->GetCharInfo(code, &metrics, bytes, coords) == false)
		{
			return false;
		}
		wide = metrics.width;
		tall = metrics.height;
		horiBearingX = metrics.horiBearingX;
		horiBearingY = metrics.horiBearingY;
		horiAdvance = metrics.horiAdvance;

		return true;
	}
	int  GetFontTall(int font_index);

public:
	FT_Library                m_library;
	std::vector<VkFont*>      m_fonts;
};

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	/*EasyVulkanFontManager* fontManager = new EasyVulkanFontManager;
	fontManager->Init();
	auto font = fontManager->LoadFont("test.ttf", 0, 64,false,false,true);

	int cw_, ct_, bx_, by_, av_;
	float crd[4];

	unsigned char* bytes = NULL;

	bool result;

	result = fontManager->GetFontInfo(font, 'a', cw_, ct_, bx_, by_, av_, bytes, crd);
	QImage image(bytes, cw_, ct_, QImage::Format::Format_RGBA8888);
	result = image.save("123.png");

	result = fontManager->GetFontInfo(font, L'²Ù', cw_, ct_, bx_, by_, av_, bytes, crd);
	image = QImage(bytes, cw_, ct_, QImage::Format::Format_RGBA8888);
	result = image.save("143.png");(*/
	QTime time;



	QMap< QString*, int> m_Map;
	QVector<QString*> m_Vector;

	for (int i = 0; i < 1000000; i++)
	{
		QString* str = new QString;
		m_Vector.append(str);
		m_Map.insert(str, i*i);
	}
	time.start();
	for (int i = 0; i < 1000000; i++)
	{
		int aaa = m_Map[m_Vector[i]];
	}

	std::cout<< time.elapsed();
	return a.exec();
}

EasyVulkanFontManager::EasyVulkanFontManager() :
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

bool EasyVulkanFontManager::Init()
{
	FT_Error err;

	if ((err = FT_Init_FreeType(&m_library)) != FT_Err_Ok)
	{
		VkThrowErrorf("EasyVulkanFontManager:FT_Init_FreeType() Error %d\n", err);
		return false;
	}

	return true;
}

void EasyVulkanFontManager::Destory()
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


int EasyVulkanFontManager::GetFontTall(int font_index)
{
	int i = CONVERT_FONT_INDEX(font_index);
	if (i == -1)
	{
		return false;
	}

	auto font = m_fonts[i];

	return font->GetFontTall();
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

bool VkFont::Create(FT_Library library, const char* filename, FT_Long face_index, int tall, bool bold, bool italic, bool antialias)
{
	FT_Error err;

	if (tall > 256)
	{
		VkThrowErrorf("VkFont Create Failed:Bigger than a page size");
		return false;
	}

	if ((err = FT_New_Face(library, filename, face_index, &m_face)) != FT_Err_Ok)
	{
		VkThrowErrorf("VkFont Create Failed:FT_New_Face() Error %d\n", err);
		return false;
	}

	if ((err = FT_Set_Pixel_Sizes(m_face, 0, tall)) != FT_Err_Ok)
	{
		VkThrowErrorf("VkFont Create Failed:FT_Set_Pixel_Sizes() Error %d\n", err);
		return false;
	}

	m_rgbaSize = (tall * 2) * tall * 4;

	m_rgba = new unsigned char[m_rgbaSize];

	if (m_rgba == NULL)
	{
		return false;
	}

	m_library = library;
	m_antialias = antialias;
	m_bold = bold;
	m_tall = tall;

	if (italic)
	{
		FT_Matrix m;
		m.xx = 0x10000L;
		m.xy = 0.5f * 0x10000L;
		m.yx = 0;
		m.yy = 0x10000L;
		FT_Set_Transform(m_face, &m, NULL);
	}

	return true;
}

void VkFont::Release()
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

bool VkFont::GetCharInfo(int code, glyphMetrics* metrics, unsigned char *&bytes, float coords[])
{
	glyphMetrics gm;
	CharMap::iterator it = m_chars.find(code);

	if (it != m_chars.end())
	{
		it->second->getInfo(metrics, bytes, coords);
		return true;
	}

	if (LoadChar(code, &gm) == false)
	{
		return false;
	}

	VkFontChar* ch = new VkFontChar();

	ch->m_code = code;
	ch->setInfo(&gm);

	for (size_t i = 0; i < m_pages.size(); i++)
	{
		VkFontPage* page = m_pages[i];

		if (page->append(gm.width, gm.height, m_rgba, ch->m_coords))
		{
			bytes = ch->m_data = page->GetBytes();
			ch->getInfo(metrics, bytes, coords);
			m_chars.insert(CharMap::value_type(code, ch));
			return true;
		}
	}

	VkFontPage* page = new VkFontPage();

	if (page->append(gm.width, gm.height, m_rgba, ch->m_coords))
	{
		bytes = ch->m_data = page->GetBytes();
		ch->getInfo(metrics, bytes, coords);
		m_chars.insert(CharMap::value_type(code, ch));
		m_pages.push_back(page);
		return true;
	}

	delete ch;
	delete page;

	return false;
}

int VkFont::GetFontTall()
{
	return m_tall;
}

char* VkFont::GetFontName()
{
	return m_face->family_name;
}

bool VkFont::LoadChar(int code, glyphMetrics* metrics)
{
	FT_Error err;

	FT_UInt glyph_index = FT_Get_Char_Index(m_face, (FT_ULong)code);

	if (glyph_index > 0)
	{
		if ((err = FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT)) == FT_Err_Ok)
		{
			FT_GlyphSlot glyph = m_face->glyph;

			FT_Render_Mode render_mode = m_antialias ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO;

			if (m_antialias && m_bold)
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
					if (!m_antialias && m_bold)
					{
						if ((err = FT_Bitmap_Embolden(m_library, bitmap, 60, 0)) != FT_Err_Ok)
						{
							//printf("FT_Bitmap_Embolden() Error %d\n", err);
						}
					}
					ConvertMONOToRGBA(bitmap, m_rgba);
					break;
				}
				case FT_PIXEL_MODE_GRAY:
				{
					ConvertGRAYToRGBA(bitmap, m_rgba);
					break;
				}
				default:
				{
					memset(m_rgba, 0xFF, m_rgbaSize);
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
void VkFont::VkFontChar::setInfo(glyphMetrics* metrics)
{
	memcpy(&m_metrics, metrics, sizeof(glyphMetrics));
}

void VkFont::VkFontChar::getInfo(glyphMetrics* metrics, unsigned char* &bytes, float coords[])
{
	memcpy(metrics, &m_metrics, sizeof(glyphMetrics));

	bytes = m_data;
	memcpy(coords, m_coords, sizeof(float) * 4);
}

VkFont::VkFontPage::VkFontPage()
{
	m_wide = m_tall = 256;
	m_posx = m_posy = 0;

	// In a line, for a max height character
	m_maxCharTall = 0;

	/*glGenTextures(1, &m_texture);    // Using your API here
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_wide, m_tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
}

VkFont::VkFontPage::~VkFontPage()
{
	// free the texture
}

bool VkFont::VkFontPage::append(int wide, int tall, unsigned char* rgba, float coords[])
{
	if (m_posy + tall > m_tall)
	{
		// not enough line space in this page
		return false;
	}

	// If this line is full ...
	if (m_posx + wide > m_wide)
	{
		int newLineY = m_posy + m_maxCharTall;

		if (newLineY + tall > m_tall)
		{
			// No more space for new line in this page, should allocate a new one
			return false;
		}

		// Begin in new line
		m_posx = 0;
		m_posy = newLineY;
		// Reset
		m_maxCharTall = 0;
	}

	/*glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, m_posx, m_posy, wide, tall, GL_RGBA, GL_UNSIGNED_BYTE, rgba);*/

	m_data = rgba;

	coords[0] = m_posx / (float)m_wide;                // left
	coords[1] = m_posy / (float)m_tall;                // top
	coords[2] = (m_posx + wide) / (float)m_wide;    // right
	coords[3] = (m_posy + tall) / (float)m_tall;    // bottom

	m_posx += wide;

	if (tall > m_maxCharTall)
	{
		m_maxCharTall = tall;
	}

	return true;
}

unsigned char* VkFont::VkFontPage::GetBytes()
{
	return m_data;
}
