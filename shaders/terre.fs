#version 330
in float ild;
in vec2 texCoord;
out vec4 fragColor;

uniform vec4 scolor;
uniform vec4 lcolor;

uniform sampler2D tex; 
uniform int sky;
uniform int sun;
void main() {
     
  fragColor = texture(tex, texCoord) * ild ;//*  lcolor;
}
