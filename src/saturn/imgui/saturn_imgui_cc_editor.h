#ifndef SaturnImGuiCcEditor
#define SaturnImGuiCcEditor

#ifdef __cplusplus

    extern int uiCcListId;

    extern void UpdateEditorFromPalette();

    extern void ResetColorCode();
    extern void RefreshColorCodeList();
    extern void RefreshModelColorCodeList();

    extern void OpenCCSelector();
    extern void OpenCCEditor();

    extern bool has_open_any_model_cc;

#endif

#endif