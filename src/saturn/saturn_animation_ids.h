#ifndef SaturnAnimationIds
#define SaturnAnimationIds

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
#include <string>
#include <vector>
#include <map>

#include <mario_animation_ids.h>

extern std::map<std::pair<int, std::string>, int> sanim_movement = {
    // 0: Movement (50)
    { std::make_pair(0, "RUNNING"), MARIO_ANIM_RUNNING},
    { std::make_pair(1, "RUNNING_UNUSED"), MARIO_ANIM_RUNNING_UNUSED}, // unused duplicate, originally part 2?
    { std::make_pair(2, "WALKING"), MARIO_ANIM_WALKING},
    { std::make_pair(3, "TIPTOE"), MARIO_ANIM_TIPTOE},
    { std::make_pair(4, "START_TIPTOE"), MARIO_ANIM_START_TIPTOE},
    { std::make_pair(5, "SINGLE_JUMP"), MARIO_ANIM_SINGLE_JUMP},
    { std::make_pair(6, "LAND_FROM_SINGLE_JUMP"), MARIO_ANIM_LAND_FROM_SINGLE_JUMP},
    { std::make_pair(7, "DOUBLE_JUMP_RISE"), MARIO_ANIM_DOUBLE_JUMP_RISE},
    { std::make_pair(8, "DOUBLE_JUMP_FALL"), MARIO_ANIM_DOUBLE_JUMP_FALL},
    { std::make_pair(9, "LAND_FROM_DOUBLE_JUMP"), MARIO_ANIM_LAND_FROM_DOUBLE_JUMP},
    { std::make_pair(10, "TRIPLE_JUMP"), MARIO_ANIM_TRIPLE_JUMP},
    { std::make_pair(11, "TRIPLE_JUMP_LAND"), MARIO_ANIM_TRIPLE_JUMP_LAND},
    { std::make_pair(12, "TRIPLE_JUMP_FLY"), MARIO_ANIM_TRIPLE_JUMP_FLY},
    { std::make_pair(13, "TRIPLE_JUMP_GROUND_POUND"), MARIO_ANIM_TRIPLE_JUMP_GROUND_POUND},
    { std::make_pair(14, "BACKFLIP"), MARIO_ANIM_BACKFLIP},
    { std::make_pair(15, "FORWARD_FLIP"), MARIO_ANIM_FORWARD_FLIP}, // unused
    { std::make_pair(16, "FORWARD_SPINNING"), MARIO_ANIM_FORWARD_SPINNING},
    { std::make_pair(17, "START_FORWARD_SPINNING"), MARIO_ANIM_START_FORWARD_SPINNING}, // unused
    { std::make_pair(18, "FORWARD_SPINNING_FLIP"), MARIO_ANIM_FORWARD_SPINNING_FLIP},
    { std::make_pair(19, "BACKWARD_SPINNING"), MARIO_ANIM_BACKWARD_SPINNING},
    { std::make_pair(20, "SLIDEFLIP"), MARIO_ANIM_SLIDEFLIP},
    { std::make_pair(21, "SLIDEFLIP_LAND"), MARIO_ANIM_SLIDEFLIP_LAND},
    { std::make_pair(22, "CRAWLING"), MARIO_ANIM_CRAWLING},
    { std::make_pair(23, "START_CRAWLING"), MARIO_ANIM_START_CRAWLING},
    { std::make_pair(24, "STOP_CRAWLING"), MARIO_ANIM_STOP_CRAWLING},
    { std::make_pair(25, "FAST_LONGJUMP"), MARIO_ANIM_FAST_LONGJUMP},
    { std::make_pair(26, "SLOW_LONGJUMP"), MARIO_ANIM_SLOW_LONGJUMP},
    { std::make_pair(27, "CROUCH_FROM_FAST_LONGJUMP"), MARIO_ANIM_CROUCH_FROM_FAST_LONGJUMP},
    { std::make_pair(28, "CROUCH_FROM_SLOW_LONGJUMP"), MARIO_ANIM_CROUCH_FROM_SLOW_LONGJUMP},
    { std::make_pair(29, "SKID_ON_GROUND"), MARIO_ANIM_SKID_ON_GROUND},
    { std::make_pair(30, "STOP_SKID"), MARIO_ANIM_STOP_SKID},
    { std::make_pair(31, "TURNING_PART1"), MARIO_ANIM_TURNING_PART1},
    { std::make_pair(32, "TURNING_PART2"), MARIO_ANIM_TURNING_PART2},
    { std::make_pair(33, "SIDESTEP_LEFT"), MARIO_ANIM_SIDESTEP_LEFT},
    { std::make_pair(34, "SIDESTEP_RIGHT"), MARIO_ANIM_SIDESTEP_RIGHT},
    { std::make_pair(35, "START_WALLKICK"), MARIO_ANIM_START_WALLKICK},
    { std::make_pair(36, "RIDING_SHELL"), MARIO_ANIM_RIDING_SHELL},
    { std::make_pair(37, "START_RIDING_SHELL"), MARIO_ANIM_START_RIDING_SHELL},
    { std::make_pair(38, "BEND_KNESS_RIDING_SHELL"), MARIO_ANIM_BEND_KNESS_RIDING_SHELL}, // unused
    { std::make_pair(39, "JUMP_RIDING_SHELL"), MARIO_ANIM_JUMP_RIDING_SHELL},
    { std::make_pair(40, "WING_CAP_FLY"), MARIO_ANIM_WING_CAP_FLY},
    { std::make_pair(41, "FLY_FROM_CANNON"), MARIO_ANIM_FLY_FROM_CANNON},
    { std::make_pair(42, "SLIDE"), MARIO_ANIM_SLIDE},
    { std::make_pair(43, "SLIDE_MOTIONLESS"), MARIO_ANIM_SLIDE_MOTIONLESS}, // unused
    { std::make_pair(44, "STOP_SLIDE"), MARIO_ANIM_STOP_SLIDE},
    { std::make_pair(45, "FALL_FROM_SLIDE"), MARIO_ANIM_FALL_FROM_SLIDE},
    { std::make_pair(46, "MOVE_IN_QUICKSAND"), MARIO_ANIM_MOVE_IN_QUICKSAND},
    { std::make_pair(47, "TWIRL"), MARIO_ANIM_TWIRL},
    { std::make_pair(48, "START_TWIRL"), MARIO_ANIM_START_TWIRL},
    { std::make_pair(49, "TWIRL_LAND"), MARIO_ANIM_TWIRL_LAND},
};

