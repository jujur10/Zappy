#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// custom variables
uniform vec2 resolution;
uniform float outlineStrength = 1.0;
uniform vec3 outlineColor = vec3(255.0, 255.0, 255.0);
uniform float threshold = 0.99999;

void main()
{
    float x = 1.0/resolution.x;
    float y = 1.0/resolution.y;

    vec4 center = texture2D(texture0, fragTexCoord);
    vec4 left   = texture2D(texture0, fragTexCoord - vec2(x, 0.0));
    vec4 right  = texture2D(texture0, fragTexCoord + vec2(x, 0.0));
    vec4 top    = texture2D(texture0, fragTexCoord - vec2(0.0, y));
    vec4 bottom = texture2D(texture0, fragTexCoord + vec2(0.0, y));

    vec4 diff = abs(center - left) + abs(center - right) + abs(center - top) + abs(center - bottom);
    float edgeIntensity = clamp(length(diff), 0.0, 1.0);

    if (edgeIntensity > threshold) {
        finalColor = vec4(outlineColor, 1.0);
    } else {
        finalColor = texture2D(texture0, fragTexCoord);
    }
}