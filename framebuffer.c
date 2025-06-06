#include <stdio.h>
#define ScreenWidth = 1920; //[Breite des Framebuffers in Pixeln]
#define ScreenHeight = 1080; //[Höhe des Framebuffers in Pixeln]


void PSet(uint32_t x, uint32_t y, uint32_t Color) {
uint32_t Offset=(ScreenWidth*y)+x;
BackBuff[Offset]=Color; // Dies ist ein Offset, keine physische Adresse.
}

void UpdateScreen() {
uint32_t FBSize=ScreenWidth*ScreenHeight*32;
fseek(FB,0,SEEK_SET);
fwrite(&FB,1,FBSize);
}

uint32_t Point(uint32_t x, uint32_t y) {
uint32_t Color;
uint32_t Offset=(ScreenWidth*y)+x;
return BackBuff[Offset];
}


// sämtliche Pixel sind linear hintereinander in BackBuff als 32-Bit-
// Werte abgelegt und eine Zeile ist im Array BackBuff ScreenWidth Pixel lang

int main() {
    // Initialisierung des Framebuffers
    FILE *FB = fopen("/dev/fb0", "wb+");
    if (FB == NULL) {
        perror("Fehler beim Öffnen des Framebuffers");
        return 1;
    }

    // Beispiel: Setze einen Pixel auf eine Farbe
    PSet(100, 100, 0xFF0000FF); // Setzt den Pixel bei (100, 100) auf Rot

    // Aktualisiere den Bildschirm
    UpdateScreen();

    // Lese einen Pixel
    uint32_t color = Point(100, 100);
    printf("Farbe des Pixels bei (100, 100): %08X\n", color);

    fclose(FB);
    return 0;
}
/*
// uint32_t BackBuff [ScreenWidth*ScreenHeight];

framebuffer_width=[Breite in Pixeln]
framebuffer_height=[Höhe in Pixeln]
framebuffer_depth=[Pixelbreite in Bits pro Pixel] //(32 Bit pro Pixel sind Standard

// auf die Datei /dev/fb0 nur mit blockorientierten Funktionen zugreifen: fread(), fwrite() und fseek()
*/



