#pragma once

#include "lvgl.h"

#define DEFINE_DIALOG(DIALOG) extern DIALOG g_ ## DIALOG;
#define IMPLEMENT_DIALOG(DIALOG) DIALOG g_ ## DIALOG;
#define GET_DIALOG_REF(DIALOG) &g_ ## DIALOG

class gui_dialog
{
public:
    gui_dialog();
    virtual void init() = 0;
    virtual void tick() = 0;
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void show();
protected:
    lv_obj_t *m_scr;
};