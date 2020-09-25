

#version 330 core
out vec4 FragColor;

in vec3 fragColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture0;

void main()
{
    FragColor = texture(texture0, TexCoord);
    //FragColor = vec4(fragColor.rgb,1.0f); //mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}