extern std::map<std::pair<int, std::string>, int> sanim_actions = {
    // 1: Actions (25)
    { std::make_pair(0, "FIRST_PUNCH"), MARIO_ANIM_FIRST_PUNCH},
    { std::make_pair(1, "SECOND_PUNCH"), MARIO_ANIM_SECOND_PUNCH},
    { std::make_pair(2, "FIRST_PUNCH_FAST"), MARIO_ANIM_FIRST_PUNCH_FAST},
    { std::make_pair(3, "SECOND_PUNCH_FAST"), MARIO_ANIM_SECOND_PUNCH_FAST},
    { std::make_pair(4, "CROUCHING"), MARIO_ANIM_CROUCHING},
    { std::make_pair(5, "START_CROUCHING"), MARIO_ANIM_START_CROUCHING},
    { std::make_pair(6, "STOP_CROUCHING"), MARIO_ANIM_STOP_CROUCHING},
    { std::make_pair(7, "BREAKDANCE"), MARIO_ANIM_BREAKDANCE},
    { std::make_pair(8, "AIR_KICK"), MARIO_ANIM_AIR_KICK},
    { std::make_pair(9, "GROUND_KICK"), MARIO_ANIM_GROUND_KICK},
    { std::make_pair(10, "SLIDE_KICK"), MARIO_ANIM_SLIDE_KICK},
    { std::make_pair(11, "CROUCH_FROM_SLIDE_KICK"), MARIO_ANIM_CROUCH_FROM_SLIDE_KICK},
    { std::make_pair(12, "FALL_FROM_SLIDE_KICK"), MARIO_ANIM_FALL_FROM_SLIDE_KICK},
    { std::make_pair(13, "DIVE"), MARIO_ANIM_DIVE},
    { std::make_pair(14, "SLIDE_DIVE"), MARIO_ANIM_SLIDE_DIVE},
    { std::make_pair(15, "SLOW_LAND_FROM_DIVE"), MARIO_ANIM_SLOW_LAND_FROM_DIVE},
    { std::make_pair(16, "GROUND_POUND"), MARIO_ANIM_GROUND_POUND},
    { std::make_pair(17, "START_GROUND_POUND"), MARIO_ANIM_START_GROUND_POUND},
    { std::make_pair(18, "GROUND_POUND_LANDING"), MARIO_ANIM_GROUND_POUND_LANDING},
    { std::make_pair(19, "PUSHING"), MARIO_ANIM_PUSHING},
    { std::make_pair(20, "STAND_AGAINST_WALL"), MARIO_ANIM_STAND_AGAINST_WALL},
    { std::make_pair(21, "START_REACH_POCKET"), MARIO_ANIM_START_REACH_POCKET}, // unused, reaching keys maybe?
    { std::make_pair(22, "REACH_POCKET"), MARIO_ANIM_REACH_POCKET}, // unused
    { std::make_pair(23, "STOP_REACH_POCKET"), MARIO_ANIM_STOP_REACH_POCKET}, // unused
    { std::make_pair(24, "A_POSE"), MARIO_ANIM_A_POSE},
};

