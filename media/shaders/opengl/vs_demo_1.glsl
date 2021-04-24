#version 330 core

layout(location = 0) in vec3 a_Position;
uniform mat4 m_proj;
uniform mat4 m_view;
uniform mat4 m_model;
uniform mat4 m_mvp;

out vec3 v_Position;

void main() {
    v_Position = a_Position;
    gl_Position = m_mvp * vec4(a_Position, 1.0);
}