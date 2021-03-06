#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 color;

out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPos;
out vec4 FragPosLightSpace;
out vec3 Color;
out vec4 DebugColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view *  model * vec4(position, 1.0f);
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;  

    TexCoords = texCoord;
    Color = color;

    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    DebugColor = vec4(0, 0, 1, 1);
} 