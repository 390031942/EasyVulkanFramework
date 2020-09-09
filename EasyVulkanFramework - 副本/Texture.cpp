#include "Texture.h"

Texture::Texture()
{
	m_DataByteCount = 4 * sizeof(float);
	m_pTextureData = NULL;
	transform = Transform();
}

void Texture::Create(QString path)
{
	QImage image;
	image.load(path);
	image = image.convertToFormat(QImage::Format_RGBA8888);

	m_Size = image.size();
	int x = ceil(log2(m_Size.width()));
	int y = ceil(log2(m_Size.height()));

	int z = max(x, y);
	int u = pow(2, z);
	image = AdjustSize(image, m_Size.width(), m_Size.height(), u);
	m_Size = image.size();
	image.save("12345.png");


	m_DataByteCount = 4 * m_Size.height() * m_Size.width() * sizeof(unsigned char);
	m_Name = QFileInfo(path).fileName();


	m_pTextureData = new unsigned char[m_DataByteCount];
	memcpy(m_pTextureData, image.bits(), m_DataByteCount);
}

void Texture::Create(uchar* data,int width,int height,QImage::Format format)
{
	QImage image(data,width,height, format);
	image = image.convertToFormat(QImage::Format_RGBA8888);
	image.save("chars/" + QUuid::createUuid().toString()+".png");
	m_Size = image.size();
	int x = ceil(log2(m_Size.width()));
	int y = ceil(log2(m_Size.height()));

	int z = max(x, y);
	int u = pow(2, z);
	image = AdjustSize(image, m_Size.width(), m_Size.height(), u);

	int x_begin = (u - width) / 2;
	int y_begin = (u - height) / 2;

	int x_end = u - x_begin;
	int y_end = u - y_begin;

	m_TexCoorRect[0]=(float)x_begin/u;
	m_TexCoorRect[1]=(float)x_end/u;
	m_TexCoorRect[2]=(float)y_begin/u;
	m_TexCoorRect[3]=(float)y_end/u;

	m_Size = image.size();

	
	m_DataByteCount = 4 * m_Size.height() * m_Size.width() * sizeof(unsigned char);
	m_Name = "Text"+QUuid::createUuid().toString();


	m_pTextureData = new unsigned char[m_DataByteCount];
	memcpy(m_pTextureData, image.bits(), m_DataByteCount);
}

QImage Texture::AdjustSize(QImage image, int width, int height, int n,bool drawBorder)
{
	int x_begin = (n - width) / 2;
	int y_begin = (n - height) / 2;

	int x_end = n - x_begin;
	int y_end = n - y_begin;

	int count = image.byteCount();

	uchar* bits = new uchar[n * n * 4];
	uchar* old_bits = image.bits();


	for (int y = 0; y < n; y++)
	{
		for (int x = 0; x< n; x++)
		{
			QColor color = image.pixelColor(x - x_begin, y - y_begin);
			if (!(x < x_begin || (width % 2 == 0 ? (x >= x_end) : (x + 1 >= x_end)) || y < y_begin || (height % 2 == 0 ? (y >= y_end) : (y + 1 >= y_end))))
			{
				if (drawBorder)
				{
					if (x == x_begin || (width % 2 == 0 ? (x == x_end - 1) : (x + 1 == x_end - 1)) || y == y_begin || (height % 2 == 0 ? (y == y_end - 1) : (y + 1 == y_end - 1)))
					{
						bits[4 * n * y + 4 * x + 0] = 0x00;
						bits[4 * n * y + 4 * x + 1] = 0x00;
						bits[4 * n * y + 4 * x + 2] = 0x00;
						bits[4 * n * y + 4 * x + 3] = 0xff;
					}
				}
				else
				{
					bits[4 * n * y + 4 * x + 0] = color.red();
					bits[4 * n * y + 4 * x + 1] = color.green();
					bits[4 * n * y + 4 * x + 2] = color.blue();
					bits[4 * n * y + 4 * x + 3] = color.alpha();
				}


			}
			else
			{
				bits[4 * n * y + 4 * x + 0] = 0xff;
				bits[4 * n * y + 4 * x + 1] = 0xff;
				bits[4 * n * y + 4 * x + 2] = 0xff;
				bits[4 * n * y + 4 * x + 3] = 0x00;
			}

		}
	}

	QImage newImage(bits, n, n, QImage::Format::Format_RGBA8888);
	return newImage;
}

void Texture::SetTexCoorRect(float left, float right, float top, float bottom)
{
	m_TexCoorRect[0] = left;
	m_TexCoorRect[1] = right;
	m_TexCoorRect[2] = top;
	m_TexCoorRect[3] = bottom;
}

void Texture::SetPosition(float x, float y, float z)
{
	transform.position = QVector3D(x, y, z);
}

VkImage Texture::GetVkImage()
{
	return this->m_Resource.vkImage;
}

VkDescriptorImageInfo Texture::GetVkImageInfo()
{
	return this->m_Resource.vkImageInfo;
}

void Texture::Draw()
{
	VkCommandBuffer cmd = EasyVulkanTextureManager::getCmdBuffer();
	VkPipeline pipeline = EasyVulkanTextureManager::getPipeline();
	VkPipelineLayout pipelineLayout = EasyVulkanTextureManager::getPipelineLayout();
	VkDescriptorSet* desSetPointer = EasyVulkanTextureManager::getDescriptorSetPtr();

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, desSetPointer, 0, NULL);
	const VkDeviceSize offsetsVertex[1] = { 0 };
	vkCmdBindVertexBuffers
	(
		cmd,
		0,
		1,
		&(m_Resource.vkVertexBuffer),
		offsetsVertex
		);
	vkCmdDraw(cmd, 6, 1, 0, 0);
}

QString Texture::GetTextureName()
{
	return m_Name;
}

QSize Texture::GetSize()
{
	return m_Size;
}

float* Texture::GetVertexRect()
{
	return this->m_VertexRect;
}

float* Texture::GetTexCoorRect()
{
	return this->m_TexCoorRect;
}
