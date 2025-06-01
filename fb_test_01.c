// direct_fb_rect.c
// Demonstrates direct framebuffer access on Linux to draw a rectangle.
//
// How to compile:
//   gcc direct_fb_rect.c -o direct_fb_rect
//
// How to run (often requires root privileges):
//   sudo ./direct_fb_rect
//   or
//   sudo ./direct_fb_rect /dev/fb1 (if fb0 is not your target)
//
// Note: This program writes directly to the framebuffer. If a desktop
// environment (X11, Wayland) is running, it may overwrite these changes
// immediately or the drawing might only be visible on a TTY.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // For uintN_t types
#include <fcntl.h>    // For O_RDWR
#include <unistd.h>   // For close(), read(), write()
#include <sys/ioctl.h> // For ioctl()
#include <sys/mman.h>  // For mmap(), munmap()
#include <linux/fb.h>  // For framebuffer specific definitions

// Helper function to draw a single pixel
// This function tries to handle common 16bpp, 24bpp, and 32bpp formats.
void draw_pixel(char *fbp, const struct fb_var_screeninfo *vinfo, const struct fb_fix_screeninfo *finfo,
                int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    // Check bounds against visible screen resolution
    if (x < 0 || x >= vinfo->xres || y < 0 || y >= vinfo->yres) {
        return; // Pixel is outside the visible screen area
    }

    // Calculate the offset in the framebuffer memory
    // Takes into account virtual screen offsets and line length (stride)
    long location = (x + vinfo->xoffset) * (vinfo->bits_per_pixel / 8) +
                    (y + vinfo->yoffset) * finfo->line_length;

    if (vinfo->bits_per_pixel == 32) {
        uint32_t pixel_color = 0;
        pixel_color |= (uint32_t)r << vinfo->red.offset;
        pixel_color |= (uint32_t)g << vinfo->green.offset;
        pixel_color |= (uint32_t)b << vinfo->blue.offset;
        if (vinfo->transp.length > 0) {
            pixel_color |= (uint32_t)a << vinfo->transp.offset;
        }
        *((uint32_t*)(fbp + location)) = pixel_color;
    } else if (vinfo->bits_per_pixel == 16) { // Common case: RGB565
        // Scale 8-bit R,G,B to fit into the bitfield lengths
        uint16_t r_scaled = (r * ((1 << vinfo->red.length) - 1)) / 255;
        uint16_t g_scaled = (g * ((1 << vinfo->green.length) - 1)) / 255;
        uint16_t b_scaled = (b * ((1 << vinfo->blue.length) - 1)) / 255;

        uint16_t pixel_color = (r_scaled << vinfo->red.offset) |
                               (g_scaled << vinfo->green.offset) |
                               (b_scaled << vinfo->blue.offset);
        *((uint16_t*)(fbp + location)) = pixel_color;
    } else if (vinfo->bits_per_pixel == 24) {
        // Assuming byte-aligned components for common RGB888 or BGR888
        if (vinfo->red.offset == 0 && vinfo->green.offset == 8 && vinfo->blue.offset == 16) { // RGB
            *(fbp + location + 0) = r;
            *(fbp + location + 1) = g;
            *(fbp + location + 2) = b;
        } else if (vinfo->blue.offset == 0 && vinfo->green.offset == 8 && vinfo->red.offset == 16) { // BGR
            *(fbp + location + 0) = b;
            *(fbp + location + 1) = g;
            *(fbp + location + 2) = r;
        } else {
            fprintf(stderr, "Warning: Unsupported 24bpp pixel format (R:%u, G:%u, B:%u offsets).\n",
                    vinfo->red.offset, vinfo->green.offset, vinfo->blue.offset);
        }
    } else {
        fprintf(stderr, "Warning: Unsupported bits_per_pixel: %d\n", vinfo->bits_per_pixel);
    }
}

int main(int argc, char *argv[]) {
    int fbfd = 0; // Framebuffer file descriptor
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    char *fbp = NULL; // Pointer to memory mapped framebuffer

    const char *fb_device = "/dev/fb0"; // Default framebuffer device
    if (argc > 1) {
        fb_device = argv[1]; // Allow specifying device, e.g., /dev/fb1
    }

    // Open the framebuffer device file
    fbfd = open(fb_device, O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return EXIT_FAILURE;
    }
    printf("Framebuffer device %s opened successfully.\n", fb_device);

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed screen information");
        close(fbfd);
        return EXIT_FAILURE;
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable screen information");
        close(fbfd);
        return EXIT_FAILURE;
    }

    printf("Screen Info:\n");
    printf("  Resolution: %dx%d, %d bpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    printf("  Virtual Res: %dx%d\n", vinfo.xres_virtual, vinfo.yres_virtual);
    printf("  Offset: x=%d, y=%d\n", vinfo.xoffset, vinfo.yoffset);
    printf("  Line Length: %d bytes\n", finfo.line_length);
    printf("  Red:   offset=%2u, length=%2u, msb_right=%u\n", vinfo.red.offset, vinfo.red.length, vinfo.red.msb_right);
    printf("  Green: offset=%2u, length=%2u, msb_right=%u\n", vinfo.green.offset, vinfo.green.length, vinfo.green.msb_right);
    printf("  Blue:  offset=%2u, length=%2u, msb_right=%u\n", vinfo.blue.offset, vinfo.blue.length, vinfo.blue.msb_right);
    if (vinfo.transp.length > 0) {
        printf("  Alpha: offset=%2u, length=%2u, msb_right=%u\n", vinfo.transp.offset, vinfo.transp.length, vinfo.transp.msb_right);
    }

    // Calculate the size of the screen in bytes
    screensize = finfo.smem_len; // Total size of framebuffer memory
    // Alternative calculation: screensize = vinfo.xres_virtual * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;
    printf("Total framebuffer size: %ld bytes\n", screensize);

    // Map the framebuffer device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (fbp == MAP_FAILED) { // Check if mmap failed (MAP_FAILED is (void *)-1)
        perror("Error: failed to map framebuffer device to memory");
        close(fbfd);
        return EXIT_FAILURE;
    }
    printf("Framebuffer device mapped to memory successfully.\n");

    // --- Draw an 8x8 white rectangle in the center of the visible screen ---
    int rect_w = 108;
    int rect_h = 108;
    // Calculate top-left corner for centering on the visible screen
    int rect_x_start = (vinfo.xres - rect_w) / 2;
    int rect_y_start = (vinfo.yres - rect_h) / 2;

    printf("Drawing %dx%d white rectangle at (%d, %d) on visible screen.\n",
           rect_w, rect_h, rect_x_start, rect_y_start);

    for (int y_offset = 0; y_offset < rect_h; y_offset++) {
        for (int x_offset = 0; x_offset < rect_w; x_offset++) {
            draw_pixel(fbp, &vinfo, &finfo,
                       rect_x_start + x_offset, rect_y_start + y_offset,
                       255, 255, 255, 255); // White color (R,G,B,A)
        }
    }

    printf("Rectangle drawn. Press Enter to unmap, close, and exit...\n");
    getchar(); // Wait for user to see the result

    // Unmap the framebuffer memory
    if (munmap(fbp, screensize) == -1) {
        perror("Error un-mapping framebuffer");
        // Continue to close fbfd anyway
    }
    // Close the framebuffer file descriptor
    close(fbfd);

    printf("Framebuffer resources released. Exiting.\n");
    return EXIT_SUCCESS;
}