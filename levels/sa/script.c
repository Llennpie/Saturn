#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/sa/header.h"

const LevelScript level_sa_entry[] = {
	INIT_LEVEL(),
	LOAD_MIO0(0x07, _sa_segment_7SegmentRomStart, _sa_segment_7SegmentRomEnd), 
	LOAD_MIO0_TEXTURE(0x09, _inside_mio0SegmentRomStart, _inside_mio0SegmentRomEnd), 
	LOAD_MIO0(0x0A, _water_skybox_mio0SegmentRomStart, _water_skybox_mio0SegmentRomEnd), 
	LOAD_MIO0(0x0B, _effect_mio0SegmentRomStart, _effect_mio0SegmentRomEnd), 
	LOAD_MIO0(0x05, _group4_mio0SegmentRomStart, _group4_mio0SegmentRomEnd), 
	LOAD_RAW(0x0C, _group4_geoSegmentRomStart, _group4_geoSegmentRomEnd), 
	LOAD_MIO0(0x06, _group13_mio0SegmentRomStart, _group13_mio0SegmentRomEnd), 
	LOAD_RAW(0x0D, _group13_geoSegmentRomStart, _group13_geoSegmentRomEnd), 
	ALLOC_LEVEL_POOL(),
	MARIO(MODEL_MARIO, 0x00000001, bhvMario), 
	JUMP_LINK(script_func_global_5), 
	JUMP_LINK(script_func_global_14), 

	AREA(1, sa_area_1),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF0, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF1, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		OBJECT(MODEL_NONE, 0, 200, 0, 0, 0, 0, 0x000A0000, bhvSpinAirborneWarp),
		MARIO_POS(0x01, 0, 0, 200, 0),
		TERRAIN(sa_area_1_collision),
		MACRO_OBJECTS(sa_area_1_macro_objs),
		STOP_MUSIC(0),
		TERRAIN_TYPE(TERRAIN_GRASS),
	END_AREA(),

	AREA(2, sa_area_2),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF0, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF1, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		OBJECT(MODEL_NONE, 0, 200, 0, 0, 0, 0, 0x000A0000, bhvSpinAirborneWarp),
		MARIO_POS(0x02, 0, 0, 0, 0),
		TERRAIN(sa_area_2_collision),
		MACRO_OBJECTS(sa_area_2_macro_objs),
		STOP_MUSIC(0),
		TERRAIN_TYPE(TERRAIN_GRASS),
	END_AREA(),

	AREA(3, sa_area_3),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF0, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF1, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		OBJECT(MODEL_NONE, 0, 200, 0, 0, 0, 0, 0x000A0000, bhvSpinAirborneWarp),
		MARIO_POS(0x03, 0, 0, 0, 0),
		TERRAIN(sa_area_3_collision),
		MACRO_OBJECTS(sa_area_3_macro_objs),
		STOP_MUSIC(0),
		TERRAIN_TYPE(TERRAIN_GRASS),
	END_AREA(),

	FREE_LEVEL_POOL(),
	MARIO_POS(0x03, 0, 0, 0, 0),
	CALL(0, lvl_init_or_update),
	CALL_LOOP(1, lvl_init_or_update),
	CLEAR_LEVEL(),
	SLEEP_BEFORE_EXIT(1),
	EXIT(),
};
