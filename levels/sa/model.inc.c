Lights1 sa_dl_green_lights = gdSPDefLights1(
	0x0, 0x7F, 0x0,
	0x0, 0xFE, 0x0, 0x28, 0x28, 0x28);

Lights1 sa_dl_blue_lights = gdSPDefLights1(
	0x0, 0x0, 0x7F,
	0x0, 0x0, 0xFE, 0x28, 0x28, 0x28);

Lights1 sa_dl_pink_lights = gdSPDefLights1(
	0x7F, 0x0, 0x7F,
	0xFE, 0x0, 0xFE, 0x28, 0x28, 0x28);

Vtx sa_dl_Cube_mesh_vtx_0[24] = {
	{{{7000, 100, -7000},0, {624, 496},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-7000, 100, -7000},0, {880, 496},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-7000, 100, 7000},0, {880, 240},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{7000, 100, 7000},0, {624, 240},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{7000, -100, 7000},0, {368, 240},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{7000, 100, 7000},0, {624, 240},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-7000, 100, 7000},0, {624, -16},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-7000, -100, 7000},0, {368, -16},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-7000, -100, 7000},0, {368, 1008},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-7000, 100, 7000},0, {624, 1008},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-7000, 100, -7000},0, {624, 752},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-7000, -100, -7000},0, {368, 752},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-7000, -100, -7000},0, {112, 496},{0x0, 0x81, 0x0, 0xFF}}},
	{{{7000, -100, -7000},0, {368, 496},{0x0, 0x81, 0x0, 0xFF}}},
	{{{7000, -100, 7000},0, {368, 240},{0x0, 0x81, 0x0, 0xFF}}},
	{{{-7000, -100, 7000},0, {112, 240},{0x0, 0x81, 0x0, 0xFF}}},
	{{{7000, -100, -7000},0, {368, 496},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{7000, 100, -7000},0, {624, 496},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{7000, 100, 7000},0, {624, 240},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{7000, -100, 7000},0, {368, 240},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{-7000, -100, -7000},0, {368, 752},{0x0, 0x0, 0x81, 0xFF}}},
	{{{-7000, 100, -7000},0, {624, 752},{0x0, 0x0, 0x81, 0xFF}}},
	{{{7000, 100, -7000},0, {624, 496},{0x0, 0x0, 0x81, 0xFF}}},
	{{{7000, -100, -7000},0, {368, 496},{0x0, 0x0, 0x81, 0xFF}}},
};

Gfx sa_dl_Cube_mesh_tri_0[] = {
	gsSPVertex(sa_dl_Cube_mesh_vtx_0 + 0, 16, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 2, 3, 0),
	gsSP1Triangle(4, 5, 6, 0),
	gsSP1Triangle(4, 6, 7, 0),
	gsSP1Triangle(8, 9, 10, 0),
	gsSP1Triangle(8, 10, 11, 0),
	gsSP1Triangle(12, 13, 14, 0),
	gsSP1Triangle(12, 14, 15, 0),
	gsSPVertex(sa_dl_Cube_mesh_vtx_0 + 16, 8, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 2, 3, 0),
	gsSP1Triangle(4, 5, 6, 0),
	gsSP1Triangle(4, 6, 7, 0),
	gsSPEndDisplayList(),
};

Vtx sa_dl_Cube2_mesh_vtx_0[24] = {
	{{{7000, 100, -7000},0, {624, 496},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-7000, 100, -7000},0, {880, 496},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-7000, 100, 7000},0, {880, 240},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{7000, 100, 7000},0, {624, 240},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{7000, -100, 7000},0, {368, 240},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{7000, 100, 7000},0, {624, 240},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-7000, 100, 7000},0, {624, -16},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-7000, -100, 7000},0, {368, -16},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-7000, -100, 7000},0, {368, 1008},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-7000, 100, 7000},0, {624, 1008},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-7000, 100, -7000},0, {624, 752},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-7000, -100, -7000},0, {368, 752},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-7000, -100, -7000},0, {112, 496},{0x0, 0x81, 0x0, 0xFF}}},
	{{{7000, -100, -7000},0, {368, 496},{0x0, 0x81, 0x0, 0xFF}}},
	{{{7000, -100, 7000},0, {368, 240},{0x0, 0x81, 0x0, 0xFF}}},
	{{{-7000, -100, 7000},0, {112, 240},{0x0, 0x81, 0x0, 0xFF}}},
	{{{7000, -100, -7000},0, {368, 496},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{7000, 100, -7000},0, {624, 496},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{7000, 100, 7000},0, {624, 240},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{7000, -100, 7000},0, {368, 240},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{-7000, -100, -7000},0, {368, 752},{0x0, 0x0, 0x81, 0xFF}}},
	{{{-7000, 100, -7000},0, {624, 752},{0x0, 0x0, 0x81, 0xFF}}},
	{{{7000, 100, -7000},0, {624, 496},{0x0, 0x0, 0x81, 0xFF}}},
	{{{7000, -100, -7000},0, {368, 496},{0x0, 0x0, 0x81, 0xFF}}},
};