extern std::map<std::pair<int, std::string>, int> sanim_automatic = {
    // 2 : Automatic (27)
    { std::make_pair(0, "IDLE_HEAD_LEFT"), MARIO_ANIM_IDLE_HEAD_LEFT},
    { std::make_pair(1, "IDLE_HEAD_RIGHT"), MARIO_ANIM_IDLE_HEAD_CENTER},
    { std::make_pair(2, "IDLE_HEAD_CENTER"), MARIO_ANIM_IDLE_HEAD_CENTER},
    { std::make_pair(3, "FIRST_PERSON"), MARIO_ANIM_FIRST_PERSON},
    { std::make_pair(4, "SLEEP_IDLE"), MARIO_ANIM_SLEEP_IDLE},
    { std::make_pair(5, "SLEEP_START_LYING"), MARIO_ANIM_SLEEP_START_LYING},
    { std::make_pair(6, "SLEEP_LYING"), MARIO_ANIM_SLEEP_LYING},
    { std::make_pair(7, "START_SLEEP_IDLE"), MARIO_ANIM_START_SLEEP_IDLE},
    { std::make_pair(8, "START_SLEEP_SCRATCH"), MARIO_ANIM_START_SLEEP_SCRATCH},
    { std::make_pair(9, "START_SLEEP_YAWN"), MARIO_ANIM_START_SLEEP_YAWN},
    { std::make_pair(10, "START_SLEEP_SITTING"), MARIO_ANIM_START_SLEEP_SITTING},
    { std::make_pair(11, "WAKE_FROM_SLEEP"), MARIO_ANIM_WAKE_FROM_SLEEP},
    { std::make_pair(12, "WAKE_FROM_LYING"), MARIO_ANIM_WAKE_FROM_LYING},
    { std::make_pair(13, "HEAD_STUCK_IN_GROUND"), MARIO_ANIM_HEAD_STUCK_IN_GROUND},
    { std::make_pair(14, "BOTTOM_STUCK_IN_GROUND"), MARIO_ANIM_BOTTOM_STUCK_IN_GROUND},
    { std::make_pair(15, "LEGS_STUCK_IN_GROUND"), MARIO_ANIM_LEGS_STUCK_IN_GROUND},
    { std::make_pair(16, "SHIVERING"), MARIO_ANIM_SHIVERING},
    { std::make_pair(17, "SHIVERING_WARMING_HAND"), MARIO_ANIM_SHIVERING_WARMING_HAND},
    { std::make_pair(18, "SHIVERING_RETURN_TO_IDLE"), MARIO_ANIM_SHIVERING},
    { std::make_pair(19, "COUGHING"), MARIO_ANIM_COUGHING},
    { std::make_pair(20, "WALK_PANTING"), MARIO_ANIM_WALK_PANTING},
    { std::make_pair(21, "IDLE_IN_QUICKSAND"), MARIO_ANIM_IDLE_IN_QUICKSAND},
    { std::make_pair(22, "GENERAL_FALL"), MARIO_ANIM_GENERAL_FALL},
    { std::make_pair(23, "GENERAL_LAND"), MARIO_ANIM_GENERAL_LAND},
    { std::make_pair(24, "AIRBORNE_ON_STOMACH"), MARIO_ANIM_AIRBORNE_ON_STOMACH},
    { std::make_pair(25, "LAND_ON_STOMACH"), MARIO_ANIM_LAND_ON_STOMACH},
    { std::make_pair(26, "BEING_GRABBED"), MARIO_ANIM_BEING_GRABBED},
};

