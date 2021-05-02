#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
uniform mat4 m_proj;
uniform mat4 m_view;
uniform mat4 m_model;
uniform mat4 m_projview;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = m_projview * m_model * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}