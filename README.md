# linux-framebuffer-development
 hardwareunabhängige API, die direkten Zugriff auf den Videospeicher (Framebuffer) eines Computers ermöglicht. Es ist ein Kernelmodul, das als Gerätedatei (/dev/fb0, /dev/fb1 usw.) zugänglich ist.


inux Framebuffer programmieren ohne X
Einsatz in der Spieleentwicklung, für eingebettete Systeme oder wenn man die X Window System (KDE, GNOME usw. basieren darauf) vermeiden möchte.
Hier sind die Hauptansätze und Bibliotheken, die du dafür nutzen kannst:
Framebuffer-Programmierung (Linux Framebuffer Device, fbdev):
Was es ist: Das Linux-Framebuffer ist eine hardwareunabhängige API, die direkten Zugriff auf den Videospeicher (Framebuffer) eines Computers ermöglicht. Es ist ein Kernelmodul, das als Gerätedatei (/dev/fb0, /dev/fb1 usw.) zugänglich ist.
Wie es funktioniert: Du kannst direkt Pixeldaten in diese Gerätedatei schreiben, um Grafiken auf dem Bildschirm darzustellen. Dies ermöglicht dir, den gesamten Bildschirm zu kontrollieren, ohne einen Display-Server wie X zu starten.
Für deine Anforderungen:
Hintergrund aus Bitmap-Tiles: Du liest deine Tile-Bitmaps ein und schreibst sie in den entsprechenden Speicherbereich des Framebuffers, um den Hintergrund zu zeichnen.
Scrollbarer Hintergrund: Du kannst den Bereich, der angezeigt wird, einfach verschieben, indem du die Startadresse im Framebuffer neu berechnest oder den Inhalt des Speichers verschiebst, bevor er an den Bildschirm gesendet wird.
Sprites: Sprites sind im Grunde kleine Bitmaps, die du über den Hintergrund zeichnest. Du musst die Transparenz (Alpha-Werte) der Sprites berücksichtigen und sie korrekt auf den Hintergrund rendern.
Vorteile: Sehr niedrige Ebene, volle Kontrolle, keine Abhängigkeiten von X11. Ideal für Embedded-Systeme oder Performance-kritische Anwendungen.
Nachteile: Sehr aufwändig, da du alles selbst implementieren musst (Pixel-Operationen, Scrolling-Logik, Sprite-Management, Event-Handling für Tastatur/Maus). Es ist nicht hardwarebeschleunigt, es sei denn, der Treiber bietet entsprechende Funktionen.
Bibliotheken, die auf dem Framebuffer aufsetzen (oder es als Backend nutzen können):
DirectFB (Direct Frame Buffer):
Was es ist: Eine Software-Bibliothek mit geringem Speicherbedarf, die Grafikanpassung, Eingabegeräte-Handling und eine Abstraktionsschicht direkt auf dem Linux-Framebuffer bietet. Sie kann auch ein integriertes Fenstersystem unterstützen.
Für deine Anforderungen: DirectFB ist ideal für das, was du beschreibst. Es wurde oft für Spiele und eingebettete Systeme verwendet, um den Overhead eines vollen X Window Systems zu umgehen. Es bietet Funktionen für das Zeichnen von Bitmaps, Blitting (schnelles Kopieren von Pixeldaten), Ebenenmanagement und Eingabeverarbeitung.
Vorteile: Bietet eine höhere Abstraktionsschicht als der direkte Framebuffer-Zugriff, unterstützt Hardware-Beschleunigung, wenn vom Treiber verfügbar, und ist darauf ausgelegt, ohne X zu laufen.
Nachteile: Die Entwicklung von DirectFB ist in den letzten Jahren etwas langsamer geworden, aber es gibt noch das "DirectFB2"-Projekt.
SDL (Simple DirectMedia Layer):
Was es ist: Eine beliebte plattformübergreifende Multimedia-Bibliothek, die eine hardwareunabhängige Schnittstelle für Grafiken, Audio, Eingabegeräte und mehr bietet. SDL ist weit verbreitet in der Spieleentwicklung.
Ohne X: SDL kann so konfiguriert werden, dass es direkt den Linux-Framebuffer verwendet (statt X11, Wayland oder andere Display-Server). Dies wird oft als "fbdev-Backend" oder "direct rendering" bezeichnet.
Für deine Anforderungen: SDL 2.0 bietet leistungsstarke 2D-Rendering-Funktionen, die ideal für Bitmap-Tiles und Sprites sind. Es abstrahiert die komplexen Details des Framebuffers für dich. Scrolling kann durch Anpassen der Render-Offsets oder durch geschicktes Blitting erreicht werden.
Vorteile: Sehr gut dokumentiert, große Community, viele Tutorials und Beispiele, hardwarebeschleunigtes 2D-Rendering auf unterstützter Hardware (auch ohne X), unterstützt alle von dir genannten Features.
Nachteile: Es ist eine Bibliothek, die du in C/C++ in deinem eigenen Programm verwenden musst. Du musst die gesamte Spiel- oder Anwendungsslogik selbst schreiben.
Allegro:
Was es ist: Eine weitere plattformübergreifende Spieleprogrammierbibliothek, ähnlich wie SDL, die sich auf 2D-Spiele und Multimedia konzentriert.
Ohne X: Allegro kann auch so konfiguriert werden, dass es direkt auf dem Linux-Framebuffer läuft, ohne X.
Für deine Anforderungen: Bietet alle notwendigen Funktionen für Bitmap-Grafiken, Sprites, Scrolling und Eingabeverarbeitung.
Vorteile: Eine etablierte Bibliothek mit einer guten Feature-Palette für 2D-Spiele.
Nachteile: Ähnlich wie SDL erfordert es die Programmierung der gesamten Logik.
Zusammenfassend:
Der Schlüssel liegt darin, das X Window System (und damit Desktops wie KDE) zu umgehen und direkt mit der Grafikhardware auf einer niedrigeren Ebene zu interagieren.
Für die größte Kontrolle und um die Funktionsweise wirklich zu verstehen: Direkte Framebuffer-Programmierung (C/C++).
Für eine gute Balance aus Kontrolle, Funktionalität und Entwicklungsfreundlichkeit: SDL oder Allegro (mit fbdev-Backend). Diese sind wahrscheinlich die beste Wahl für die meisten Projekte, die deine Anforderungen erfüllen.
Eine spezialisierte Option, die ebenfalls gut passt, ist DirectFB, falls du die Bibliothek magst und die Entwicklung für deine Zwecke ausreichend ist.

Ziele:
Uridium Clone
Intro/Demo  Programmierung
