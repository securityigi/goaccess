/**
 * output.c -- output csv to the standard output stream
 * Copyright (C) 2009-2014 by Gerardo Orellana <goaccess@prosoftcorp.com>
 * GoAccess - An Ncurses apache weblog analyzer & interactive viewer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * A copy of the GNU General Public License is attached to this
 * source distribution for its full text.
 *
 * Visit http://goaccess.prosoftcorp.com for new releases.
 */

#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>

#include "csv.h"

#ifdef HAVE_LIBTOKYOCABINET
#include "tcabinet.h"
#else
#include "glibht.h"
#endif

#include "commons.h"
#include "error.h"
#include "gdashboard.h"
#include "gstorage.h"
#include "ui.h"
#include "util.h"
#include "xmalloc.h"

static void print_csv_data (FILE * fp, GHolder * h, int processed);

static GCSV paneling[] = {
  {VISITORS, print_csv_data},
  {REQUESTS, print_csv_data},
  {REQUESTS_STATIC, print_csv_data},
  {NOT_FOUND, print_csv_data},
  {HOSTS, print_csv_data},
  {BROWSERS, print_csv_data},
  {OS, print_csv_data},
  {REFERRERS, print_csv_data},
  {REFERRING_SITES, print_csv_data},
  {KEYPHRASES, print_csv_data},
#ifdef HAVE_LIBGEOIP
  {GEO_LOCATION, print_csv_data},
#endif
  {STATUS_CODES, print_csv_data},
};

static GCSV *
panel_lookup (GModule module)
{
  int i, num_panels = ARRAY_SIZE (paneling);

  for (i = 0; i < num_panels; i++) {
    if (paneling[i].module == module)
      return &paneling[i];
  }
  return NULL;
}

static void
escape_cvs_output (FILE * fp, char *s)
{
  while (*s) {
    switch (*s) {
    case '"':
      fprintf (fp, "\"\"");
      break;
    default:
      fputc (*s, fp);
      break;
    }
    s++;
  }
}

static void
print_csv_sub_items (FILE * fp, GHolder * h, int idx, int processed)
{
  GSubList *sub_list = h->items[idx].sub_list;
  GSubItem *iter;
  float percent;
  int i = 0;

  if (sub_list == NULL)
    return;

  for (iter = sub_list->head; iter; iter = iter->next, i++) {
    percent = get_percentage (processed, iter->metrics->hits);
    percent = percent < 0 ? 0 : percent;

    fprintf (fp, "\"%d\",", i); /* idx */
    fprintf (fp, "\"%d\",", idx);       /* parent idx */
    fprintf (fp, "\"%s\",", module_to_id (h->module));
    fprintf (fp, "\"%d\",", iter->metrics->hits);
    fprintf (fp, "\"%d\",", iter->metrics->visitors);
    fprintf (fp, "\"%4.2f%%\",", percent);
    fprintf (fp, "\"%ld\",", iter->metrics->bw.nbw);

    if (conf.serve_usecs)
      fprintf (fp, "\"%ld\"", iter->metrics->avgts.nts);
    fprintf (fp, ",");

    if (conf.append_method && iter->metrics->method)
      fprintf (fp, "\"%s\"", iter->metrics->method);
    fprintf (fp, ",");

    if (conf.append_protocol && iter->metrics->protocol)
      fprintf (fp, "\"%s\"", iter->metrics->protocol);
    fprintf (fp, ",");

    fprintf (fp, "\"");
    escape_cvs_output (fp, iter->metrics->data);
    fprintf (fp, "\",");
    fprintf (fp, "\r\n");       /* parent idx */
  }
}

/* generate CSV unique visitors stats */
static void
print_csv_data (FILE * fp, GHolder * h, int processed)
{
  GMetrics *nmetrics;
  int i;

  for (i = 0; i < h->idx; i++) {
    set_data_metrics (h->items[i].metrics, &nmetrics, processed);

    fprintf (fp, "\"%d\",", i); /* idx */
    fprintf (fp, ",");  /* no parent */
    fprintf (fp, "\"%s\",", module_to_id (h->module));
    fprintf (fp, "\"%d\",", nmetrics->hits);
    fprintf (fp, "\"%d\",", nmetrics->visitors);
    fprintf (fp, "\"%4.2f%%\",", nmetrics->percent);
    fprintf (fp, "\"%ld\",", nmetrics->bw.nbw);

    if (conf.serve_usecs)
      fprintf (fp, "\"%ld\"", nmetrics->avgts.nts);
    fprintf (fp, ",");

    if (conf.append_method && nmetrics->method)
      fprintf (fp, "\"%s\"", nmetrics->method);
    fprintf (fp, ",");

    if (conf.append_protocol && nmetrics->protocol)
      fprintf (fp, "\"%s\"", nmetrics->protocol);
    fprintf (fp, ",");

    fprintf (fp, "\"");
    escape_cvs_output (fp, nmetrics->data);
    fprintf (fp, "\"\r\n");

    if (h->sub_items_size)
      print_csv_sub_items (fp, h, i, processed);

    free (nmetrics);
  }
}

/* entry point to generate a a csv report writing it to the fp */
void
output_csv (GLog * logger, GHolder * holder)
{
  GModule module;
  FILE *fp = stdout;

  for (module = 0; module < TOTAL_MODULES; module++) {
    const GCSV *panel = panel_lookup (module);
    if (!panel)
      continue;
    panel->render (fp, holder + module, logger->process);
  }

  fclose (fp);
}
