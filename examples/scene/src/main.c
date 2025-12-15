#include "hoshi/mod.h"
#include "hoshi/settings.h"

#include "header.h"

ModDesc mod_desc = {
    .name = "Scene Example",       // Name of the mod.
    .author = "Your Name",              // Creator of the mod.
    .version.major = 1,                 // Version of the mod.
    .version.minor = 0,
    .OnBoot = OnBoot,
};

// To-do
void OnBoot()
{
}