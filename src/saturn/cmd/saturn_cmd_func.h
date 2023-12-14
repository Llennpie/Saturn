#ifndef SaturnCommandFunctions
#define SaturnCommandFunctions

#include "saturn_cmd.h"

extern void cmd_mario_angle(CommandContext context);
extern void cmd_mario_pos(CommandContext context);
extern void cmd_mario_size_linked(CommandContext context);
extern void cmd_mario_size(CommandContext context);
extern void cmd_mario_walkpoint(CommandContext context);
extern void cmd_mario_state_powerup(CommandContext context);
extern void cmd_mario_state_eye(CommandContext context);
extern void cmd_mario_state_hands(CommandContext context);
extern void cmd_mario_state_cap(CommandContext context);
extern void cmd_camera_freeze(CommandContext context);
extern void cmd_camera_unfreeze(CommandContext context);
extern void cmd_camera_pos(CommandContext context);
extern void cmd_camera_angle(CommandContext context);
extern void cmd_camera_fov(CommandContext context);
extern void cmd_camera_follow(CommandContext context);
extern void cmd_timeline_delete(CommandContext context);
extern void cmd_timeline_curve(CommandContext context);
extern void cmd_timeline_value_float(CommandContext context);
extern void cmd_timeline_value_bool(CommandContext context);
extern void cmd_colorcode(CommandContext context);
extern void cmd_warp_register(CommandContext context);
extern void cmd_warp_to(CommandContext context);
extern void cmd_object_mario(CommandContext context);
extern void cmd_object_mario_behparam(CommandContext context);
extern void cmd_object_register_pos(CommandContext context);
extern void cmd_object_register_angle(CommandContext context);
extern void cmd_object_register_behparam(CommandContext context);
extern void cmd_object_constant_pos(CommandContext context);
extern void cmd_object_constant_angle(CommandContext context);
extern void cmd_object_constant_behparam(CommandContext context);
extern void cmd_animation(CommandContext context);
extern void cmd_animation_speed(CommandContext context);
extern void cmd_animation_looping(CommandContext context);
extern void cmd_animation_hanging(CommandContext context);
extern void cmd_animation_stop(CommandContext context);
extern void cmd_location_warp(CommandContext context);
extern void cmd_location_delete(CommandContext context);
extern void cmd_location_save_mario(CommandContext context);
extern void cmd_location_save_constant(CommandContext context);
extern void cmd_location_save_register(CommandContext context);
extern void cmd_environment(CommandContext context);
extern void cmd_call(CommandContext context);
extern void cmd_if(CommandContext context);
extern void cmd_register_set(CommandContext context);
extern void cmd_register_assign(CommandContext context);
extern void cmd_register_copy(CommandContext context);
extern void cmd_register_show(CommandContext context);
extern void cmd_operation(CommandContext context);
extern void cmd_goto(CommandContext context);
extern void cmd_wait(CommandContext context);
extern void cmd_alias_set(CommandContext context);
extern void cmd_alias_delete(CommandContext context);
extern void cmd_kill(CommandContext context);

#endif