Lights1 sa_dl_sm64_material_lights = gdSPDefLights1(
	0x3F, 0x32, 0x19,
	0x0, 0x0, 0x0, 0x0, 0x7F, 0x0);

Vtx sa_dl_Plane_001_mesh_vtx_0[4] = {
	{{{-6400, 0, 6400},0, {8688, -7696},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{6400, 0, 6400},0, {-7696, -7696},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{6400, 0, -6400},0, {-7696, 8688},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-6400, 0, -6400},0, {8688, 8688},{0x0, 0x7F, 0x0, 0xFF}}},
};

Gfx sa_dl_Plane_001_mesh_tri_0[] = {
	gsSPVertex(sa_dl_Plane_001_mesh_vtx_0 + 0, 4, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 2, 3, 0),
	gsSPEndDisplayList(),
};

Gfx mat_sa_dl_sm64_material[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(sa_dl_sm64_material_lights),
	gsSPEndDisplayList(),
};

Gfx sa_dl_Plane_001_mesh[] = {
	gsSPDisplayList(mat_sa_dl_sm64_material),
	gsSPDisplayList(sa_dl_Plane_001_mesh_tri_0),
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