extern std::map<std::pair<int, std::string>, int> sanim_damagedeaths = {
    // 3 : Damage/Deaths (22)
    { std::make_pair(0, "SOFT_BACK_KB"), MARIO_ANIM_SOFT_BACK_KB},
    { std::make_pair(1, "SOFT_FRONT_KB"), MARIO_ANIM_SOFT_FRONT_KB},
    { std::make_pair(2, "BACKWARD_KB"), MARIO_ANIM_BACKWARD_KB},
    { std::make_pair(3, "FORWARD_KB"), MARIO_ANIM_FORWARD_KB},
    { std::make_pair(4, "BACKWARD_AIR_KB"), MARIO_ANIM_BACKWARD_AIR_KB},
    { std::make_pair(5, "AIR_FORWARD_KB"), MARIO_ANIM_AIR_FORWARD_KB},
    { std::make_pair(6, "BACKWARDS_WATER_KB"), MARIO_ANIM_BACKWARDS_WATER_KB},
    { std::make_pair(7, "WATER_FORWARD_KB"), MARIO_ANIM_WATER_FORWARD_KB},
    { std::make_pair(8, "GROUND_BONK"), MARIO_ANIM_GROUND_BONK},
    { std::make_pair(9, "DYING_ON_BACK"), MARIO_ANIM_DYING_ON_BACK},
    { std::make_pair(10, "DYING_ON_STOMACH"), MARIO_ANIM_DYING_ON_STOMACH},
    { std::make_pair(11, "DYING_FALL_OVER"), MARIO_ANIM_DYING_FALL_OVER},
    { std::make_pair(12, "DYING_IN_QUICKSAND"), MARIO_ANIM_DYING_IN_QUICKSAND},
    { std::make_pair(13, "FALL_OVER_BACKWARDS"), MARIO_ANIM_FALL_OVER_BACKWARDS},
    { std::make_pair(14, "ELECTROCUTION"), MARIO_ANIM_ELECTROCUTION},
    { std::make_pair(15, "SHOCKED"), MARIO_ANIM_SHOCKED},
    { std::make_pair(16, "SUFFOCATING"), MARIO_ANIM_SUFFOCATING},
    { std::make_pair(17, "FIRE_LAVA_BURN"), MARIO_ANIM_FIRE_LAVA_BURN},
    { std::make_pair(18, "STAND_UP_FROM_LAVA_BOOST"), MARIO_ANIM_STAND_UP_FROM_LAVA_BOOST},
    { std::make_pair(19, "DROWNING_PART1"), MARIO_ANIM_DROWNING_PART1},
    { std::make_pair(20, "DROWNING_PART2"), MARIO_ANIM_DROWNING_PART2},
    { std::make_pair(21, "WATER_DYING"), MARIO_ANIM_WATER_DYING},
};

