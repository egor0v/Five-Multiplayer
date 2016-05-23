#pragma once

float draw_menu_line(std::string caption, float lineWidth, float lineHeight, float lineTop, float lineLeft, float textLeft, bool active, bool title, bool bDrawRect = true, bool rescaleText = true);
void draw_text(float x, float y, char* chSampleText, color_t color);
void draw_rect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7);