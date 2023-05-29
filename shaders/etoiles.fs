#version 330

in vec4 mpos;
in vec3 mnormal;
in vec2 tcoord;
in float ild;
out vec4 fragColor;

uniform float zoom;

uniform sampler2D permTexture;
uniform sampler2D gradTexture;
#define ONE 0.00390625
#define ONEHALF 0.001953125
float fade(float t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0); // Improved fade, yields C2-continuous noise
}

float noise(vec2 P)
{
  vec2 Pi = ONE*floor(P)+ONEHALF; // Integer part, scaled and offset for texture lookup
  vec2 Pf = fract(P);             // Fractional part for interpolation

  // Noise contribution from lower left corner
  vec2 grad00 = texture(permTexture, Pi).rg * 4.0 - 1.0;
  float n00 = dot(grad00, Pf);

  // Noise contribution from lower right corner
  vec2 grad10 = texture(permTexture, Pi + vec2(ONE, 0.0)).rg * 4.0 - 1.0;
  float n10 = dot(grad10, Pf - vec2(1.0, 0.0));

  // Noise contribution from upper left corner
  vec2 grad01 = texture(permTexture, Pi + vec2(0.0, ONE)).rg * 4.0 - 1.0;
  float n01 = dot(grad01, Pf - vec2(0.0, 1.0));

  // Noise contribution from upper right corner
  vec2 grad11 = texture(permTexture, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  float n11 = dot(grad11, Pf - vec2(1.0, 1.0));

  // Blend contributions along x
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade(Pf.x));

  // Blend contributions along y
  float n_xy = mix(n_x.x, n_x.y, fade(Pf.y));

  // We're done, return the final noise value.
  return n_xy;
}

// https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 co){
  return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main(void) {
  // calcul bizarre pour limiter le nombre d'étoiles
  // normalement il devrait pas y avoir besoin de lumière car c'est les étoiles
  if (rand(tcoord) > 0.996 && rand(vec2(ild+tcoord[0], ild+tcoord[0])) > 0.9){
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
  } else {
    float n, A = 1.0, mf = 1.0;
    for (int i = 0; i < 3; ++i){
      n += A * noise(tcoord * 20.5f * mf);
      A = A/2.0;
      mf = mf * 2.0;
    }
    vec4 mycolor = vec4(n);
    fragColor = vec4(0.1, 0.1, 0.3, 0.0)*vec4(0.1, 0.1, 0.2, 0.0)* mycolor;
  }

}
