#include "saturn/saturn_timelines.h"

#define DEFAULT 0
#define FORCE_WAIT 1
#define EVENT 2

#define SATURN_KFENTRY_BOOL(id, variable, name) timelineDataTable.insert({ id, { &variable, KFTYPE_BOOL, KFBEH_FORCE_WAIT, name, 0, 1 } })
#define SATURN_KFENTRY_FLOAT(id, variable, name) timelineDataTable.insert({ id, { &variable, KFTYPE_FLOAT, KFBEH_DEFAULT, name, -2, 1 } })
#define SATURN_KFENTRY_ANIM(id, name) timelineDataTable.insert({ id, { &current_animation, KFTYPE_ANIM, KFBEH_EVENT, name, 0, 5 } })
#define SATURN_KFENTRY_EXPRESSION(id, name) timelineDataTable.insert({ id, { &current_model, KFTYPE_EXPRESSION, KFBEH_EVENT, name, 0, 1 } })
#define SATURN_KFENTRY_COLOR(id, variable, name) \
    timelineDataTable.insert({ id "_r", { &variable.x, KFTYPE_FLOAT, KFBEH_DEFAULT, name " R", 0, 1 } });\
    timelineDataTable.insert({ id "_g", { &variable.y, KFTYPE_FLOAT, KFBEH_DEFAULT, name " G", 0, 1 } });\
    timelineDataTable.insert({ id "_b", { &variable.z, KFTYPE_FLOAT, KFBEH_DEFAULT, name " B", 0, 1 } })
#define SATURN_KFENTRY_COLOR_VEC3F(id, variable, name) \
    timelineDataTable.insert({ id "_r", { &variable[0], KFTYPE_FLOAT, KFBEH_DEFAULT, name " R", 0, 1 } });\
    timelineDataTable.insert({ id "_g", { &variable[1], KFTYPE_FLOAT, KFBEH_DEFAULT, name " G", 0, 1 } });\
    timelineDataTable.insert({ id "_b", { &variable[2], KFTYPE_FLOAT, KFBEH_DEFAULT, name " B", 0, 1 } })

std::map<std::string, std::tuple<void*, KeyframeType, char, std::string, int, int>> timelineDataTable = {};

