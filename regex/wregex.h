#ifndef _WREGEX_H
#define _WREGEX_H

#include <regex.h>
#include <stdlib.h>
#include <string.h>

/**
 * (c) Wanding Zhou
 * 
 * Example usage:
 *
 * wregex_t *wr = init_wregex("^(g\.)?([0-9]*)$", 3);
 * 
 * if (match_wregex(wr, "g.123456")) {
 *    unsigned g;
 *    for (g=0; g<wr->nmatch; ++g) {
 *      char *s = group_wregex(wr, g);
 *      printf("Group %u: %s\n", g, s);
 *      free(s);
 *    }
 *  } else {
 *    printf("Matching using wregex failed.\n");
 *  }
 * 
 * free_wregex(wr);
 *
 */

typedef struct wregex_t {
  regex_t regex;
  size_t nmatch;
  regmatch_t *pmatch;
  int flag;
  char *query;                  /* temporary query string */
} wregex_t;

static inline void free_wregex(wregex_t *wr) {
  regfree(&wr->regex);
  free(wr->pmatch);
  free(wr);
}

/**
 * return NULL if fail to compile */
static inline wregex_t *init_wregex(char *pattern, size_t nmatch) {
  wregex_t *wr = calloc(1, sizeof(wregex_t));
  wr->nmatch = nmatch;
  if (nmatch>0) wr->flag |= REG_EXTENDED;
  wr->pmatch = calloc(nmatch, sizeof(regmatch_t));
  if (regcomp(&wr->regex, pattern, wr->flag)==0)
    return wr;
  else {
    free_wregex(wr);
    return NULL;
  }
}

/**
 * if success return 1 else 0, more intuitive to me */
static inline int match_wregex(wregex_t *wr, char *query) {
  wr->query = query;
  return regexec(&wr->regex, query, wr->nmatch, wr->pmatch, 0)==0?1:0;
}

/**
 * get the i-th matched substring */
static inline char *group_wregex(wregex_t *wr, unsigned i) {
  regmatch_t *pm = wr->pmatch + i;
  if (pm->rm_so == (size_t)-1 || pm->rm_eo == pm->rm_so)
    return NULL;
  else {
    int l = pm->rm_eo - pm->rm_so;
    char *s = calloc(l + 1, sizeof(char));
    strncpy(s, wr->query + pm->rm_so, l);
    s[l] = '\0';
    return s;
  }
}

#endif /* _WREGEX_H */
