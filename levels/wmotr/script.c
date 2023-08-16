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

/* Fast64 begin persistent block [includes] */
/* Fast64 end persistent block [includes] */

#include "make_const_nonconst.h"
#include "levels/wmotr/header.h"

/* Fast64 begin persistent block [scripts] */
/* Fast64 end persistent block [scripts] */

const LevelScript level_wmotr_entry[] = {
	INIT_LEVEL(),
	LOAD_MIO0(0x07, _wmotr_segment_7SegmentRomStart, _wmotr_segment_7SegmentRomEnd), 
	LOAD_MIO0(0x0A, _water_skybox_mio0SegmentRomStart, _water_skybox_mio0SegmentRomEnd), 
	LOAD_MIO0_TEXTURE(0x09, _sky_mio0SegmentRomStart, _sky_mio0SegmentRomEnd), 
	LOAD_MIO0(0x05, _group2_mio0SegmentRomStart, _group2_mio0SegmentRomEnd), 
	LOAD_RAW(0x0C, _group2_geoSegmentRomStart, _group2_geoSegmentRomEnd), 
	LOAD_MIO0(0x06, _group17_mio0SegmentRomStart, _group17_mio0SegmentRomEnd), 
	LOAD_RAW(0x0D, _group17_geoSegmentRomStart, _group17_geoSegmentRomEnd), 
	LOAD_MIO0(0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd), 
	LOAD_RAW(0x0F, _common0_geoSegmentRomStart, _common0_geoSegmentRomEnd), 
	ALLOC_LEVEL_POOL(),
	MARIO(MODEL_MARIO, 0x00000001, bhvMario), 
	JUMP_LINK(script_func_global_1), 
	JUMP_LINK(script_func_global_3), 
	JUMP_LINK(script_func_global_18), 
	LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CASTLE_DOOR, castle_door_geo), 
	LOAD_MODEL_FROM_GEO(MODEL_CASTLE_WOODEN_DOOR, wooden_door_geo), 
	LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CLOCK_MINUTE_HAND, castle_geo_001530), 
	LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CLOCK_HOUR_HAND, castle_geo_001548), 
	LOAD_MODEL_FROM_GEO(MODEL_MAD_PIANO, mad_piano_geo), 

	/* Fast64 begin persistent block [level commands] */
	/* Fast64 end persistent block [level commands] */

	AREA(1, wmotr_area_1),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF0, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF1, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		OBJECT(MODEL_1UP, 1330, -862, -4526, 0, 0, 0, 0x00000000, bhv1Up),
		OBJECT(MODEL_CASTLE_CASTLE_DOOR, 2074, -473, 171, 0, -90, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_CASTLE_CASTLE_DOOR, -2046, -472, 168, 0, 90, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_CASTLE_CASTLE_DOOR, -1240, -673, -669, 0, 0, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_RED_FLAME, -1231, -1034, -6135, 0, 0, 0, 0x00000000, bhvFlame),
		OBJECT(MODEL_CASTLE_WOODEN_DOOR, 18, -773, -7004, 0, 0, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_CASTLE_CLOCK_HOUR_HAND, 11, 488, -1110, 0, 0, 0, 0x00000000, bhvClockHourHand),
		OBJECT(MODEL_CASTLE_CASTLE_DOOR, 1775, -171, -1281, 0, 0, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_CASTLE_CASTLE_DOOR, -1736, -171, -1257, 0, 180, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_MAD_PIANO, -2983, -171, -4236, 0, 35, 0, 0x00000000, bhvMadPiano),
		OBJECT(MODEL_CASTLE_CASTLE_DOOR, -60, -674, 2541, 0, 0, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_CASTLE_CASTLE_DOOR, 97, -674, 2541, 0, 180, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_CASTLE_CLOCK_MINUTE_HAND, 16, 487, -1122, 0, 0, 0, 0x00000000, bhvClockMinuteHand),
		OBJECT(MODEL_CASTLE_CASTLE_DOOR, -62, -171, -1270, 0, 0, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_CASTLE_CASTLE_DOOR, 95, -171, -1270, 0, 180, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_CASTLE_WOODEN_DOOR, 17, -673, -284, 0, 0, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_NONE, 0, 200, 0, 0, 0, 0, 0x000A0000, bhvSpinAirborneWarp),
		MARIO_POS(0x01, 0, 0, 200, 0),
		OBJECT(MODEL_CASTLE_CASTLE_DOOR, 1274, -673, -673, 0, 0, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_RED_FLAME, 1875, -943, -4637, 0, 0, 0, 0x00000000, bhvFlame),
		OBJECT(MODEL_CASTLE_WOODEN_DOOR, -726, -673, -416, 0, -45, 0, 0x00000000, bhvDoor),
		OBJECT(MODEL_CASTLE_WOODEN_DOOR, 750, -673, -419, 0, 45, 0, 0x00000000, bhvDoor),
		TERRAIN(wmotr_area_1_collision),
		MACRO_OBJECTS(wmotr_area_1_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_INSIDE_CASTLE),
		TERRAIN_TYPE(TERRAIN_STONE),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	FREE_LEVEL_POOL(),
	MARIO_POS(0x01, 0, 0, 200, 0),
	CALL(0, lvl_init_or_update),
	CALL_LOOP(1, lvl_init_or_update),
	CLEAR_LEVEL(),
	SLEEP_BEFORE_EXIT(1),
	EXIT(),
};
