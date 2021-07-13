// ddd_sub.c.inc

void bhv_bowsers_sub_loop(void) {
#ifndef DYNOS_COOP
    extern s32 gDDDBowsersSub;

    // Always spawn
    if (gDDDBowsersSub == 1) {
        return;
    }

    // Never spawn
    if (gDDDBowsersSub == 0) {
        obj_mark_for_deletion(o);
        return;
    }
#endif

    if (save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_2 | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR))
        obj_mark_for_deletion(o);
}
