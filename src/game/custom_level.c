#include "custom_level.h"

#include <string.h>

#include "include/types.h"
#include "include/surface_terrains.h"

Collision custom_level_collision[1024 * 64] = {
    COL_INIT(),
    COL_VERTEX_INIT(0),
    COL_TRI_INIT(0, 0),
    COL_TRI_STOP(),
    COL_END(),
};
Gfx* custom_level_dl;
Gfx* geo_iter;
Vtx* vertices;
int* faces;
char** textures;
int vertex_iter = 0;
int face_iter = 0;
int used_vert = 0;
int tex_iter = 0;

Lights1 lights = gdSPDefLights1(0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0x28, 0x28, 0x28);

void custom_level_new() {
    if (vertices != NULL) {
        free(vertices);
        free(faces);
        free(custom_level_dl);
        for (int i = 0; i < tex_iter; i++) {
            free(textures[i]);
        }
        free(textures);
        geo_iter = NULL;
    }
    vertices = malloc(sizeof(Vtx) * 8192);
    faces = malloc(sizeof(int) * 3 * 8192);
    custom_level_dl = malloc(sizeof(Gfx) * 16384);
    textures = (char**)malloc(sizeof(char*) * 128);
    for (int i = 0; i < 128; i++) {
        textures[i] = (char*)malloc(sizeof(char) * 256);
    }
    vertex_iter = 0;
    face_iter = 0;
    used_vert = 0;
    tex_iter = 0;
    geo_iter = custom_level_dl;
	gDPPipeSync(geo_iter++);
	gDPSetCombineLERP(geo_iter++, TEXEL0, 0, SHADE, 0, 0, 0, 0, ENVIRONMENT, TEXEL0, 0, SHADE, 0, 0, 0, 0, ENVIRONMENT);
	gSPTexture(geo_iter++, 65535, 65535, 0, 0, 1);
	gSPSetLights1(geo_iter++, lights);
    custom_level_texture("");
}

void custom_level_vertex(float x, float y, float z, float u, float v) {
    if (vertex_iter == 8192) return;
    vertices[vertex_iter].v.ob[0] = x;
    vertices[vertex_iter].v.ob[1] = y;
    vertices[vertex_iter].v.ob[2] = z;
    vertices[vertex_iter].v.flag = 0;
    vertices[vertex_iter].v.tc[0] = u;
    vertices[vertex_iter].v.tc[1] = v;
    vertices[vertex_iter].v.cn[0] = 255;
    vertices[vertex_iter].v.cn[1] = 255;
    vertices[vertex_iter].v.cn[2] = 255;
    vertices[vertex_iter].v.cn[3] = 255;
    vertex_iter++;
}

void custom_level_face() {
    int begin = used_vert;
    int num_vert = vertex_iter - used_vert;
    gSPVertex(geo_iter++, vertices + begin, num_vert, 0);
    for (int i = 2; i < num_vert; i++) {
        if (face_iter == 8192) break;
        faces[face_iter * 3 + 0] = begin;
        faces[face_iter * 3 + 1] = i - 1 + begin;
        faces[face_iter * 3 + 2] = i + begin;
        face_iter++;
        gSP1Triangle(geo_iter++, 0, i - 1, i, 0);
    }
    used_vert = vertex_iter;
}

void custom_level_texture(char* texpath) {
    memcpy(textures[tex_iter], texpath, strlen(texpath) + 1);
	gDPSetTextureImage(geo_iter++, G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, textures[tex_iter]);
	gDPSetTile(geo_iter++, G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 0, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0);
	gDPLoadBlock(geo_iter++, 7, 0, 0, 1023, 256);
	gDPSetTile(geo_iter++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0);
	gDPSetTileSize(geo_iter++, 0, 0, 0, 124, 124);
    tex_iter++;
}

void custom_level_finish() {
    int iter = 0;
    custom_level_collision[iter++] = TERRAIN_LOAD_VERTICES;
    custom_level_collision[iter++] = vertex_iter;
    int lowest = 0x7FFF;
    for (int i = 0; i < vertex_iter; i++) {
        for (int j = 0; j < 3; j++) {
            if (lowest > vertices[i].v.ob[j]) lowest = vertices[i].v.ob[j];
            custom_level_collision[iter++] = vertices[i].v.ob[j];
        }
    }
    custom_level_collision[iter++] = 0;
    custom_level_collision[iter++] = face_iter;
    for (int i = 0; i < face_iter; i++) {
        for (int j = 0; j < 3; j++) {
            custom_level_collision[iter++] = faces[i * 3 + j];
        }
    }
    custom_level_collision[iter++] = TERRAIN_LOAD_CONTINUE;
    custom_level_collision[iter++] = TERRAIN_LOAD_END;
    gSPEndDisplayList(geo_iter++);
}

Gfx *geo_custom_level(s32 sp, struct GraphNode* graphNode, UNUSED void* context) {
    return custom_level_dl;
}