extern std::map<std::pair<int, std::string>, int> sanim_cutscenes = {
    // 4 : Cutscenes (23)
    { std::make_pair(0, "STAR_DANCE"), MARIO_ANIM_STAR_DANCE},
    { std::make_pair(1, "RETURN_FROM_STAR_DANCE"), MARIO_ANIM_RETURN_FROM_STAR_DANCE},
    { std::make_pair(2, "MISSING_CAP"), MARIO_ANIM_MISSING_CAP},
    { std::make_pair(3, "THROW_CATCH_KEY"), MARIO_ANIM_THROW_CATCH_KEY},
    { std::make_pair(4, "UNLOCK_DOOR"), MARIO_ANIM_UNLOCK_DOOR},
    { std::make_pair(5, "SUMMON_STAR"), MARIO_ANIM_SUMMON_STAR},
    { std::make_pair(6, "RETURN_STAR_APPROACH_DOOR"), MARIO_ANIM_RETURN_STAR_APPROACH_DOOR},
    { std::make_pair(7, "PULL_DOOR_WALK_IN"), MARIO_ANIM_PULL_DOOR_WALK_IN},
    { std::make_pair(8, "PUSH_DOOR_WALK_IN"), MARIO_ANIM_PUSH_DOOR_WALK_IN},
    { std::make_pair(9, "CREDITS_WAVING"), MARIO_ANIM_CREDITS_WAVING},
    { std::make_pair(10, "CREDITS_PEACE_SIGN"), MARIO_ANIM_CREDITS_PEACE_SIGN},
    { std::make_pair(11, "CREDITS_LOOK_UP"), MARIO_ANIM_CREDITS_LOOK_UP},
    { std::make_pair(12, "CREDITS_RETURN_FROM_LOOK_UP"), MARIO_ANIM_CREDITS_RETURN_FROM_LOOK_UP},
    { std::make_pair(13, "CREDITS_RAISE_HAND"), MARIO_ANIM_CREDITS_RAISE_HAND},
    { std::make_pair(14, "CREDITS_LOWER_HAND"), MARIO_ANIM_CREDITS_LOWER_HAND},
    { std::make_pair(15, "CREDITS_TAKE_OFF_CAP"), MARIO_ANIM_CREDITS_TAKE_OFF_CAP},
    { std::make_pair(16, "CREDITS_START_WALK_LOOK_UP"), MARIO_ANIM_CREDITS_START_WALK_LOOK_UP},
    { std::make_pair(17, "CREDITS_LOOK_BACK_THEN_RUN"), MARIO_ANIM_CREDITS_LOOK_BACK_THEN_RUN},
    { std::make_pair(18, "FINAL_BOWSER_RAISE_HAND_SPIN"), MARIO_ANIM_FINAL_BOWSER_RAISE_HAND_SPIN},
    { std::make_pair(19, "FINAL_BOWSER_WING_CAP_TAKE_OFF"), MARIO_ANIM_FINAL_BOWSER_WING_CAP_TAKE_OFF},
    { std::make_pair(20, "PUT_CAP_ON"), MARIO_ANIM_PUT_CAP_ON},
    { std::make_pair(21, "TAKE_CAP_OFF_THEN_ON"), MARIO_ANIM_TAKE_CAP_OFF_THEN_ON},
    { std::make_pair(22, "QUICKLY_PUT_CAP_ON"), MARIO_ANIM_PUT_CAP_ON}, // unused
};

extern std::map<std::pair<int, std::string>, int> sanim_water = {
    // 5 : Water (16)
    { std::make_pair(0, "WATER_IDLE"), MARIO_ANIM_WATER_IDLE},
    { std::make_pair(1, "SWIM_PART1"), MARIO_ANIM_SWIM_PART1},
    { std::make_pair(2, "SWIM_PART2"), MARIO_ANIM_SWIM_PART2},
    { std::make_pair(3, "FLUTTERKICK"), MARIO_ANIM_FLUTTERKICK},
    { std::make_pair(4, "WATER_ACTION_END"), MARIO_ANIM_WATER_ACTION_END}, // either swimming or flutterkicking
    { std::make_pair(5, "FALL_FROM_WATER"), MARIO_ANIM_FALL_FROM_WATER},
    { std::make_pair(6, "WATER_IDLE_WITH_OBJ"), MARIO_ANIM_WATER_IDLE_WITH_OBJ},
    { std::make_pair(7, "WATER_PICK_UP_OBJ"), MARIO_ANIM_WATER_PICK_UP_OBJ},
    { std::make_pair(8, "WATER_GRAB_OBJ_PART1"), MARIO_ANIM_WATER_GRAB_OBJ_PART1},
    { std::make_pair(9, "WATER_GRAB_OBJ_PART2"), MARIO_ANIM_WATER_GRAB_OBJ_PART2},
    { std::make_pair(10, "STOP_GRAB_OBJ_WATER"), MARIO_ANIM_STOP_GRAB_OBJ_WATER},
    { std::make_pair(11, "WATER_THROW_OBJ"), MARIO_ANIM_WATER_THROW_OBJ},
    { std::make_pair(12, "SWIM_WITH_OBJ_PART1"), MARIO_ANIM_SWIM_WITH_OBJ_PART1},
    { std::make_pair(13, "SWIM_WITH_OBJ_PART2"), MARIO_ANIM_SWIM_WITH_OBJ_PART2},
    { std::make_pair(14, "FLUTTERKICK_WITH_OBJ"), MARIO_ANIM_FLUTTERKICK_WITH_OBJ},
    { std::make_pair(15, "WATER_ACTION_END_WITH_OBJ"), MARIO_ANIM_WATER_ACTION_END_WITH_OBJ}, // either swimming or flutterkicking
};

