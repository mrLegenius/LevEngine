#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

void main()
{
	gl_Position = u_LightSpaceMatrix * u_Model * vec4(position, 1.0f);
};

#shader fragment
#version 330 core

void main()
{
	 gl_FragDepth = gl_FragCoord.z;
}