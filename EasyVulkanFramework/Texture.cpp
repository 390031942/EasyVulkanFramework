#include "Texture.h"

TextureBase::TextureBase(){}


void SingleText::expand(int n,bool drawBorder)
{
	int x_begin = 0;
	int y_begin = 0;

	int width = this->actualSize.width();
	int height = this->actualSize.height();

	int x_end = width;
	int y_end = height;

	this->coordinate[0] = 0;
	this->coordinate[1] = (float)width / n;
	this->coordinate[2] = 0;
	this->coordinate[3] = (float)height / n;
	uchar* bits = new uchar[n * n * 4]{0};

	//处理逐行
	for (int y = 0; y < y_end; y++)
	{
		//处理每一列
		for (int x = 0; x < x_end; x++)
		{
			if (drawBorder)
			{
				if(y == 0||y== y_end || x==0||x == x_end)
				{
					bits[4 * n * y + 4 * x + 0] = 0x00;
					bits[4 * n * y + 4 * x + 1] = 0x00;
					bits[4 * n * y + 4 * x + 2] = 0x00;
					bits[4 * n * y + 4 * x + 3] = 0xff;
				}
			}
			else
			{
				bits[4 * n * y + 4 * x + 0] = bytes[4 * n * y + 4 * x + 0];
				bits[4 * n * y + 4 * x + 1] = bytes[4 * n * y + 4 * x + 1];
				bits[4 * n * y + 4 * x + 2] = bytes[4 * n * y + 4 * x + 2];
				bits[4 * n * y + 4 * x + 3] = bytes[4 * n * y + 4 * x + 3];
			}
		}
	}

	free(this->bytes);
	this->bytes = bits;
}
