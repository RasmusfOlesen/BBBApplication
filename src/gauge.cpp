#include "Renderer.hpp"

int offX = 0;
int offY = 0;

uint32_t ScaleColor = 0x00FFFFFF;

//Draw guide lines
Renderer.drawArc( //Outside of tickmarks
    128 + offX,
    179 + offY,
    112,
    45,
    135,
    1,
    0x00FFFFFF)

Renderer.drawArc( //Inside of tickmarks
    128 + offX,
    179 + offY,
    94,
    45,
    135,
    1,
    0x00FFFFFF)

//Draw Bezel
Renderer.drawCircle(
    128 + offX,
    128 + offY,
    127,
    3,
    0x00C0C0C0) //LightGray 192,192,192

// Draw Needle hub
Renderer.fillCircle(
    128 + offX,
    179 + offY,
    22,
    0x00808080) //DarkGray 128,128,128

//############################################################
//Draw Major scale
renderer.drawLine( //Tick 1
    61 + offX,
    111 + offY,
    49 + offX,
    99 + offY,
    7,
    ScaleColor);

renderer.drawLine( //Tick 2
    87 + offX,
    93 + offY,
    79 + offX,
    78 + offY,
    7,
    ScaleColor);

renderer.drawLine( //Tick 3
    129 + offX,
    84 + offY,
    129 + offX,
    67 + offY,
    7,
    ScaleColor);

renderer.drawLine( //Tick 4
    175 + offX,
    97 + offY,
    183 + offX,
    82 + offY,
    7,
    ScaleColor);

renderer.drawLine( //Tick 5
    191 + offX,
    109 + offY,
    203 + offX,
    97 + offY,
    7,
    ScaleColor);

//############################################################
//Draw minor scale
renderer.drawLine( //Tick 1
    74 + offX,
    101 + offY,
    64 + offX,
    87 + offY,
    5,
    ScaleColor);

renderer.drawLine( //Tick 2
    108 + offX,
    86 + offY,
    104 + offX,
    70 + offY,
    5,
    ScaleColor);

renderer.drawLine( //Tick 3
    153 + offX,
    87 + offY,
    157 + offX,
    71 + offY,
    5,
    ScaleColor);

renderer.drawLine( //Tick 4
    186 + offX,
    104 + offY,
    196 + offX,
    90 + offY,
    5,
    ScaleColor);

//############################################################