#version 150

uniform mat4 modelViewProjectionMatrix; // Projection * ModelView (provided in OF)
in vec4 position; //provided in OF
in vec3 normal; //provided in OF
in vec2 texcoord; //provided in OF

//uniform vec4 uColor; // get rid of this

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;

// send to fragment shader
out vec4 pos;
out vec3 norm;
out float zHeight;

uniform float DispScale; // scalar for displacement map

uniform sampler2DRect DispTex;

void main()
{
    // mesh displacement
    float displacementZ = texture(DispTex, texcoord).r;
    vec4 vPos = position;
    vPos.z = (vPos.z + 1.0) * displacementZ * DispScale;
    vec4 modifiedMesh = modelViewProjectionMatrix * vPos;
    
    // for fragment shader
    norm = normalize(NormalMatrix * normal);
    pos = ModelViewMatrix * position;
    zHeight = vPos.z;
    
    gl_Position = modifiedMesh;
}