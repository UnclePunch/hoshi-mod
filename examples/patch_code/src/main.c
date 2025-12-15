#include "patches.h"

#include "hoshi/mod.h"
#include "hoshi/settings.h"

// Code patches can be applied anywhere, we will be applying them immediately on boot.
void OnBoot()
{
    Patches_Apply();
}

ModDesc mod_desc = {
    .name = "Code Patch Example",       // Name of the mod.
    .author = "Your Name",              // Creator of the mod.
    .version.major = 1,                 // Version of the mod.
    .version.minor = 0,
    .OnBoot = OnBoot,
};