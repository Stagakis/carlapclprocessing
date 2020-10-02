

#version 330 core
out vec4 FragColor;

in vec3 fragColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture0;
uniform int program_switcher;

void main()
{
    switch(program_switcher){
        case 0:
            FragColor = texture(texture0, TexCoord);
            break;
        case 1:
            FragColor = vec4(fragColor.rgb,1.0f);
            break;
    }
    //FragColor = vec4(fragColor.rgb,1.0f); //mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}