Gfx sa_dl_Cube2_mesh_tri_0[] = {
	gsSPVertex(sa_dl_Cube2_mesh_vtx_0 + 0, 16, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 2, 3, 0),
	gsSP1Triangle(4, 5, 6, 0),
	gsSP1Triangle(4, 6, 7, 0),
	gsSP1Triangle(8, 9, 10, 0),
	gsSP1Triangle(8, 10, 11, 0),
	gsSP1Triangle(12, 13, 14, 0),
	gsSP1Triangle(12, 14, 15, 0),
	gsSPVertex(sa_dl_Cube2_mesh_vtx_0 + 16, 8, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 2, 3, 0),
	gsSP1Triangle(4, 5, 6, 0),
	gsSP1Triangle(4, 6, 7, 0),
	gsSPEndDisplayList(),
};

Vtx sa_dl_Cube3_mesh_vtx_0[24] = {
	{{{7000, 100, -7000},0, {624, 496},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-7000, 100, -7000},0, {880, 496},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-7000, 100, 7000},0, {880, 240},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{7000, 100, 7000},0, {624, 240},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{7000, -100, 7000},0, {368, 240},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{7000, 100, 7000},0, {624, 240},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-7000, 100, 7000},0, {624, -16},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-7000, -100, 7000},0, {368, -16},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-7000, -100, 7000},0, {368, 1008},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-7000, 100, 7000},0, {624, 1008},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-7000, 100, -7000},0, {624, 752},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-7000, -100, -7000},0, {368, 752},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-7000, -100, -7000},0, {112, 496},{0x0, 0x81, 0x0, 0xFF}}},
	{{{7000, -100, -7000},0, {368, 496},{0x0, 0x81, 0x0, 0xFF}}},
	{{{7000, -100, 7000},0, {368, 240},{0x0, 0x81, 0x0, 0xFF}}},
	{{{-7000, -100, 7000},0, {112, 240},{0x0, 0x81, 0x0, 0xFF}}},
	{{{7000, -100, -7000},0, {368, 496},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{7000, 100, -7000},0, {624, 496},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{7000, 100, 7000},0, {624, 240},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{7000, -100, 7000},0, {368, 240},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{-7000, -100, -7000},0, {368, 752},{0x0, 0x0, 0x81, 0xFF}}},
	{{{-7000, 100, -7000},0, {624, 752},{0x0, 0x0, 0x81, 0xFF}}},
	{{{7000, 100, -7000},0, {624, 496},{0x0, 0x0, 0x81, 0xFF}}},
	{{{7000, -100, -7000},0, {368, 496},{0x0, 0x0, 0x81, 0xFF}}},
};

Gfx sa_dl_Cube3_mesh_tri_0[] = {
	gsSPVertex(sa_dl_Cube3_mesh_vtx_0 + 0, 16, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 2, 3, 0),
	gsSP1Triangle(4, 5, 6, 0),
	gsSP1Triangle(4, 6, 7, 0),
	gsSP1Triangle(8, 9, 10, 0),
	gsSP1Triangle(8, 10, 11, 0),
	gsSP1Triangle(12, 13, 14, 0),
	gsSP1Triangle(12, 14, 15, 0),
	gsSPVertex(sa_dl_Cube3_mesh_vtx_0 + 16, 8, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 2, 3, 0),
	gsSP1Triangle(4, 5, 6, 0),
	gsSP1Triangle(4, 6, 7, 0),
	gsSPEndDisplayList(),
};

Gfx mat_sa_dl_green[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(sa_dl_green_lights),
	gsSPEndDisplayList(),
};

Gfx mat_sa_dl_blue[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(sa_dl_blue_lights),
	gsSPEndDisplayList(),
};

Gfx mat_sa_dl_pink[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(sa_dl_pink_lights),
	gsSPEndDisplayList(),
};

Gfx sa_dl_Cube_mesh[] = {
	gsSPDisplayList(mat_sa_dl_green),
	gsSPDisplayList(sa_dl_Cube_mesh_tri_0),
	gsSPEndDisplayList(),
};

Gfx sa_dl_Cube2_mesh[] = {
	gsSPDisplayList(mat_sa_dl_blue),
	gsSPDisplayList(sa_dl_Cube2_mesh_tri_0),
	gsSPEndDisplayList(),
};

Gfx sa_dl_Cube3_mesh[] = {
	gsSPDisplayList(mat_sa_dl_pink),
	gsSPDisplayList(sa_dl_Cube3_mesh_tri_0),
	gsSPEndDisplayList(),
};

Gfx sa_dl_material_revert_render_settings[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};

