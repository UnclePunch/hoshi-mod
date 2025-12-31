#include "scene.h"
#include "hoshi/mod.h"
#include "hoshi/settings.h"
#include "hoshi/screen_cam.h"
#include "hoshi/func.h"

#include "os.h"
#include "text.h"

#include "scene_example.h"

// a major scene is a collection of minor scenes
MajorSceneDesc major_desc = {
    .major_id = -1,
    .next_major_id = 0,
    .initial_minor_id = -1,
    .cb_Enter = MajorScene_Enter,
    .cb_ExitMinor = MajorScene_Exit,
};

// a minor scene is the actual scene instance the user sees and interacts with.
MinorSceneDesc minor_scene = {
    .idx = -1,
    .x1 = -1,
    .cb_Load = MinorScene_Load,
    .cb_Exit = MinorScene_Exit,
    .cb_ThinkPreGObjProc = MinorScene_Think,
    .preload_kind = 0, // 4 will preserve menu file preloads
};

MajorKind SceneExample_Install()
{
    // install our scene
    minor_scene.idx = Hoshi_InstallMinorScene(&minor_scene);
    major_desc.initial_minor_id = minor_scene.idx;
    major_desc.major_id = Hoshi_InstallMajorScene(&major_desc);

    return major_desc.major_id;
}

// Major Scene Functions
void MajorScene_Enter()
{
    OSReport("Entered major!\n");
}
void MajorScene_Exit()
{
    OSReport("Exited major!\n");
}

// camera object parameters our scene will use to render objects
COBJDesc cobj_desc = {
    .class_name = 0,
    .flags = 0x1,
    .projection_type = PROJ_ORTHO,
    .viewport_left = 0,
    .viewport_right = 640,
    .viewport_top = 0,
    .viewport_bottom = 480,
    .scissor_lr = 640,
    .scissor_tb = 480,
    .eye_desc = &(WOBJDesc){.pos = {0, 0, 300}},
    .interest_desc = &(WOBJDesc){.pos = {0, 0, 0}},
    .roll = 0,
    .vector = &(Vec3){0, 1, 0},
    .near = 0.1,
    .far = 32777777,
    .projection_param.ortho = 
    {
        .top = 240,
        .bottom = -240,
        .left = -320,
        .right = 320,
    },
};
GOBJ *cam_gobj;

// Minor Scene Functions
void MinorScene_Load()
{
    OSReport("Entered minor!\n");

    // create camera
    cam_gobj = GOBJ_EZCreator(0, 0, 0,
                0, 0,
                HSD_OBJKIND_COBJ, &cobj_desc,
                0, 0,
                Camera_GX, (1ULL << CAM_GXLINK), 0);

    // load asset file
    HSD_Archive *archive = Archive_LoadFile("IfScene.dat");
    JOBJSet **scene_models = Archive_GetPublicAddress(archive, "ScInfDVD_scene_models");
    JOBJSet *dvd_set = scene_models[0];

    // create logo
    Logo_Create(dvd_set,  M_1DEGREE * 300);
}
void MinorScene_Exit(void *data)
{
    OSReport("Exited minor!\n");

    // decide where to go
    if (Scene_GetDirection() == PAD_BUTTON_B)
    {
        GameData *gd = Gm_GetGameData();
        gd->main_menu.top_menu = MAINMENU_TOPMENU_OPTIONS;
        gd->main_menu.is_in_submenu = 1;
        gd->main_menu.cursor_val[0] = 4;
        gd->main_menu.cursor_val[1] = 0;

        Scene_SetNextMajor(MJRKIND_MENU);
        Scene_ExitMajor();
    }

}
void MinorScene_Think(void *data)
{
    // get the pressed inputs of all controllers
    int down = Pad_GetDown(20);

    // check to exit
    if (down & (PAD_BUTTON_B | PAD_BUTTON_START))
    {
        SFX_Play(FGMMENU_CS_CANCEL);
        Scene_SetDirection(PAD_BUTTON_B);   // PAD_BUTTON_B serves as a "back" indicator
        Scene_ExitMinor();                  // signals to exit the scene loop and decide the next scene by calling MinorScene_Exit
    }
}

void Camera_GX(GOBJ *g)
{
    // set cobj
    if (!CObj_SetCurrent(g->hsd_object))
        return;
    
    // set background to black
    CObj_SetEraseColor(0, 0, 0, 255);
    CObj_EraseScreen(g->hsd_object, GX_ENABLE, GX_DISABLE, GX_DISABLE);

    // render all models in the scene (gobjs with gx_link CAM_GXLINK)
    CObj_RenderGXLinks(g, (1 << 0) | (1 << 1) | (1 << 2));

    return;
}

