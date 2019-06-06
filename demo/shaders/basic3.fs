#version 330
uniform vec4 lumpos;
uniform int sky, fog;
uniform sampler1D degrade;
in vec2 vsoTexCoord;
in vec3 vsoNormal;
in vec4 vsoModPosition;
in vec3 vsoPosition;

out vec4 fragColor;

void main(void) {
  vec3 lum = normalize(vsoModPosition.xyz - lumpos.xyz);
  fragColor = vec4(texture(degrade, (1.0 + vsoPosition.y) / 2.0).rgb * (vec3(0.1) + 0.9 * vec3(1) * dot(normalize(vsoNormal), -lum)), 1.0);

  const float SQRT2 = 1.442695, fog_density2 = 0.001;
  const vec4 fog_color = vec4(0.5, 0.5, 0.5, 1.0);
  if(sky != 0) {
    float x = 0.02 + 0.96 * abs(2.0 * vsoTexCoord.x - 1);
    float y = clamp(2.0 * (1.0 - vsoTexCoord.y), 0, 1);
    //vec4  tex = vec4(vec4(0, 0.5, 0.5, 1.0) + pow(texture(degrade, vec2(x, 0.96 * y)).rrrr, vec4(4.0)));
    vec4 tex = vec4(texture(degrade, (1.0 + vsoPosition.y) / 2.0).rgb * (vec3(0.1) + 0.9 * vec3(1) * dot(normalize(vsoNormal), -lum)), 1.0);
     if(fog!= 0) {
      float ffactor = 1.0 - pow(y, 30);
      fragColor = mix(fog_color, tex, ffactor);
    } else{
            fragColor = tex;
    }
  }
  else{
    vec4 tex = vec4(texture(degrade, (1.0 + vsoPosition.y) / 2.0).rgb * (vec3(0.1) + 0.9 * vec3(1) * dot(normalize(vsoNormal), -lum)), 1.0);
    if(fog != 0) {
      float z = gl_FragCoord.z / gl_FragCoord.w;
      float ffactor = exp(-fog_density2 * z * z);
      ffactor = clamp(ffactor, 0.0, 1.0);
      fragColor = mix(fog_color, tex, ffactor);
      fragColor = mix(fog_color, tex, ffactor);
    } 
    else{
      fragColor = tex;
    }
  }
}
