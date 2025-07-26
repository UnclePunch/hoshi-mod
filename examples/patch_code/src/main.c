#include "patches.h"

#include "hoshi/settings.h"

char ModName[] = "Code Patch Example"; // Name of the mod.
char ModAuthor[] = "Your Name";        // Creator of the mod.
char ModVersion[] = "v1.0";            // Version of the mod.

// Code patches can be applied anywhere, we will be applying them immediately on boot.
void OnBoot()
{
    Patches_Apply();
}