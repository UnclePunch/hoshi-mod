#include "hsd.h"
#include "useful.h"
#include "audio.h"
#include "game.h"
#include "inline.h"

#include "header.h"
#include "hoshi/settings.h"

u8 *alloc_arr;                    // pointer to an array allocated at runtime in OnBoot.
int is_play_sfx_every_second = 0; // variable that is updated by the player in the in-game settings menu via the ModMenu defined below.

char ModName[] = "Template Mod"; // Name of the mod.
char ModAuthor[] = "Your Name";  // Creator of the mod.
char ModVersion[] = "v1.0";      // Version of the mod.

int ModSaveSize = sizeof(struct TemplateSave); // Size of the save data your mod uses. A pointer to the saved data is passed into OnSaveInit.
TemplateSave *template_save_ptr;               // Pointer to the mod's save data. This must be updated manually in OnSaveInit by the mod author.

// Creates a menu that appears in the in-game Settings menu.
// Menus may be nested by setting the OptionDesc::kind to OPTKIND_MENU
MenuDesc ModMenu = {
    .name = "Template Menu",
    .option_num = 2,
    .options = (OptionDesc[]){
        {
            .name = "SFX Every Second",
            .kind = OPTKIND_VALUE,
            .val = &is_play_sfx_every_second,
            .value_num = 2,
            .value_names = (char *[]){
                "Off",
                "On",
            },
        },
        {
            .kind = OPTKIND_MENU,
            .menu_ptr = &(MenuDesc){
                .name = "Template Submenu",
                .option_num = 1,
                .options = (OptionDesc[]){
                    {
                        .name = "Debug Menu",
                        .kind = OPTKIND_SCENE,
                        .major_idx = MJRKIND_DEBUG,
                    },
                },
            },
        },
    },
};

// Runs immediately after the mod file is loaded.
// Calls to HSD_MemAlloc in THIS function specifically wil persist throughout the entire runtime of the game.
// All calls to HSD_MemAlloc from elsewhere will return an allocation that exists only within the current scene.
void OnBoot()
{
    OSReport("Hello from boot\n");

    alloc_arr = HSD_MemAlloc(sizeof(u8) * 200); // persistently allocate an array of size 200
}

// Runs on startup after the save data is loaded into memory.
// If your mod uses save data it is required to save a pointer to it so it can be interfaced with it later.
void OnSaveInit(TemplateSave *save, int is_req_init)
{
    if (is_req_init)
    {
        OSReport("save data for Template Mod created!\n");
        save->boot_num = 0;
    }
    else
        save->boot_num++;

    OSReport("Template Mod present for [%d] boot cycles\n", save->boot_num);

    template_save_ptr = save;
}

// Runs when entering the main menu.
void OnMainMenuLoad()
{
    OSReport("Entering the main menu.\n");
}

// Runs when entering the player select menu.
// Currently only executes when entering city trial player select.
void OnPlayerSelectLoad()
{
    OSReport("Entering the city trial player select menu.\n");
}

// Runs upon entering a 3D game. Can be either Air Ride or City Trial. Must be explicity checked using Gm_IsInCity().
// Players, riders, their machines, and the map have all been instantiated by the time this is executed.
void On3DLoad()
{
    // determine the game mode
    char *mode_name = Gm_IsInCity() ? "City Trial" : "Air Ride";
    OSReport("Now starting %s game on map [%d].\n", mode_name, Gm_GetCurrentGrKind());

    // loop across all 5 potential players
    for (int i = 0; i < 5; i++)
    {
        // skip non-present players
        if (Ply_GetPKind(i) == PKIND_NONE)
            continue;

        // get this rider's data
        GOBJ *rg = Ply_GetRiderGObj(i);
        RiderData *rd = rg->userdata;

        // get this rider's machine
        MachineKind machine_kind = rd->starting_machine_idx;

        // log some data on them
        OSReport("Player %d using rider [%d] color [%d] riding machine [%d].\n",
                 i + 1,
                 rd->kind,
                 rd->color_idx,
                 machine_kind);
    }
}

// Runs when pausing the match. The index of the pausing player is passed in as an argument.
void On3DPause(int pause_ply)
{
    OSReport("Player [%d] has paused the game.\n", pause_ply);
}

// Runs when unpausing the match.
void On3DUnpause()
{
    OSReport("Resuming the game.\n");
}

// Runs when exiting a match.
void On3DExit()
{
    OSReport("Exiting 3D.\n");
}

// Runs every scene change.
// The memory heap is destroyed and recreated every scene change, meaning HSD objects
// such as CObj's (camera) and JObj's (models) will not persist across them.
// This hook can be used to recreate processes/objects that should always be running.
void OnSceneChange()
{
    // Log out the current scene information
    OSReport("We are now entering major %d / minor %d\n", Scene_GetCurrentMajor(), Scene_GetCurrentMinor());

    // Create a GObj with a process to run every frame.
    GOBJ *g = GOBJ_EZCreator(0, 0, 0,                            // p_link 0 runs during 3d pause
                             sizeof(PerFrameFuncData), HSD_Free, // initialize gobj's data
                             HSD_OBJKIND_NONE, 0,                // gobj does not contain an hsd object
                             Func_PerFrame, 0,                   // per frame process
                             0, 0, 0);                           // not being rendered
}

////////////////////////////
// User Defined Functions //
////////////////////////////

void Func_PerFrame(GOBJ *g)
{
    // modify this value in the in-game settings menu
    if (is_play_sfx_every_second == 0)
        return;

    // retrieve gobj's data
    PerFrameFuncData *gp = g->userdata;

    gp->timer++;

    if (gp->timer > 60)
    {
        SFX_Play(FGMMENU_CS_KETTEI);
        gp->timer = 0;
    }
}