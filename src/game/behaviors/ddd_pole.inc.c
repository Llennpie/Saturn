
void bhv_ddd_pole_init(void) {
#ifndef DYNOS_COOP
    extern s32 gDDDPoles;

    // Always spawn
    if (gDDDPoles == 1) {
        o->hitboxDownOffset = 100.0f;
        o->oDDDPoleMaxOffset = 100.0f * o->oBehParams2ndByte;
        return;
    }

    // Never spawn
    if (gDDDPoles == 0) {
        obj_mark_for_deletion(o);
        return;
    }
#endif

    if (!(save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_2 | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR))) {
        obj_mark_for_deletion(o);
    } else {
        o->hitboxDownOffset = 100.0f;
        o->oDDDPoleMaxOffset = 100.0f * o->oBehParams2ndByte;
    }
}

void bhv_ddd_pole_update(void) {
    if (o->oTimer > 20) {
        o->oDDDPoleOffset += o->oDDDPoleVel;

        if (clamp_f32(&o->oDDDPoleOffset, 0.0f, o->oDDDPoleMaxOffset)) {
            o->oDDDPoleVel = -o->oDDDPoleVel;
            o->oTimer = 0;
        }
    }

    obj_set_dist_from_home(o->oDDDPoleOffset);
}
