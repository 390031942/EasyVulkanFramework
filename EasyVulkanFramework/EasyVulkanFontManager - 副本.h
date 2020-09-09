#pragma once
#include <ft2build.h>
#include <stdio.h>
#include <QImage>

#include <QXmlStreamWriter>
#include <QFile>
#include <QDir>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_OUTLINE_H

#define CONVERT_FONT_INDEX(x) (((x) < 1 || (x) > (int)m_fonts.size()) ? -1 : (x) - 1)

#define VkThrowErrorf(msg,...) char* message = new char[127]{ NULL };sprintf(message,msg,__VA_ARGS__); throw std::runtime_error(message);

#pragma comment(lib,"freetype.lib")

static void ConvertMONOToRGBA(FT_Bitmap* source, unsigned char* rgba)
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
static void ConvertGRAYToRGBA(FT_Bitmap* source, unsigned char* rgba)
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

	bool  IsBold()
	{
		return m_bold;
	}
	bool  IsItalic()
	{
		return m_italic;
	}
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
		unsigned char* m_data;
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
	bool                       m_italic;
	int                        m_tall;
	int                        m_rgbaSize;
	unsigned char* m_rgba;
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
	bool GetFontInfo(VkFont* font, int code, int& wide, int& tall, int& horiBearingX, int& horiBearingY, int& horiAdvance, unsigned char*& bytes, float* coords)
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

	vector<VkFont*> ReadFontFromFile()
	{
		QDir dir("fontCache");

		QFileInfoList list = dir.entryInfoList();
		for (int i = 0; i < list.size(); ++i)
		{
			QFileInfo fileInfo = list.at(i);
			if (fileInfo.isDir())
			{
				QFile xmlFile(fileInfo.absolutePath() + "\\font.xml");				
				QFile datafile(fileInfo.absolutePath()+ "\\data.bin");
				if (!xmlFile.open(QIODevice::ReadWrite) || datafile.open(QIODevice::ReadWrite | QIODevice::Append))
				{
					continue;
				}
				VkFont* font = new VkFont;
				QXmlStreamReader xmlReader(&xmlFile);
				xmlReader.readNext(); // 读取下一个记号，它返回记号的类型

				bool IsBold;
				bool IsItalic;

				glyphMetrics metrics;
				float coord[4];

				std::vector<QByteArray> bytesArray;

				while (!xmlReader.atEnd())
				{
					if (xmlReader.isEndElement())
					{
						xmlReader.readNext();
					}

					else if (xmlReader.isStartElement())
					{
						if (xmlReader.name() == "fontRoot")
						{
							xmlReader.readNext();
						}
						else if (xmlReader.name() == "font")
						{
							QXmlStreamAttributes attrs= xmlReader.attributes();
							IsBold = attrs.value("bold").toInt();
							IsItalic = attrs.value("italic").toInt();
						}
						else if (xmlReader.name() == "char")
						{
							QXmlStreamAttributes attrs = xmlReader.attributes();
							metrics.width = attrs.value("w").toInt();
							metrics.height = attrs.value("h").toInt();
							metrics.horiBearingX = attrs.value("bx").toInt();
							metrics.horiBearingY = attrs.value("by").toInt();
							metrics.horiAdvance = attrs.value("ad").toInt();
							coord[0] = attrs.value("c0").toFloat();
							coord[1] = attrs.value("c1").toFloat();
							coord[2] = attrs.value("c2").toFloat();
							coord[3] = attrs.value("c3").toFloat();

							bytesArray.push_back(datafile.read(metrics.width * metrics.height * 4 * sizeof(float))); 
						}
					}
					else
					{
						xmlReader.readNext();
					}
				}

				font->m_bold = IsBold;
				font->IsItalic = IsItalic;

				font->m_face 
			}

		}
	}
	void  SaveFont(VkFont* font)
	{
		char* fontName = font->GetFontName();
		int   fontSize = font->GetFontTall();

		QFile xmlfile(QString("fontCache\\") + fontName + "\\font.xml");
		xmlfile.open(QIODevice::ReadWrite| QIODevice::Append);

		QFile datafile(QString("fontCache\\") + fontName + "\\data.bin");
		datafile.open(QIODevice::ReadWrite | QIODevice::Append);

		QXmlStreamWriter xmlWriter(&xmlfile);
		xmlWriter.setAutoFormatting(true); //格式输出，也就是会有标签的缩进
		xmlWriter.writeStartDocument();//开始进行 XML 文档的输出,这个函数会写下 
		xmlWriter.writeStartElement("fontRoot"); //根节点

		xmlWriter.writeStartElement("font");
		xmlWriter.writeAttribute("size", QString::number(fontSize));
		xmlWriter.writeAttribute("bold", font->IsBold() ? "1" : "0");
		xmlWriter.writeAttribute("italic",font->IsItalic() ? "1" : "0");

		for (int i = 0; i < font->m_chars.size(); i++)
		{
			xmlWriter.writeStartDocument("char");
			xmlWriter.writeAttribute("ch", QString::number(font->m_chars[i]->m_code));

			glyphMetrics metrics;
			unsigned char* data;
			float coord[4];
			font->m_chars[i]->getInfo(&metrics, data, coord);

			xmlWriter.writeAttribute("w",  QString::number(metrics.width));
			xmlWriter.writeAttribute("h"  ,QString::number(metrics.height));
			xmlWriter.writeAttribute("ad", QString::number(metrics.horiAdvance));
			xmlWriter.writeAttribute("bx", QString::number(metrics.horiBearingX));
			xmlWriter.writeAttribute("by", QString::number(metrics.horiBearingY));

			xmlWriter.writeAttribute("c0", QString::number(coord[0]));
			xmlWriter.writeAttribute("c1", QString::number(coord[1]));
			xmlWriter.writeAttribute("c2", QString::number(coord[2]));
			xmlWriter.writeAttribute("c3", QString::number(coord[3]));

			datafile.write((char*)data,4*metrics.width*metrics.height);
			xmlWriter.writeEndElement();
		}
		xmlWriter.writeEndElement();		
		xmlWriter.writeEndDocument();

		xmlfile.flush();
		datafile.flush();

		xmlfile.close();
		datafile.close();
	}
};