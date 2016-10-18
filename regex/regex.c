#include <regex.h>
#include <stdio.h>
#include "wregex.h"

/*
 * Match string against the extended regular expression in
 * pattern, treating errors as no match.
 *
 * Return 1 for match, 0 for no match.
 */
int
match(const char *string, char *pattern)
{
  int status;
  regex_t re;

  /* REG_EXTENDED don't escape capturing parentheses
     but do escape literal parenthesis
     
     REG_NOSUB: nmatch and pmatch are ignored in regexec()
  */

  if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
    return(0);      /* Report error. */
  }
  status = regexec(&re, string, (size_t) 0, NULL, 0);
  regfree(&re);
  if (status != 0) {
    return(0);      /* Report error. */
  }
  return(1);
}


/**
   test examples:
   $ ./a.out '^(g\.)?([0-9]*)$' 'g.123456'

   nmatch: 3
   Group 0: [ 0- 8]: g.123456
   Group 1: [ 0- 2]: g.
   Group 2: [ 2- 8]: 123456


   $ ./a.out '^(g\.)?([[:digit:]]*)$' 'g.123456'
   # the same as above

   $ ./a.out '^(g\.)?([[:digit:]]+)(_([[:digit:]]+))?$' 'g.123456_3432'
   nmatch: 10
   Group 0: [ 0-13]: g.123456_3432
   Group 1: [ 0- 2]: g.
   Group 2: [ 2- 8]: 123456
   Group 3: [ 8-13]: _3432
   Group 4: [ 9-13]: 3432

   $ ./a.out '^(g\.)?([[:digit:]]+)(_([[:digit:]]+))?(\.)?(del([atgcnATGCN[:digit:]]*))?(ins([atgcnATGCN]*))?(([atgcnATGCN?]*)>([atgcnATGCN?]*))?(dup([atgcATGCN[:digit:]]*))?$' 'g.123456_3432delATCinsAA'

   nmatch: 20
   Group 0: [ 0-24]: g.123456_3432delATCinsAA
   Group 1: [ 0- 2]: g.
   Group 2: [ 2- 8]: 123456
   Group 3: [ 8-13]: _3432
   Group 4: [ 9-13]: 3432
   Group 6: [13-19]: delATC
   Group 7: [16-19]: ATC
   Group 8: [19-24]: insAA
   Group 9: [22-24]: AA

   $ ./a.out '^(g\.)?([[:digit:]]+)(_([[:digit:]]+))?(\.)?(del([atgcnATGCN[:digit:]]*))?(ins([atgcnATGCN]*))?(([atgcnATGCN?]*)>([atgcnATGCN?]*))?(dup([atgcATGCN[:digit:]]*))?$' 'g.123456_3432dup'
   nmatch: 20
   Group 0: [ 0-16]: g.123456_3432dup
   Group 1: [ 0- 2]: g.
   Group 2: [ 2- 8]: 123456
   Group 3: [ 8-13]: _3432
   Group 4: [ 9-13]: 3432
   Group 13: [13-16]: dup
   Group 14: [16-16]:

  */

/* (g\.)?(\d+)(_(\d+))?(\.)?(del([atgcnATGCN\d]*))?(ins([atgcnATGCN]*))?(([atgcnATGCN?]*)>([atgcnATGCN?]*))?(dup([atgcnATGCN\d]*))?$ */
/* http://pubs.opengroup.org/onlinepubs/009695399/basedefs/xbd_chap09.html */
int main(int argc, char *argv[])
{

  if (argc > 1) {
    regex_t re;
    /* note we need to double escape \\. to avoid warning from C compiler */
    /*  */
    if (regcomp(&re, argv[1], REG_EXTENDED)) {
      printf("Couldn't compile regular expression.\n");
    }

    size_t nmatch = 20;
    regmatch_t pmatch[nmatch];
    if (regexec(&re, argv[2], nmatch, pmatch, 0)==0) {
      printf("nmatch: %zu\n", nmatch);
      unsigned int g = 0;
      for (g=0; g<nmatch; ++g) {
        /* begin of the match is .rm_so
           end of the match is .rm_eo (1 past the last character of the substring */
        if (pmatch[g].rm_so == (size_t)-1) /* .rm_so == (size_t)-1 means unmatched group */
          continue;// no more groups
        printf("Group %u: [%2lld-%2lld]: %.*s\n", g, pmatch[g].rm_so, pmatch[g].rm_eo,
               (int) (pmatch[g].rm_eo - pmatch[g].rm_so), argv[2] + pmatch[g].rm_so);
      }
    } else {
      printf("matching failed\n");
    }
    regfree(&re);

    /* use wregex.h */
    wregex_t *wr = init_wregex(argv[1], 20);
    if (match_wregex(wr, argv[2])) {
      unsigned g;
      for (g=0; g<wr->nmatch; ++g) {
        char *s = group_wregex(wr, g);
        printf("Group %u: %s\n", g, );
        free(s);
      }
    } else {
      printf("Matching using wregex failed.\n");
    }

    free_wregex(wr);
  }
  return 0;
}
