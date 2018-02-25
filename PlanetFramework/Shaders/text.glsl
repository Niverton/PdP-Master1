<VERTEX>
#version 300 es

//in
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec2 charSize;
layout (location = 4) in uint channel;
//out
flat out uint geo_channel;
out vec3 geo_position;
out vec4 geo_color;
out vec2 geo_texCoord;
out vec2 geo_charSize;
//
//
void main()
{
  geo_position = position;
  geo_color = color;
  geo_texCoord = texCoord;
  geo_charSize = charSize;
  geo_channel = channel;
}
</VERTEX>
<GEOMETRY>
#version 400 core

layout(points, invocations = 1) in;
layout(triangle_strip, max_vertices=4) out;

in VSO
{
  flat uint channel;
  vec3 position;
  vec4 color;
  vec2 texCoord;
  vec2 charSize;
} inputs[];
out GSO 
{
  flat uint channel;
  vec4 color;
  vec2 texCoord;
} outputs;

uniform mat4 transform;
uniform vec2 texSize;

void main()
{
  vec2 cS = inputs[0].charSize;

  vec3 pos = inputs[0].position;
  vec2 tc = inputs[0].texCoord;


  outputs.channel = inputs[0].channel;
  gl_Position = transform * vec4(pos.x, pos.y + cS.y, pos.z, 1);
  outputs.color = inputs[0].color;
  outputs.texCoord = tc+vec2(0, cS.y)/texSize;
  EmitVertex();

  outputs.channel = inputs[0].channel;
  gl_Position = transform * vec4(pos.x + cS.x, pos.y + cS.y, pos.z, 1);
  outputs.color = inputs[0].color;
  outputs.texCoord = tc+vec2(cS.x, cS.y)/texSize;
  EmitVertex();

  outputs.channel = inputs[0].channel;
  gl_Position = transform * vec4(pos.x, pos.y, pos.z, 1);
  outputs.color = inputs[0].color;
  outputs.texCoord = tc;
  EmitVertex();

  outputs.channel = inputs[0].channel;
  gl_Position = transform * vec4(pos.x + cS.x, pos.y, pos.z, 1);
  outputs.color = inputs[0].color;
  outputs.texCoord = tc+vec2(cS.x, 0)/texSize;
  EmitVertex();

  EndPrimitive();
} 
</GEOMETRY>
<FRAGMENT>
#version 300 es

//IN
flat in uint frag_channel;
in highp vec4 frag_color;
in highp vec2 frag_texCoord;

uniform sampler2D fontTex;

out highp vec4 outColor;

void main()
{
  highp float tValue = texture( fontTex, frag_texCoord )[frag_channel];
  if( tValue < 1.0/255.0 ) {
    discard;
  }
  highp vec4 textureColor = frag_color*tValue;
  outColor = textureColor;
}
</FRAGMENT>
