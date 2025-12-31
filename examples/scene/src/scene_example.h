#include "datatypes.h"
#include "obj.h"
#include "scene.h"

#define LOGO_SPEED 2.5
#define LOGO_WIDTH 1.7
#define LOGO_HEIGHT 0.82

#define CAM_GXLINK 10

typedef struct LogoData
{
    Vec2 position;
    Vec2 collision_box;
    float move_angle;
    float move_speed;
    int color_idx;
} LogoData;

typedef struct CollFlags
{
    u8 left : 1;
    u8 right : 1;
    u8 top : 1;
    u8 bottom : 1;
} CollFlags;

MajorKind SceneExample_Install();

void MajorScene_Enter();
void MajorScene_Exit();
void MinorScene_Load();
void MinorScene_Exit(void *data);
void MinorScene_Think(void *data);

void Camera_GX(GOBJ *g);

void Logo_Create(JOBJSet *dvd_set, float angle);
void Logo_Think(GOBJ *g);