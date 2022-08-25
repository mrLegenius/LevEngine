#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	TexCoords = position;
	vec4 pos = u_Projection * u_View * vec4(position, 1.0);
    gl_Position = pos.xyww;
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube u_SkyboxTexture;

void main()
{
	FragColor = texture(u_SkyboxTexture, TexCoords);
}