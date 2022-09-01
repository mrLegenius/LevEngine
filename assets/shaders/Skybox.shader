#shader vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in int a_EntityID;

struct VertexOutput
{
	vec3 TexCoord;
};

layout (location = 0) out VertexOutput Output;
layout (location = 1) out flat int v_EntityID;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	Output.TexCoord = a_Position;
	v_EntityID = a_EntityID;
	vec4 pos = u_Projection * u_View * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
}

#shader fragment
#version 460 core
layout (location = 0) out vec4 o_Color;
layout (location = 1) out int o_EntityID;

struct VertexOutput
{
	vec3 TexCoord;
};

layout (location = 0) in VertexOutput Input;
layout (location = 1) in flat int v_EntityID;

layout (binding = 0) uniform samplerCube u_Textures[32];

void main()
{
	o_EntityID = v_EntityID;
	o_Color = texture(u_Textures[0], Input.TexCoord);
}