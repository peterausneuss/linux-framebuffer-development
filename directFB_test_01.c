// DirectFB (Direct Frame Buffer):

// DirectFB is a graphics library that provides a hardware abstraction layer for graphics and input devices.
// It is designed to be lightweight and efficient, making it suitable for embedded systems and devices with limited resources.
// DirectFB provides a simple API for rendering graphics, handling input events, and managing windows.
// It supports various graphics operations such as drawing shapes, images, and text.
// DirectFB is often used in applications that require direct access to the framebuffer, such as media players, games, and user interfaces.
#include <stdio.h>
#include <directfb.h>
#include <directfb_util.h>
int main(int argc, char *argv[]) {
    IDirectFB *dfb;
    IDirectFBSurface *surface;
    DFBResult ret;

    // Initialize DirectFB
    ret = DirectFBInit(&argc, &argv);
    if (ret != DFB_OK) {
        fprintf(stderr, "DirectFBInit failed: %s\n", DirectFBErrorString(ret));
        return 1;
    }

    // Create the DirectFB interface
    ret = DirectFBCreate(&dfb);
    if (ret != DFB_OK) {
        fprintf(stderr, "DirectFBCreate failed: %s\n", DirectFBErrorString(ret));
        return 1;
    }

    // Create a surface
    ret = dfb->CreateSurface(dfb, NULL, &surface);
    if (ret != DFB_OK) {
        fprintf(stderr, "CreateSurface failed: %s\n", DirectFBErrorString(ret));
        dfb->Release(dfb);
        return 1;
    }

    // Clean up
    surface->Release(surface);
    dfb->Release(dfb);

    printf("DirectFB test completed successfully.\n");
    return 0;
}
// Compile with: gcc directFB_test_01.c -o directFB_test_01 -ldirectfb
// Run with: ./directFB_test_01
// This code initializes DirectFB, creates a DirectFB interface, and creates a surface.
// It checks for errors at each step and cleans up resources before exiting.
// Make sure you have the DirectFB development libraries installed on your system to compile this code.
//  sudo apt-get update
//  sudo apt-get install libdirectfb-dev
// The code is a simple test to ensure that DirectFB can be initialized and used to create a surface.
// It does not perform any graphics rendering or input handling, but serves as a basic setup for further development.
// Note: DirectFB is not actively maintained, and its use is generally discouraged in favor of more modern libraries like SDL or GLFW for new projects.
// Ensure you have the DirectFB development libraries installed on your system to compile this code.

