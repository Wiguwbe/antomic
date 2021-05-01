#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

layout (std140, binding = 0) uniform Matrices
{
    mat4 m_proj;
    mat4 m_view;
    mat4 m_projview;
};

uniform mat4 m_model;
out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = m_projview * m_model * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}