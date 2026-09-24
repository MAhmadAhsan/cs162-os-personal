/*

  Word Count using dedicated lists

*/

/*
Copyright © 2019 University of California, Berkeley

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <assert.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#include "word_count.h"

/* Global data structure tracking the words encountered */
WordCount *word_counts = NULL;

/* The maximum length of each word in a file */
#define MAX_WORD_LEN 64

/*
 * 3.1.1 Total Word Count
 *
 * Returns the total amount of words found in infile.
 * Useful functions: fgetc(), isalpha().
 */
int num_words(FILE* infile) {
  if(infile == NULL)
  {
    return -1;
  }

  int num_words = 0;

  int ch;
  int count = 0;
  while((ch = fgetc(infile)) != EOF)
  {
    if(isalpha(ch))
    {
      count += 1;
    }
    else
    {
      if(count > 1)
      {
        num_words += 1;
      }
      count = 0;
    }
  }

  if(count > 1)
  {
    num_words += 1;
  }

  return num_words;
}

/*
 * 3.1.2 Word Frequency Count
 *
 * Given infile, extracts and adds each word in the FILE to `wclist`.
 * Useful functions: fgetc(), isalpha(), tolower(), add_word().
 * 
 * As mentioned in the spec, your code should not panic or
 * segfault on errors. Thus, this function should return
 * 1 in the event of any errors (e.g. wclist or infile is NULL)
 * and 0 otherwise.
 */
int count_words(WordCount **wclist, FILE *infile) {
  if (wclist == NULL || infile == NULL) {
    return 1;
  }

  int n = 0;
  char chararr[MAX_WORD_LEN + 1];
  int ch;

  while ((ch = fgetc(infile)) != EOF) {
    if (isalpha((unsigned char)ch)) {
      if (n < MAX_WORD_LEN) {
        chararr[n] = tolower((unsigned char)ch);
        n += 1;
      }
    } else {
      if (n > 0) {
        chararr[n] = '\0';
        int s = add_word(wclist, chararr);
        if (s != 0) {
          return 1;
        }
      }
      n = 0;
    }
  }

  if (n > 0) {
    chararr[n] = '\0';
    int s = add_word(wclist, chararr);
    if (s != 0) {
      return 1;
    }
  }

  return 0;
}

/*
 * Comparator to sort list by frequency.
 * Useful function: strcmp().
 */
static bool wordcount_less(const WordCount *wc1, const WordCount *wc2) {
  if(wc1->count < wc2->count)
  {
    return true;
  }
  else if(wc1->count > wc2->count)
  {
    return false;
  }
  else
  {
    return strcmp(wc1->word, wc2->word) < 0 ? true : false;
  }

  return 0;
}

// In trying times, displays a helpful message.
static int display_help(void) {
	printf("Flags:\n"
	    "--count (-c): Count the total amount of words in the file, or STDIN if a file is not specified. This is default behavior if no flag is specified.\n"
	    "--frequency (-f): Count the frequency of each word in the file, or STDIN if a file is not specified.\n"
	    "--help (-h): Displays this help message.\n");
	return 0;
}

typedef enum {
  COUNT,
  FREQUENCY,
  HELP
} Mode;

/*
 * Parse the arguments and return the mode
 */

Mode get_mode(int argc, char *argv[])
{
  static struct option long_options[] =
  {
      {"count", no_argument, 0, 'c'},
      {"frequency", no_argument, 0, 'f'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}
  };
  
  Mode mode = COUNT;
  // Variables for command line argument parsing
  int i;
  // Sets flags
  while ((i = getopt_long(argc, argv, "cfh", long_options, NULL)) != -1) {
      switch (i) {
          case 'c':
              mode = COUNT;
              break;
          case 'f':
              mode = FREQUENCY;
              break;
          case 'h':
              mode = HELP;
      }
  }
  return mode;
}


/*
 * Handle command line flags and arguments.
 */
int main(int argc, char *argv[]) {
  Mode mode = get_mode(argc, argv);

  FILE *infiles[100];
  int n;

  if (argc - optind < 1)
  {
      infiles[0] = stdin;
      n = 1;
  }
  else
  {
      n = argc - optind;

      for (int i = 0; i < n; i++) {
          infiles[i] = fopen(argv[optind + i], "r");

          if (infiles[i] == NULL) {
              return 1;
          }
      }
  }

  int total_count = 0;

  switch (mode) {
    case COUNT:
        // count words
        for(int i = 0; i < n; i++)
        {
          int count = num_words(infiles[i]);
          if(count == -1)
          {
            return 1;
          }
          total_count += count;
        }
        printf("The total number of words is: %i\n", total_count);
        break;

    case FREQUENCY:
        // calculate frequency
        init_words(&word_counts);
        for(int i = 0; i < n; i++)
        {
          count_words(&word_counts, infiles[i]);
        }

        wordcount_sort(&word_counts, wordcount_less);

        printf("The frequencies of each word are: \n");
        fprint_words(word_counts, stdout);
        break;
    case HELP:
        printf("Please specify a mode.\n");
        return display_help();
  }

  for(int i = 0; i < n; i++)
  {
    fclose(infiles[i]);
  }
  return 0;
}
