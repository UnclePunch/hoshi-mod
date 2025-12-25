typedef struct TemplateSave
{
    int boot_num;
} TemplateSave;

typedef struct PerFrameFuncData
{
    int timer;
} PerFrameFuncData;

void OnBoot();
void OnSaveInit();
void OnSaveLoaded();
void OnMainMenuLoad();
void OnPlayerSelectLoad();
void On3DLoadStart();
void On3DLoadEnd();
void On3DPause(int pause_ply);
void On3DUnpause(int pause_ply);
void On3DExit();
void OnSceneChange();
void OnFrame();

void GOBJ_PerFrame(GOBJ *g);