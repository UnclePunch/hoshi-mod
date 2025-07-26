#include "code_patch/code_patch.h"
#include "event.h"
#include "machine.h"
#include "os.h"

#include "patches.h"

// Here we will declare code patches using macros which contain the assembly the game executes.
// This must be decalred early in the source file because the macro abstracts a function declaration.

// example code to be executed when an event is triggered
CODEPATCH_HOOKCREATE(0x800ee0a0,     // Memory address to begin executing custom code
                     "mr 3,27\n\t",  // ASM instructions to execute before calling our C function. In this case we move r27 (incoming event index) into r3 (to pass into our function).
                     Event_OnDecide, // Pointer to our C function
                     "",             // ASM instructions to execute after calling our C function (none).
                     0);             // Return address. Use 0 to branch back to the injection site.

void Event_OnDecide(EventKind kind)
{
    OSReport("Event kind %d incoming.\n", kind);
}

// Function to handle applying the code patches at runtime.
void Patches_Apply()
{
    CODEPATCH_HOOKAPPLY(0x800ee0a0); // Applies the hook for the memory address 0x800ee0a0 defined above using CODEPATCH_HOOKCREATE

    CODEPATCH_REPLACEINSTRUCTION(0x8000d34c, 0x38800000 | VCKIND_DRAGOON); // modify the instruction at 0x8000d34c to "li r4, VCKND_DRAGOON"
}