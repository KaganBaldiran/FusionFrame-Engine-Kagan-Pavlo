#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

FUSIONCORE::Texture2D::Texture2D(const char* filePath , GLenum target , GLenum type , GLuint Mag_filter , GLuint Min_filter , GLuint Wrap_S_filter, GLuint Wrap_T_filter , bool Flip)
{
	PathData = std::string(filePath);

	glGenTextures(1, &id);
	glBindTexture(target, id);

	stbi_set_flip_vertically_on_load(Flip);

	unsigned char* data = stbi_load(filePath, &width, &height, &channels, NULL);
	if (!data)
	{
		LOG_ERR("Error importing the texture! :: " << filePath);
		return;
	}

	GLenum format;
	if (channels == 1)
	{
		format = GL_RED;
	}
	else if (channels == 3)
	{
		format = GL_RGB;
	}
	else if (channels == 4)
	{
		format = GL_RGBA;
	}

	glTexImage2D(target, 0, format, width, height, 0, format, type, data);
	glTextureParameteri(target, GL_TEXTURE_MAG_FILTER, Mag_filter);
	glTextureParameteri(target, GL_TEXTURE_MIN_FILTER, Min_filter);
	glTextureParameteri(target, GL_TEXTURE_WRAP_S, Wrap_S_filter);
	glTextureParameteri(target, GL_TEXTURE_WRAP_T, Wrap_T_filter);

	glGenerateMipmap(target);

	stbi_image_free(data);
	glBindTexture(target, 0);

	if (Flip)
	{
		stbi_set_flip_vertically_on_load(false);
	}

	LOG_INF("Texture Imported : " << PathData);
}


void FUSIONCORE::Texture2D::Clear()
{
	glDeleteTextures(1, &id);
	LOG_INF("Texture Cleaned : " << PathData);
}

GLuint FUSIONCORE::Texture2D::GetTexture()
{
	return this->id;
}

int FUSIONCORE::Texture2D::GetWidth()
{
	return this->width;
}

int FUSIONCORE::Texture2D::GetHeight()
{
	return this->height;
}

void FUSIONCORE::Texture2D::Bind(GLuint slot , GLuint shader , const char* uniform)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
	glUniform1i(glGetUniformLocation(shader, uniform), slot);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FUSIONCORE::Texture2D::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}

std::string FUSIONCORE::Texture2D::GetFilePath()
{
	return this->PathData;
}

int FUSIONCORE::Texture2D::GetChannelCount()
{
	return this->channels;
}