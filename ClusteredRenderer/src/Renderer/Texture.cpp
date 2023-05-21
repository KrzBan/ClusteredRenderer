#include "Texture.hpp"

Texture::Texture(std::string_view filepath)
    : m_Path{ filepath }
{
    glGenTextures(1, &m_Handle);
    glBindTexture(GL_TEXTURE_2D, m_Handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    int width = 0, height = 0, bpp = 0;
    stbi_uc* data = stbi_load(m_Path.c_str(), &width, &height, &bpp, 0);

    GLenum internalFormat = 0, dataFormat = 0;
    switch (bpp) {
    case 1:	dataFormat = GL_RED; internalFormat = GL_RED;    break;
    case 2: dataFormat = GL_RG; internalFormat = GL_RG8;     break;
    case 3: dataFormat = GL_RGB; internalFormat = GL_RGB8;    break;
    case 4: dataFormat = GL_RGBA; internalFormat = GL_RGBA8;   break;

    }
    m_DataFormat = dataFormat;
    m_InternalFormat = internalFormat;

    if (data) {
        m_BPP = bpp;
        m_Width = width;
        m_Height = height;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  //to load data with weird width and heights
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        size_t pathIndex = m_Path.find_last_of("/");
        if (pathIndex == std::string::npos) pathIndex = 0;
        throw std::runtime_error(std::format("Failed to load texture: {}", m_Path.substr(pathIndex)));
    }

    stbi_image_free(data);
}

Texture::~Texture() {
    glDeleteTextures(1, &m_Handle);
}

void Texture::Bind(uint32 slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_Handle);
}
void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

uint32_t Texture::GetHandle() const {
    return m_Handle;
}

int Texture::GetWidth() const {
    return m_Width;
}
int Texture::GetHeight() const {
    return m_Height;
}