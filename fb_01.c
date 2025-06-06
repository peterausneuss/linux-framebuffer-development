#include <stdio.h>
#include <stdlib.h> // For malloc, free
#include <stdint.h> // For uint32_t

// Corrected macro definitions
#define ScreenWidth  1920 //[Breite des Framebuffers in Pixeln]
#define ScreenHeight 1080 //[Höhe des Framebuffers in Pixeln]

// Global back buffer, to be allocated in main
uint32_t *BackBuff = NULL;


void PSet(uint32_t x, uint32_t y, uint32_t Color) {
    if (x >= ScreenWidth || y >= ScreenHeight) {
        // fprintf(stderr, "PSet: Koordinaten (%u, %u) außerhalb der Grenzen (%d, %d)\n", x, y, ScreenWidth, ScreenHeight);
        return; // Out of bounds
    }
    if (!BackBuff) {
        // fprintf(stderr, "PSet: BackBuff nicht initialisiert.\n");
        return;
    }
    uint32_t Offset = (ScreenWidth * y) + x;
    BackBuff[Offset] = Color; // Dies ist ein Offset, keine physische Adresse.
}

void UpdateScreen(FILE *FB) {
    if (!FB) {
        fprintf(stderr, "UpdateScreen: Framebuffer (FB) ist NULL.\n");
        return;
    }
    if (!BackBuff) {
        fprintf(stderr, "UpdateScreen: BackBuff nicht initialisiert.\n");
        return;
    }

    // Seek to the beginning of the framebuffer
    if (fseek(FB, 0, SEEK_SET) != 0) {
        perror("UpdateScreen: Fehler bei fseek");
        return;
    }

    // Write the entire BackBuff to the framebuffer
    // fwrite returns the number of elements successfully written
    size_t elements_to_write = (size_t)ScreenWidth * ScreenHeight;
    size_t elements_written = fwrite(BackBuff, sizeof(uint32_t), elements_to_write, FB);

    if (elements_written != elements_to_write) {
        perror("UpdateScreen: Fehler bei fwrite oder unvollständiger Schreibvorgang");
        if (ferror(FB)) {
            fprintf(stderr, "UpdateScreen: Fehlerindikator für Stream gesetzt.\n");
        }
        fprintf(stderr, "UpdateScreen: Erwartet %zu Elemente zu schreiben, aber %zu geschrieben.\n", elements_to_write, elements_written);
        return;
    }

    // Ensure data is written to the device
    if (fflush(FB) != 0) {
        perror("UpdateScreen: Fehler bei fflush");
    }
}

uint32_t Point(uint32_t x, uint32_t y) {
    if (x >= ScreenWidth || y >= ScreenHeight) {
        // fprintf(stderr, "Point: Koordinaten (%u, %u) außerhalb der Grenzen (%d, %d)\n", x, y, ScreenWidth, ScreenHeight);
        return 0; // Return a default/error color (e.g., black)
    }
    if (!BackBuff) {
        // fprintf(stderr, "Point: BackBuff nicht initialisiert.\n");
        return 0; // Return a default/error color
    }
    uint32_t Offset = (ScreenWidth * y) + x;
    return BackBuff[Offset];
}

// sämtliche Pixel sind linear hintereinander in BackBuff als 32-Bit-
// Werte abgelegt und eine Zeile ist im Array BackBuff ScreenWidth Pixel lang

int main() {
    FILE *FB = NULL;

    // Allocate memory for the back buffer (assuming 32-bit color)
    BackBuff = (uint32_t *)malloc((size_t)ScreenWidth * ScreenHeight * sizeof(uint32_t));
    if (BackBuff == NULL) {
        perror("Fehler bei der Speicherreservierung für BackBuff");
        return 1;
    }

    // Initialisierung des Framebuffers
    // "wb+" opens for writing/reading, truncates if exists, creates if not.
    // For device files like /dev/fb0, "rb+" (must exist, open for read/write) is often preferred.
    // However, sticking to "wb+" as per original intent of overwriting.
    FB = fopen("/dev/fb0", "wb+");
    if (FB == NULL) {
        perror("Fehler beim Öffnen des Framebuffers");
        free(BackBuff); // Clean up allocated memory
        return 1;
    }

    // Beispiel: Setze einen Pixel auf eine Farbe 0xFF0000FF
    // (z.B. Blau bei ARGB: Alpha=FF, Rot=00, Grün=00, Blau=FF)
    PSet(100, 100, 0xFF0000FF);

    // Draw a red line 20 pixels thick starting at y=500, full screen width
    uint32_t red_color = 0xFFFF0000; // ARGB: Alpha=FF, Red=FF, Green=00, Blue=00
    int line_y_start = 500;
    int line_thickness = 20;

    for (int y = line_y_start; y < line_y_start + line_thickness; ++y) {
        // PSet has its own bounds checking, but this outer check can be an optimization
        if (y >= 0 && (uint32_t)y < ScreenHeight) {
            for (int x = 0; x < ScreenWidth; ++x) {
                PSet((uint32_t)x, (uint32_t)y, red_color);
            }
        }
    }

    // Aktualisiere den Bildschirm
    UpdateScreen(FB);
    // Lese einen Pixel
    uint32_t color = Point(100, 100);
    printf("Farbe des Pixels bei (100, 100): %08X\n", color);

    if (FB != NULL) {
        fclose(FB);
    }
    if (BackBuff != NULL) {
        free(BackBuff);
        BackBuff = NULL;
    }
    printf("Programm erfolgreich beendet.\n");
    return 0;
}
/*
// uint32_t BackBuff [ScreenWidth*ScreenHeight];

framebuffer_width=[Breite in Pixeln]
framebuffer_height=[Höhe in Pixeln]
framebuffer_depth=[Pixelbreite in Bits pro Pixel] //(32 Bit pro Pixel wird hier angenommen)

// auf die Datei /dev/fb0 nur mit blockorientierten Funktionen zugreifen: fread(), fwrite() und fseek()
*/
