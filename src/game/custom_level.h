#ifndef CUSTOM_LEVEL_H
#define CUSTOM_LEVEL_H

#include "include/types.h"
#include "include/macros.h"

extern Collision custom_level_collision[];
extern Gfx* custom_level_dl;

void custom_level_new();
void custom_level_vertex(float x, float y, float z, float u, float v);
void custom_level_face();
void custom_level_texture(char* texture);
void custom_level_finish();

Gfx* geo_custom_level(s32 sp, struct GraphNode* graphNode, UNUSED void* context);

#endif