extern std::map<std::pair<int, std::string>, int> sanim_climbing = {
    // 6 : Climbing (20)
    { std::make_pair(0, "IDLE_ON_POLE"), MARIO_ANIM_IDLE_ON_POLE},
    { std::make_pair(1, "GRAB_POLE_SHORT"), MARIO_ANIM_GRAB_POLE_SHORT},
    { std::make_pair(2, "GRAB_POLE_SWING_PART1"), MARIO_ANIM_GRAB_POLE_SWING_PART1},
    { std::make_pair(3, "GRAB_POLE_SWING_PART2"), MARIO_ANIM_GRAB_POLE_SWING_PART2},
    { std::make_pair(4, "CLIMB_UP_POLE"), MARIO_ANIM_CLIMB_UP_POLE},
    { std::make_pair(5, "SLIDEJUMP"), MARIO_ANIM_SLIDEJUMP}, // pole jump and wall kick
    { std::make_pair(6, "HANDSTAND_IDLE"), MARIO_ANIM_HANDSTAND_IDLE},
    { std::make_pair(7, "HANDSTAND_JUMP"), MARIO_ANIM_HANDSTAND_JUMP},
    { std::make_pair(8, "START_HANDSTAND"), MARIO_ANIM_START_HANDSTAND},
    { std::make_pair(9, "RETURN_FROM_HANDSTAND"), MARIO_ANIM_RETURN_FROM_HANDSTAND},
    { std::make_pair(10, "HANDSTAND_LEFT"), MARIO_ANIM_HANDSTAND_LEFT},
    { std::make_pair(11, "HANDSTAND_RIGHT"), MARIO_ANIM_HANDSTAND_RIGHT},
    { std::make_pair(12, "IDLE_ON_LEDGE"), MARIO_ANIM_IDLE_ON_LEDGE},
    { std::make_pair(13, "SLOW_LEDGE_GRAB"), MARIO_ANIM_SLOW_LEDGE_GRAB},
    { std::make_pair(14, "FAST_LEDGE_GRAB"), MARIO_ANIM_FAST_LEDGE_GRAB},
    { std::make_pair(15, "CLIMB_DOWN_LEDGE"), MARIO_ANIM_CLIMB_DOWN_LEDGE},
    { std::make_pair(16, "HANG_ON_OWL"), MARIO_ANIM_HANG_ON_OWL},
    { std::make_pair(17, "HANG_ON_CEILING"), MARIO_ANIM_HANG_ON_CEILING},
    { std::make_pair(18, "MOVE_ON_WIRE_NET_RIGHT"), MARIO_ANIM_MOVE_ON_WIRE_NET_RIGHT},
    { std::make_pair(19, "MOVE_ON_WIRE_NET_LEFT"), MARIO_ANIM_MOVE_ON_WIRE_NET_LEFT},
};

