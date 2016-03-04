#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include <X11/Xlib.h>

#include "xosd.h"

void
printerror()
{
  fprintf(stderr, "ERROR: %s\n", xosd_error);
}

int
main(int argc, char *argv[])
{
  xosd *osd;
  int a;

  if (setlocale(LC_ALL, "") == NULL || !XSupportsLocale())
    fprintf(stderr, "Locale not available, expect problems with fonts.\n");

  osd = xosd_create(4);

  if (0 != xosd_set_outline_offset(osd, 1)) {
    printerror();
  }

  if (0 != xosd_set_font(osd, (char *) osd_default_font)) {
    printerror();
  }

  if (0 != xosd_set_timeout(osd, 2)) {
    printerror();
  }


  if (-1 == xosd_display(osd, 3, XOSD_string, "Blah")) {
    printerror();
  }

  if (-1 == xosd_display(osd, 0, XOSD_string, "blah2")) {
    printerror();
  }

  if (-1 == xosd_display(osd, 1, XOSD_string, "wibble")) {
    printerror();
  }

  sleep(1);

  if (0 != xosd_scroll(osd, 1)) {
    printerror();
  }

  if (-1 == xosd_display(osd, 1, XOSD_string, "bloggy")) {
    printerror();
  }

  sleep(1);

  if (0 != xosd_scroll(osd, 1)) {
    printerror();
  }

  if (0 != xosd_scroll(osd, 1)) {
    printerror();
  }

  if (0 != xosd_destroy(osd)) {
    printerror();
  }

  return EXIT_SUCCESS;
}