// { id, { variable_ptr, type, behavior, name, precision, num_values } }
void saturn_fill_data_table() {
    SATURN_KFENTRY_BOOL("k_skybox_mode", use_color_background, "Skybox Mode");
    SATURN_KFENTRY_BOOL("k_shadows", enable_shadows, "Shadows");
    SATURN_KFENTRY_FLOAT("k_shade_x", world_light_dir1, "Mario Shade X");
    SATURN_KFENTRY_FLOAT("k_shade_y", world_light_dir2, "Mario Shade Y");
    SATURN_KFENTRY_FLOAT("k_shade_z", world_light_dir3, "Mario Shade Z");
    SATURN_KFENTRY_FLOAT("k_shade_t", world_light_dir4, "Mario Shade Tex");
    SATURN_KFENTRY_FLOAT("k_scale", marioScaleSizeX, "Mario Scale");
    SATURN_KFENTRY_FLOAT("k_scale_x", marioScaleSizeX, "Mario Scale X");
    SATURN_KFENTRY_FLOAT("k_scale_y", marioScaleSizeY, "Mario Scale Y");
    SATURN_KFENTRY_FLOAT("k_scale_z", marioScaleSizeZ, "Mario Scale Z");
    SATURN_KFENTRY_BOOL("k_head_rot", enable_head_rotations, "Head Rotations");
    SATURN_KFENTRY_BOOL("k_v_cap_emblem", show_vmario_emblem, "M Cap Emblem");
    SATURN_KFENTRY_FLOAT("k_angle", gMarioState->faceAngle[1], "Mario Angle");
    SATURN_KFENTRY_BOOL("k_hud", configHUD, "HUD");
    SATURN_KFENTRY_BOOL("k_time_freeze", enable_time_freeze, "Time Freeze");
    SATURN_KFENTRY_FLOAT("k_fov", camera_fov, "FOV");
    SATURN_KFENTRY_FLOAT("k_focus", camera_focus, "Follow");
    SATURN_KFENTRY_FLOAT("k_c_camera_pos0", freezecamPos[0], "Camera Pos X");
    SATURN_KFENTRY_FLOAT("k_c_camera_pos1", freezecamPos[1], "Camera Pos Y");
    SATURN_KFENTRY_FLOAT("k_c_camera_pos2", freezecamPos[2], "Camera Pos Z");
    SATURN_KFENTRY_FLOAT("k_c_camera_yaw", freezecamYaw, "Camera Yaw");
    SATURN_KFENTRY_FLOAT("k_c_camera_pitch", freezecamPitch, "Camera Pitch");
    SATURN_KFENTRY_FLOAT("k_c_camera_roll", freezecamRoll, "Camera Roll");
    SATURN_KFENTRY_FLOAT("k_gravity", gravity, "Gravity");
    SATURN_KFENTRY_COLOR_VEC3F("k_light_col", gLightingColor, "Light Color");
    SATURN_KFENTRY_COLOR("k_color", uiChromaColor, "Skybox Color");
    SATURN_KFENTRY_COLOR("k_1/2###hat_half_1", uiHatColor, "Hat, Main");
    SATURN_KFENTRY_COLOR("k_1/2###hat_half_2", uiHatShadeColor, "Hat, Shade");
    SATURN_KFENTRY_COLOR("k_1/2###overalls_half_1", uiOverallsColor, "Overalls, Main");
    SATURN_KFENTRY_COLOR("k_1/2###overalls_half_2", uiOverallsShadeColor, "Overalls, Shade");
    SATURN_KFENTRY_COLOR("k_1/2###gloves_half_1", uiGlovesColor, "Gloves, Main");
    SATURN_KFENTRY_COLOR("k_1/2###gloves_half_2", uiGlovesShadeColor, "Gloves, Shade");
    SATURN_KFENTRY_COLOR("k_1/2###shoes_half_1", uiShoesColor, "Shoes, Main");
    SATURN_KFENTRY_COLOR("k_1/2###shoes_half_2", uiShoesShadeColor, "Shoes, Shade");
    SATURN_KFENTRY_COLOR("k_1/2###skin_half_1", uiSkinColor, "Skin, Main");
    SATURN_KFENTRY_COLOR("k_1/2###skin_half_2", uiSkinShadeColor, "Skin, Shade");
    SATURN_KFENTRY_COLOR("k_1/2###hair_half_1", uiHairColor, "Hair, Main");
    SATURN_KFENTRY_COLOR("k_1/2###hair_half_2", uiHairShadeColor, "Hair, Shade");
    SATURN_KFENTRY_COLOR("k_1/2###shirt_half_1", uiShirtColor, "Shirt, Main");
    SATURN_KFENTRY_COLOR("k_1/2###shirt_half_2", uiShirtShadeColor, "Shirt, Shade");
    SATURN_KFENTRY_COLOR("k_1/2###shoulders_half_1", uiShouldersColor, "Shoulders, Main");
    SATURN_KFENTRY_COLOR("k_1/2###shoulders_half_2", uiShouldersShadeColor, "Shoulders, Shade");
    SATURN_KFENTRY_COLOR("k_1/2###arms_half_1", uiArmsColor, "Arms, Main");
    SATURN_KFENTRY_COLOR("k_1/2###arms_half_2", uiArmsShadeColor, "Arms, Shade");
    SATURN_KFENTRY_COLOR("k_1/2###overalls_bottom_half_1", uiOverallsBottomColor, "Overalls (Bottom), Main");
    SATURN_KFENTRY_COLOR("k_1/2###overalls_bottom_half_2", uiOverallsBottomShadeColor, "Overalls (Bottom), Shade");
    SATURN_KFENTRY_COLOR("k_1/2###leg_top_half_1", uiLegTopColor, "Leg (Top), Main");
    SATURN_KFENTRY_COLOR("k_1/2###leg_top_half_2", uiLegTopShadeColor, "Leg (Top), Shade");
    SATURN_KFENTRY_COLOR("k_1/2###leg_bottom_half_1", uiLegBottomColor, "Leg (Bottom), Main");
    SATURN_KFENTRY_COLOR("k_1/2###leg_bottom_half_2", uiLegBottomShadeColor, "Leg (Bottom), Shade");
    SATURN_KFENTRY_ANIM("k_mario_anim", "Animation");
    SATURN_KFENTRY_EXPRESSION("k_mario_expr", "Expression");
}