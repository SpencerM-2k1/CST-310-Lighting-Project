#version 330 core
layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec2 inTexCoord;

out vec2 TexCoord;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(inPosition.x, inPosition.y, 0.0, 1.0);
    TexCoord = inTexCoord;
}