extern std::map<std::pair<int, std::string>, int> sanim_object = {
    // 7 : Object (24)
    { std::make_pair(0, "IDLE_WITH_LIGHT_OBJ"), MARIO_ANIM_IDLE_WITH_LIGHT_OBJ},
    { std::make_pair(1, "PICK_UP_LIGHT_OBJ"), MARIO_ANIM_PICK_UP_LIGHT_OBJ},
    { std::make_pair(2, "THROW_LIGHT_OBJECT"), MARIO_ANIM_THROW_LIGHT_OBJECT},
    { std::make_pair(3, "PLACE_LIGHT_OBJ"), MARIO_ANIM_PLACE_LIGHT_OBJ},
    { std::make_pair(4, "WALK_WITH_LIGHT_OBJ"), MARIO_ANIM_WALK_WITH_LIGHT_OBJ},
    { std::make_pair(5, "SLOW_WALK_WITH_LIGHT_OBJ"), MARIO_ANIM_SLOW_WALK_WITH_LIGHT_OBJ},
    { std::make_pair(6, "RUN_WITH_LIGHT_OBJ"), MARIO_ANIM_RUN_WITH_LIGHT_OBJ},
    { std::make_pair(7, "JUMP_WITH_LIGHT_OBJ"), MARIO_ANIM_JUMP_WITH_LIGHT_OBJ},
    { std::make_pair(8, "JUMP_LAND_WITH_LIGHT_OBJ"), MARIO_ANIM_JUMP_LAND_WITH_LIGHT_OBJ},
    { std::make_pair(9, "FALL_WITH_LIGHT_OBJ"), MARIO_ANIM_FALL_WITH_LIGHT_OBJ},
    { std::make_pair(10, "FALL_LAND_WITH_LIGHT_OBJ"), MARIO_ANIM_FALL_LAND_WITH_LIGHT_OBJ},
    { std::make_pair(11, "SLIDING_ON_BOTTOM_WITH_LIGHT_OBJ"), MARIO_ANIM_SLIDING_ON_BOTTOM_WITH_LIGHT_OBJ},
    { std::make_pair(12, "STAND_UP_FROM_SLIDING_WITH_LIGHT_OBJ"), MARIO_ANIM_STAND_UP_FROM_SLIDING_WITH_LIGHT_OBJ},
    { std::make_pair(13, "FALL_FROM_SLIDING_WITH_LIGHT_OBJ"), MARIO_ANIM_FALL_FROM_SLIDE},
    { std::make_pair(14, "STOP_SLIDE_LIGHT_OBJ"), MARIO_ANIM_STOP_SLIDE_LIGHT_OBJ},
    { std::make_pair(15, "IDLE_HEAVY_OBJ"), MARIO_ANIM_IDLE_HEAVY_OBJ},
    { std::make_pair(16, "GRAB_HEAVY_OBJECT"), MARIO_ANIM_GRAB_HEAVY_OBJECT},
    { std::make_pair(17, "HEAVY_THROW"), MARIO_ANIM_HEAVY_THROW},
    { std::make_pair(18, "GROUND_THROW"), MARIO_ANIM_GROUND_THROW},
    { std::make_pair(19, "WALK_WITH_HEAVY_OBJ"), MARIO_ANIM_WALK_WITH_HEAVY_OBJ},
    { std::make_pair(20, "GRAB_BOWSER"), MARIO_ANIM_GRAB_BOWSER},
    { std::make_pair(21, "SWINGING_BOWSER"), MARIO_ANIM_SWINGING_BOWSER},
    { std::make_pair(22, "RELEASE_BOWSER"), MARIO_ANIM_RELEASE_BOWSER},
    { std::make_pair(23, "HOLDING_BOWSER"), MARIO_ANIM_HOLDING_BOWSER},
};

// unused
extern std::map<std::pair<int, std::string>, int> sanim_misc = {
    // 8 : Misc. (2)
    { std::make_pair(0, "TILT_SPAZ"), MARIO_ANIM_TILT_SPAZ}, // custom saturn anim for spazzing, credits to Weegeepie
    { std::make_pair(1, "A_POSE"), MARIO_ANIM_A_POSE},
};

extern std::vector<std::map<std::pair<int, std::string>, int>> sanim_maps = {
    sanim_movement,
    sanim_actions,
    sanim_automatic,
    sanim_damagedeaths,
    sanim_cutscenes,
    sanim_water,
    sanim_climbing,
    sanim_object,
    sanim_misc
};

extern std::map<int, int> saturn_anims_size = {
    { 0, 49 },
    { 1, 23 },
    { 2, 26 },
    { 3, 21 },
    { 4, 22 },
    { 5, 15 },
    { 6, 19 },
    { 7, 23 },
    { 8, 1 },
};

#endif

#endif