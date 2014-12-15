#version 150

uniform mat4 modelViewProjectionMatrix; // Projection * ModelView (provided in OF)
in vec4 position; //provided in OF
in vec3 normal; //provided in OF
in vec2 texcoord;

//uniform vec4 uColor; // get rid of this

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;

//out     vec3 LightIntensity;
flat out     vec4 pos;
flat out     vec3 norm;

//uniform vec4 LightPosition;// Light position in eye coords.
//uniform vec3 Kd;           // Diffuse reflectivity
//uniform vec3 Ld;           // Light source intensity
//uniform vec3 Ka;           // Ambient reflectivity
uniform float dispScale;        // scalar for displacement map

uniform sampler2DRect dispTex;

void main() {
    // mesh displacement
    float displacementZ = texture(dispTex, texcoord).r;
    vec4 vPos = position;
    vPos.z = (vPos.z + 1.0) * displacementZ * dispScale;
    vec4 modifiedHeight = modelViewProjectionMatrix * vPos;
    
    // for fragment shader
    norm = normalize(NormalMatrix * normal);
    pos = ModelViewMatrix * position;
    
    //gl_Position = modelViewProjectionMatrix * position;
    gl_Position = modifiedHeight;
}