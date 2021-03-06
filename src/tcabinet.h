/**
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef TCABINET_H_INCLUDED
#define TCABINET_H_INCLUDED

#include <tcadb.h>

#include "commons.h"
#include "gstorage.h"
#include "parser.h"

#define TC_MMAP  0
#define TC_LCNUM 1024
#define TC_NCNUM 512
#define TC_LMEMB 128
#define TC_NMEMB 256
#define TC_BNUM  32749
#define TC_DBPATH "/tmp/"
#define TC_ZLIB 1
#define TC_BZ2  2

#define DB_PARAMS 256

/* B+ Tree - on-disk databases */
#define DB_GEN_STATS "db_gen_stats.tcb"
#define DB_HOST_AGENTS "db_host_agents.tcb"
#define DB_HOSTNAMES "db_hostnames.tcb"
#define DB_UNIQUE_KEYS "db_unique_keys.tcb"

#define DB_KEYMAP    "db_keymap.tcb"
#define DB_DATAMAP   "db_datamap.tcb"
#define DB_ROOTMAP   "db_rootmap.tcb"
#define DB_UNIQMAP   "db_uniqmap.tcb"
#define DB_VISITORS  "db_visitors.tcb"
#define DB_HITS      "db_hits.tcb"
#define DB_BW        "db_bw.tcb"
#define DB_AVGTS     "db_avgts.tcb"
#define DB_METHODS   "db_methods.tcb"
#define DB_PROTOCOLS "db_protocols.tcb"

#define INT_TO_POINTER(i) ((void *) (long) (i))

/* tables for the whole app */
extern TCADB *ht_general_stats;
extern TCADB *ht_hostnames;
extern TCADB *ht_hosts_agents;
extern TCADB *ht_unique_keys;

/* *INDENT-OFF* */

uint32_t get_ht_size_by_metric (GModule module, GMetric metric);
uint32_t get_ht_size (TCADB *adb);

int ht_insert_agent (const char *key);
int ht_insert_cumulative (TCADB *adb, int data_nkey, uint64_t size);
int ht_insert_hit (TCADB *adb, int data_nkey, int uniq_nkey, int root_nkey);
int ht_insert_keymap (TCADB * adb, const char *value);
int ht_insert_nkey_nval (TCADB * adb, int nkey, int nval);
int ht_insert_nodemap (TCADB *adb, int nkey, const char *value);
int ht_insert_num (TCADB *adb, int data_nkey);
int ht_insert_uniqmap (TCADB *adb, char *uniq_key);
int ht_insert_unique_key (const char *key);

char *get_hostname (const char *host);
char *get_node_from_key (int data_nkey, GModule module, GMetric metric);
char *get_root_from_key (int root_nkey, GModule module);
int get_num_from_key (int data_nkey, GModule module, GMetric metric);
uint64_t get_cumulative_from_key (int data_nkey, GModule module, GMetric metric);

GRawData *parse_raw_data (void *db, int ht_size, GModule module);
void free_db_key (TCADB *adb);
void free_storage (void);
void init_storage (void);

/* *INDENT-ON* */

#endif
