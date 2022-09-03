#shader vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

struct VertexOutput
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
    float TilingFactor;
};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat float v_TexIndex;
layout (location = 5) out flat int v_EntityID;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
    vec3 u_CameraPosition;
};

uniform mat4 u_Model;

void main()
{
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0f);

    Output.Position = vec3(u_Model * vec4(a_Position, 1.0f));
    Output.Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    Output.TexCoord = a_TexCoord;
    Output.TilingFactor = a_TilingFactor;
    v_TexIndex = a_TexIndex;
    v_EntityID = a_EntityID;
}

#shader fragment
#version 460 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct VertexOutput
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
    float TilingFactor;
};

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
    vec3 u_CameraPosition;
};

layout(std140, binding = 1) uniform Lights
{
    DirLight u_DirLight;
};

layout (location = 0) in VertexOutput Input;
layout (location = 4) in flat float v_TexIndex;
layout (location = 5) in flat int v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Input.Normal);
    vec3 viewDir = normalize(u_CameraPosition - Input.Position);

    vec3 result = CalcDirLight(u_DirLight, norm, viewDir);

    //vec4 texColor = texture(u_Textures[int(v_TexIndex)], Input.TexCoord * Input.TilingFactor);

    //if (texColor.a == 0.0) discard;

    o_Color = vec4(result, 1.0f);
    o_EntityID = v_EntityID;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);// u_Material.shininess);
    // combine results
    vec3 ambient = light.ambient;// * u_Material.ambient;
    vec3 diffuse = light.diffuse * diff;// * u_Material.diffuse;
    vec3 specular = light.specular * spec;// * u_Material.specular;

    return (ambient + diffuse + specular);
}