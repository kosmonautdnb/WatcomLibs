// actually to slow here with the newest DJGPP but very useful for WatcomC/OpenWatcom
// A little calculator / immediate mode C compilation and output
// usage: app_c 5*5
// or: app_c.exe and typing several lines followed by an empty line (just a simple return or a 'q')
// you have to configure here atleast the BASE_FOLDER first.
// if you have int values over 32767 the calculations lead to wrong results in WatcomC (due to WatcomC real mode), 
// better use floats e.g. "40000.0" instead of "40000"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE_FOLDER "C:\\_C_\\" // has to be changed

#define C_FILENAME BASE_FOLDER "PRG.CPP"
#define EXE_FILENAME BASE_FOLDER "PRG.EXE"
#define INTERMEDIATE_FILENAME BASE_FOLDER "ERRORS.TXT"

char buffer[2048];

int main(int argc, const char *argv[]) {

  FILE *out = fopen(C_FILENAME, "w");
  if (out == NULL) {printf("couldn't create %s\n",C_FILENAME); return 0;}

  fprintf(out,"#include <stdio.h>\n");
  fprintf(out,"#include <stdlib.h>\n");
  fprintf(out,"#include <conio.h>\n");
  fprintf(out,"#include <string.h>\n");
  fprintf(out,"#include <math.h>\n");
  fprintf(out,"float neededForWatcomC11FloatSupport = 0;\n");
  fprintf(out,"int main(int argc, const char *argv[]) {\n");

  if (argc > 1) {
    fprintf(out,"printf(\"%%f\\n\", (double)(");
    for (int i = 1; i < argc; i++) {
      fprintf(out," %s",argv[i]);
    }
    fprintf(out,"));\n");
  } else {
    while(1) {
      gets(buffer);
      if (strlen(buffer) == 0) break;
      if (strlen(buffer) == 1 && (buffer[0] == 'q' || buffer[0] == 'Q')) break;
      fprintf(out,"%s\n",buffer);
    }
  }

  fprintf(out,"return 0;}\n");
  fclose(out);

  remove(EXE_FILENAME);

#ifdef __WATCOMC__
  system("wcl.exe /fe=" EXE_FILENAME " " C_FILENAME " >" INTERMEDIATE_FILENAME);
#endif

#ifdef __DJGPP__
  system("gpp.exe -o" EXE_FILENAME " " C_FILENAME " >" INTERMEDIATE_FILENAME);
#endif

  FILE *in; in = fopen(EXE_FILENAME,"rb");
  if (in != NULL) { 
    fclose(in);
    system(EXE_FILENAME);
  } else {
    printf("<<ERROR in PROGRAM>>\n"); in = fopen(INTERMEDIATE_FILENAME,"r");
    if (in != NULL) {
      while(fgets(buffer,2048,in)) {
        printf("%s",buffer);
      }
      fclose(in);
    }
  }

  return 0;
}
