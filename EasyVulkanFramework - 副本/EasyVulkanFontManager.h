#pragma once
#include <QMap>
#include <QFile>
#include <qvector2d.h>
#include <string>

#include <SDL_ttf.h>
#include <vulkan/vulkan.h>

#include "EasyVulkanAssert.h"
#include "EasyVulkanTextureManager.h"

using namespace std;
#pragma comment(lib,"SDL2_ttf.lib")
#pragma comment(lib,"SDL2.lib")

struct Character
{
	Texture*		texture;
	QVector2D		bearing;
	int		        advance;    //Offset to advance to next glyph
	int             xMax;
	int             xMin;
	int             yMax;
	int             yMin;
};

class EasyVulkanFontManager
{
public:
	static bool Init(QString fontFile, int size, bool bold, bool italic);

	static Character* GetCharInfo(char ch);

private:
	static QMap<char, Character*> m_CharMap;
};


