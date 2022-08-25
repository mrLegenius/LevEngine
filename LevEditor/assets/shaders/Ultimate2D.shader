#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TexTiling;
layout(location = 5) in int a_EntityID;

out vec2 v_TexCoord;
out vec4 v_Color;
out flat float v_TexIndex;
out flat int v_EntityID;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	gl_Position = u_Projection * u_View * vec4(a_Position, 1.0f);
	v_TexCoord = a_TexCoord * a_TexTiling;
	v_Color = a_Color;
	v_TexIndex = a_TexIndex;
	v_EntityID = a_EntityID;
};

#shader fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int entity;

in vec2 v_TexCoord;
in vec4 v_Color;
in flat float v_TexIndex;
in flat int v_EntityID;

uniform sampler2D u_Textures[32];

void main()
{
	color = texture(u_Textures[int(v_TexIndex)], v_TexCoord) * v_Color; 
	entity = v_EntityID;
};