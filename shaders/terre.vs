#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tC;

uniform mat4 proj;
uniform mat4 mod;
uniform mat4 view;

out float ild;
out vec2 texCoord; 
const vec3 Ld0 = vec3(2, -1, -1); // origine de la lumière

void main() {


     // calcul de l'indice de lumière
     // on calcul la normale du vertex, on normalise, et on la compare avec le vecteur de luminosité, pour obtenir l'indice de luminosité
     // plus ils divergent, moins la lumière frappe le vertex
     vec4 mvpos = view * mod * vec4(pos, 1.0);
     vec3 N = ((inverse(transpose(view * mod))) * vec4(normal, 0.0)).xyz;
     N = normalize(N);
     vec3 Ld = normalize(Ld0);
     ild = clamp(dot(N, -Ld), 0.0, 1.0);

     //
     gl_Position = proj* mvpos;
     texCoord = tC;
}