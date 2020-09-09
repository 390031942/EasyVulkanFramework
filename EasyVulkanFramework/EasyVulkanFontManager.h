#pragma once
#include <ft2build.h>
#include <stdio.h>
#include <QImage>

#include <QXmlStreamWriter>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include "EasyVulkanGlobal.h"

#include "ChineseChars.h"
#include "Texture.h"

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
	void  release();
	bool  getCharInfo(int code, SingleText * &texture,glyphMetrics* metrics);
	bool  setCharInfo(int code, glyphMetrics metrics, unsigned char* bytes, float coords[])
	{
		SingleTextMap::iterator it = texts.find(code);

		if (it != texts.end())
		{
			return true;
		}

		SingleText* texture = new SingleText;
		texture->create(bytes, metrics.width, metrics.height, metrics.horiBearingX, metrics.horiBearingY, metrics.horiAdvance,coords);
		return true;
	}
	bool                       antialias;
	bool                       bold;
	bool                       italic;
	int                        tall;
	std::string                fontName;
	std::string                fontPath;
private:
	VkFont();
	~VkFont();

	bool create(FT_Library library, const char* filePath, FT_Long face_index, int tall, bool bold, bool italic, bool antialias);
	bool loadChar(int code, glyphMetrics* metrics);

	typedef std::map<int, SingleText*> SingleTextMap;

	FT_Library                 library;
	FT_Face                    face;
	int                        rgbaSize;
	unsigned char*             rgba;
	SingleTextMap              texts;

	friend class EasyVulkanFontManager;
};

class EasyVulkanFontManager
{
public:

	EasyVulkanFontManager();
	~EasyVulkanFontManager();
	bool init();
	void destory();


	VkFont* loadFont(std::string fontPath,int tall, bool bold, bool italic, bool antialias, int face = 0)
	{
		VkFont* vkFont = new VkFont();
		vkFont->create(library, fontPath.c_str(), 0, tall, bold, italic, antialias);

		if (fontMap.find(vkFont->fontName) != fontMap.cend())
		{
			fontMap[vkFont->fontName] = vkFont;
			saveFont(vkFont);
		}
		else
		{
			std::string fontName = vkFont->fontName;
			delete vkFont;
			vkFont = fontMap[fontName];
		}
		return vkFont;
	}

public:
	FT_Library                         library;
	std::map<std::string,VkFont*>      fontMap;

	vector<VkFont*> readFontFromFile()
	{
		QDir dir("fontCache");

		QFileInfoList list = dir.entryInfoList();
		for (int i = 0; i < list.size(); ++i)
		{
			QFileInfo fileInfo = list.at(i);
			if (fileInfo.isDir())
			{
				QFile jsonfile(fileInfo.absolutePath() + "\\font.json");
				QFile datafile(fileInfo.absolutePath() + "\\data.bin");
				if (!jsonfile.open(QIODevice::ReadWrite) || datafile.open(QIODevice::ReadWrite | QIODevice::Append))
				{
					continue;
				}
				VkFont* font = new VkFont;
				QJsonParseError error;
				QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonfile.readAll(), &error);

				VkAssert(error.error == QJsonParseError::NoError, "readFontFromFile");

				QJsonObject rootObj = jsonDoc.object();//取得最外层这个大对象
				font->fontName = rootObj.value("fontName").toString().toStdString();
				font->fontPath = rootObj.value("fontPath").toString().toStdString();
				font->create(library, font->fontPath.c_str(), 0, 64, false, false, false);
				
				int count = rootObj.value("count").toInt();

				QVariantList list = rootObj.value("chList").toVariant().toList();
				VkAssert(list.size() == count, "readFontFromFile");
				for (int i = 0; i < count; i++)
				{
					QVariantMap map = list[i].toMap();

					glyphMetrics gm{ map["width"].toInt(),
									 map["height"].toInt(),
									 map["bearingX"].toInt(),
									 map["bearingY"].toInt(),
									 map["advance"].toInt() };
					
					float* coords = new float[4]{  map["coordinate_Left"].toFloat(),
												   map["coordinate_Right"].toFloat(),
												   map["coordinate_Top"].toFloat(),
												   map["coordinate_Bottom"].toFloat() };

					int code = map["ch"].toInt();
					
					unsigned char* bytes =(unsigned char*)datafile.read(gm.height * gm.width * 4 * sizeof(float)).data();
					font->setCharInfo(code, gm, bytes,coords);
				}
				fonts.push_back(font);
			}

		}
	}
	void            saveFont(VkFont* font)
	{
		std::string fontName = font->fontName;
		int   fontSize = font->tall;

		QFile jsonFile(QString("fontCache\\") + fontName.c_str() + "\\font.json");
		jsonFile.open(QIODevice::ReadWrite| QIODevice::Append);

		QFile datafile(QString("fontCache\\") + fontName.c_str() + "\\data.bin");
		datafile.open(QIODevice::ReadWrite | QIODevice::Append);

		QMap<QString, QVariant> map;
		QVariantList chList;
		//定义基本字体信息
		map["fontName"] = QString(fontName.c_str());
		map["fontPath"] = font->fontName.c_str();
		map["count"] = 3500 + 96;

		for (int i = 0; i < font->chars.size(); i++)
		{
			QMap<QString, QVariant> chMap;
			
			chMap["ch"] = QString::number(font->chars[i]->code);

			glyphMetrics metrics;
			unsigned char* data;
			float coord[4];
			font->chars[i]->getInfo(&metrics, data, coord);

			chMap["width"]    =QString::number(metrics.width);
			chMap["height"]   =QString::number(metrics.height);
			chMap["advance"] = QString::number(metrics.horiAdvance);
			chMap["bearingX"] =QString::number(metrics.horiBearingX);
			chMap["bearingY"] =QString::number(metrics.horiBearingY);

			chMap["coordinate_Left"]   =QString::number(coord[0]);
			chMap["coordinate_Right"]  =QString::number(coord[1]);
			chMap["coordinate_Top"]    =QString::number(coord[2]);
			chMap["coordinate_Bottom"] =QString::number(coord[3]);

			datafile.write((char*)data,4*metrics.width*metrics.height);
			chList.append(chMap);
		}
		map["chList"] = chList;
		QJsonDocument doc = QJsonDocument::fromVariant(map);

		jsonFile.write(doc.toJson());
		jsonFile.flush();
		datafile.flush();

		jsonFile.close();
		datafile.close();
	}
};


class FontObject
{
private:
	int         fontID;
public:
	std::string fontName;
	int         fontSize;


};
