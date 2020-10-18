#version 330 core
out vec4 FragColor;

in vec3 fragColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture0;
uniform vec2 stepSize;
uniform int program_switcher;
uniform float alpha_value;
//#define alpha_value 0.5f
#define radius 4
// vec3(0.15f, 0.15f, 0.9f); road color
void main()
{
    //FragColor = texture2D(texture0, TexCoord);
    //if(FragColor.rgb == vec3(0.0f,0.0f,0.0f)) discard;
    //return;

    mat4 test_for_blue;
    mat4 texture_roi;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            test_for_blue[i][j] = 0.9f;
            texture_roi[i][j] = texture2D(texture0, TexCoord + vec2(j*stepSize.st[0], i*stepSize.st[1])).rgb;
        }
    }
    int num_of_blue_pixels = dot(texture_roi, test_for_blue)/16.0f;

    if(texture2D(texture0, TexCoord).rgb != vec3(0.0f, 0.0f, 0.0f)){ //The fragment already has a color
        FragColor = vec4(texture2D(texture0, TexCoord).rgb, alpha_value);
        return;
    }

    vec4 final_color = vec4(texture(texture0, TexCoord).rgb, alpha_value); //initialize the color with the current pixel
    for(int u = -radius; u<radius; u++){
        int v_range = int(sqrt(pow(radius,2) - pow(u,2)));
        for(int v = -v_range; v< v_range; v++){
            vec3 color = texture2D(texture0, TexCoord + vec2(u*stepSize.st[0], v*stepSize.st[1])).rgb;
            if(color.rgb != vec3(0.0f, 0.0f, 0.0f)){
                final_color = vec4(color, alpha_value);
            }
        }
    }
    FragColor = final_color;

    if(FragColor.rgb == vec3(0.0f,0.0f,0.0f)) discard;
}

/*
#version 330 core

out vec4 FragColor;

in vec3 fragColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture0;
uniform vec2 stepSize;
uniform int program_switcher;

void main()
{
    FragColor = vec4(fragColor, 1.0f);
    //return;
    for(int u = -10; u<10; u++){
        for(int v = -10; v<10; v++){
            vec4 color = texture2D(texture0, TexCoord + vec2(u*stepSize.st[0], v*stepSize.st[1] ));
            if(color == vec4(0.0f, 0.0f, 1.0f, 1.0f)){
                FragColor = color;
                return;
            }
        }
    }
}
*/
