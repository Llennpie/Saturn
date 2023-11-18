#ifndef SaturnImGuiCcEditor
#define SaturnImGuiCcEditor

#ifdef __cplusplus
    #include <string>
    #include "saturn/saturn_models.h"

    extern int uiCcListId;

    extern void UpdateEditorFromPalette();

    extern void ResetColorCode(bool);
    extern void RefreshColorCodeList();

    extern void OpenCCSelector();
    extern void OpenModelCCSelector(Model, std::vector<std::string>, std::string);
    extern void OpenCCEditor();

    extern bool has_open_any_model_cc;

#endif

#endif