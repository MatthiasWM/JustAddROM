

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Preferences.H>
#include <FL/filename.H>
#include <FL/Fl_File_Chooser.H>

Fl_Window *win;

uint8_t ROM[1024*1024*8];

void add_ROM()
{
	char romA[FL_PATH_MAX];
	char romB[FL_PATH_MAX];
	Fl_Preferences prefs(Fl_Preferences::USER, "org.messagepad", "JustAddROM");
	prefs.get("ROMFileA", romA, "", FL_PATH_MAX);
	prefs.get("ROMFileB", romB, "", FL_PATH_MAX);

	FILE *in;
	for (;;) {
		in = fopen(romA, "rb");
		if (in==NULL) {
			char *fn = fl_file_chooser("Choose Senior Cirrus image", "", "");
			if (fn==NULL)
				return;
		}
	}
}

int main(int argc, char **argv)
{
	win = new Fl_Window(640, 480, "JustAddROM");
	win->show(argc, argv);

	add_ROM();

	Fl::run();
	return 0;
}


