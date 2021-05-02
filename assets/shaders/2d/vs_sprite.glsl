/*
   Copyright 2020 Alexandre Pires (c.alexandre.pires@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#version 420 core

layout(location = 0) in vec2 m_pos;
layout(location = 1) in vec2 m_tex;

layout(std140, binding = 0) uniform Matrices {
    mat4 m_proj;
    mat4 m_view;
    mat4 m_projview;
    mat4 m_ortho;
};

uniform mat4 m_model;
out vec2 t_coord;

void main() {
    t_coord = m_tex;
    gl_Position = m_ortho * m_model * vec4(m_pos, 0.0, 1.0);
}