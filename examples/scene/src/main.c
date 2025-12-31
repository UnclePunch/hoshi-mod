#include "scene.h"
#include "hoshi/mod.h"
#include "hoshi/settings.h"

#include "header.h"
#include "scene_example.h"

// Creates a menu that appears in the in-game Settings menu.
OptionDesc ModSettings = {
    .name = "Example Scene",
    .description = "Enter a custom scene.",
    .kind = OPTKIND_SCENE,
    .major_idx = -1,
};


ModDesc mod_desc = {
    .name = "Example Scene",            // Name of the mod.
    .author = "UnclePunch",             // Creator of the mod.
    .version.major = 1,                 // Version of the mod.
    .version.minor = 0,
    .option_desc = &ModSettings,
    .OnBoot = OnBoot,
};

void OnBoot()
{
    // call function to install the scene and update the settings menu with its major scene index.
    ModSettings.major_idx = SceneExample_Install();    
}