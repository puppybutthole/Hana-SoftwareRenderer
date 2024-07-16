#pragma once

#include "color.h"

class RenderBuffer
{
public:
    RenderBuffer(int width, int height);
    ~RenderBuffer();

    int width, height;
    unsigned char* color_buffer;
    float* depth_buffer;
    int* triIdx_buffer;

    void set_triIdx(int x, int y, int idx);
    int get_triIdx(int x, int y);

    void set_depth(int x, int y, float depth);
    float get_depth(int x, int y);
    void set_color(int x, int y, Color color);
    Color get_color(int x, int y);

    void renderbuffer_release();
    void renderbuffer_clear_color(Color color);
    void renderbuffer_clear_depth(float depth);
    void renderbuffer_clear_idx(int triIdx);
};