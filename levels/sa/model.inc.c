Lights1 sa_dl_bsfloor_001_lights = gdSPDefLights1(
	0x0, 0x7F, 0x0,
	0x0, 0xFE, 0x0, 0x28, 0x28, 0x28);

Lights1 sa_dl_bsfloor_lights = gdSPDefLights1(
	0x0, 0x0, 0x7F,
	0x0, 0x0, 0xFE, 0x28, 0x28, 0x28);

Lights1 sa_dl_psfloor_lights = gdSPDefLights1(
	0x7F, 0x0, 0x7F,
	0xFE, 0x0, 0xFE, 0x28, 0x28, 0x28);

Vtx sa_dl_Plane_002_mesh_layer_1_vtx_0[4] = {
	{{{-5000, 0, 5000},0, {-16, 1008},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{5000, 0, 5000},0, {1008, 1008},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{5000, 0, -5000},0, {1008, -16},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-5000, 0, -5000},0, {-16, -16},{0x0, 0x7F, 0x0, 0xFF}}},
};

Gfx sa_dl_Plane_002_mesh_layer_1_tri_0[] = {
	gsSPVertex(sa_dl_Plane_002_mesh_layer_1_vtx_0 + 0, 4, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 2, 3, 0),
	gsSPEndDisplayList(),
};

Vtx sa_dl_Plane_001_mesh_layer_1_vtx_0[4] = {
	{{{-5000, 0, 5000},0, {-16, 1008},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{5000, 0, 5000},0, {1008, 1008},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{5000, 0, -5000},0, {1008, -16},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-5000, 0, -5000},0, {-16, -16},{0x0, 0x7F, 0x0, 0xFF}}},
};

Gfx sa_dl_Plane_001_mesh_layer_1_tri_0[] = {
	gsSPVertex(sa_dl_Plane_001_mesh_layer_1_vtx_0 + 0, 4, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 2, 3, 0),
	gsSPEndDisplayList(),
};

Vtx sa_dl_Plane_mesh_layer_1_vtx_0[4] = {
	{{{-5000, 0, 5000},0, {-16, 1008},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{5000, 0, 5000},0, {1008, 1008},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{5000, 0, -5000},0, {1008, -16},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-5000, 0, -5000},0, {-16, -16},{0x0, 0x7F, 0x0, 0xFF}}},
};

Gfx sa_dl_Plane_mesh_layer_1_tri_0[] = {
	gsSPVertex(sa_dl_Plane_mesh_layer_1_vtx_0 + 0, 4, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 2, 3, 0),
	gsSPEndDisplayList(),
};

Gfx mat_sa_dl_bsfloor_001[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(sa_dl_bsfloor_001_lights),
	gsSPEndDisplayList(),
};

Gfx mat_sa_dl_bsfloor[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(sa_dl_bsfloor_lights),
	gsSPEndDisplayList(),
};

Gfx mat_sa_dl_psfloor[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(sa_dl_psfloor_lights),
	gsSPEndDisplayList(),
};

Gfx sa_dl_Plane_002_mesh_layer_1[] = {
	gsSPDisplayList(mat_sa_dl_bsfloor_001),
	gsSPDisplayList(sa_dl_Plane_002_mesh_layer_1_tri_0),
	gsSPEndDisplayList(),
};

Gfx sa_dl_Plane_001_mesh_layer_1[] = {
	gsSPDisplayList(mat_sa_dl_bsfloor),
	gsSPDisplayList(sa_dl_Plane_001_mesh_layer_1_tri_0),
	gsSPEndDisplayList(),
};

Gfx sa_dl_Plane_mesh_layer_1[] = {
	gsSPDisplayList(mat_sa_dl_psfloor),
	gsSPDisplayList(sa_dl_Plane_mesh_layer_1_tri_0),
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