static GXColor logo_colors[] = {
    { 255, 255, 255, 255 },
    { 128, 128, 255, 255 },
    { 255, 128, 128, 255 },
    { 255, 128, 255, 255 },
    { 128, 255, 128, 255 },
    { 128, 255, 255, 255 },
    { 255, 255, 128, 255 }, 
};
void Logo_Create(JOBJSet *dvd_set, float angle)
{
    COBJ *c = cam_gobj->hsd_object;

    // instantiate
    GOBJ *dvd_gobj = GOBJ_EZCreator(0, 0, 0,
                        sizeof(LogoData), HSD_Free,
                        HSD_OBJKIND_JOBJ, dvd_set->jobj,    
                        Logo_Think, 0,
                        JObj_GX, CAM_GXLINK, 0);

    // get pointer to logo joint object (model)
    JOBJ *lj = dvd_gobj->hsd_object;

    // init logo's data
    LogoData *ld = dvd_gobj->userdata;
    ld->collision_box = (Vec2){LOGO_WIDTH * lj->scale.X, LOGO_HEIGHT * lj->scale.Y};
    ld->position = (Vec2){c->projection_param.ortho.left + ld->collision_box.X, 
                            c->projection_param.ortho.top - ld->collision_box.Y};
    ld->move_angle = angle;
    ld->move_speed = LOGO_SPEED;
    ld->color_idx = 0;

    // OSReport("logo placed at (%.2f, %.2f)\n", ld->position.X, ld->position.Y);
}
void Logo_Think(GOBJ *g)
{
    LogoData *ld = g->userdata;
    JOBJ *lj = g->hsd_object;
    COBJ *c = cam_gobj->hsd_object;

    // move logo
    ld->position.X += cos(ld->move_angle) * ld->move_speed;
    ld->position.Y += sin(ld->move_angle) * ld->move_speed;

    // OSReport("logo at (%.2f, %.2f)\n", ld->position.X, ld->position.Y);
    // OSReport("angle %.2f\n", ld->move_angle / M_1DEGREE);

    // collision check
    CollFlags coll_flags = {0};
    if (ld->position.X + ld->collision_box.X > c->projection_param.ortho.right)
        coll_flags.right = 1;
    if (ld->position.X - ld->collision_box.X < c->projection_param.ortho.left)
        coll_flags.left = 1;
    if (ld->position.Y + ld->collision_box.Y > c->projection_param.ortho.top)
        coll_flags.top = 1;
    if (ld->position.Y - ld->collision_box.Y < c->projection_param.ortho.bottom)
        coll_flags.bottom = 1;

    if (coll_flags.right || coll_flags.left || coll_flags.top || coll_flags.bottom)
    {
        OSReport("collision detected!\n");

        // move inside
        if (coll_flags.right)
            ld->position.X = c->projection_param.ortho.right - ld->collision_box.X;
        if (coll_flags.left)
            ld->position.X = c->projection_param.ortho.left + ld->collision_box.X;
        if (coll_flags.top)
            ld->position.Y = c->projection_param.ortho.top - ld->collision_box.Y;
        if (coll_flags.bottom)
            ld->position.Y = c->projection_param.ortho.bottom + ld->collision_box.Y;

        // reflect angle
        if (coll_flags.right || coll_flags.left)
            ld->move_angle = atan2(sin(ld->move_angle), -cos(ld->move_angle));
        if (coll_flags.top || coll_flags.bottom)
            ld->move_angle = atan2(-sin(ld->move_angle), cos(ld->move_angle));

        // change logo color
        int color_idx;
        do
        {
            color_idx = HSD_Randi(sizeof(logo_colors) / sizeof(logo_colors[0]));
        } while (color_idx == ld->color_idx);
        
        // set mesh's material diffuse color
        ld->color_idx = color_idx;
        JObj_GetDObjIndex(lj, 1, 0)->mobj->mat->diffuse = logo_colors[ld->color_idx];

        // OSReport("new position (%.2f, %.2f)\n", ld->position.X, ld->position.Y);
        // OSReport("new angle %.2f\n", ld->move_angle / M_1DEGREE);
    }

    // update logo model position
    lj->trans.X = ld->position.X;
    lj->trans.Y = ld->position.Y;
    JObj_SetMtxDirtySub(lj);

    // update any animations it may have
    JObj_AnimAll(g->hsd_object);
}