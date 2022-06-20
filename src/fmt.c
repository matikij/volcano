#include "fmt.h"

#define RED "\x1b[38;5;1m"
#define END "\x1b[0m"

static enum loglevel_t _loglevel = LOG_DEBUG;

void printerr(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "%s", RED);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "%s", END);
  va_end(args);
}

void print_libusb_err(int status) {
  printerr("Error: %s\n", libusb_error_name(status));
}

void print64(uint8_t *buf) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 16; j++) {
      printf("%02x ", buf[i*16+j]);
    }
    printf("\n");
  }
}

static const char *loglevelstr(enum loglevel_t level) {
  switch (level) {
    case LOG_ERROR:   return "ERROR";
    case LOG_WARNING: return "WARN ";
    case LOG_INFO:    return "INFO ";
    case LOG_DEBUG:   return "DEBUG";
  }
  return "NONE ";
}

void dlog(enum loglevel_t level, const char *fmt, ...) {
  if (level <= _loglevel) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    pid_t pid = getpid();

    va_list args;
    va_start(args, fmt);
    fprintf(
      stderr,
      "%d-%02d-%02d %02d:%02d:%02d %-6d %-10s ",
      tm.tm_year + 1900,
      tm.tm_mon+1,
      tm.tm_mday,
      tm.tm_hour,
      tm.tm_min,
      tm.tm_sec,
      pid,
      loglevelstr(level));
    vfprintf(stderr, fmt, args);
    va_end(args);
  }
}

void set_loglevel(enum loglevel_t level) {
  _loglevel = level;
}

bool strmatch(char *str, ...) {
  va_list args;
  va_start(args, str);
  const char *tok = va_arg(args, const char *);
  while (tok != NULL) {
    if (strcmp(str, tok) == 0) {
      va_end(args);
      return true;
    }
    tok = va_arg(args, const char *);
  }
  va_end(args);
  return false;
}

char **strsplit(char *str, size_t *outsz, const char *delim) {
  if (str == NULL) return NULL;

  *outsz = 0;
  char **out = NULL;

  char *tok = strtok(str, delim);
  while (tok != NULL) {
    char *elem = calloc(strlen(tok) + 1, sizeof (char));
    strcpy(elem, tok);
    *outsz = *outsz + 1;
    out = realloc(out, *outsz * sizeof (char *));
    out[*outsz-1] = elem;
    tok = strtok(NULL, delim);
  }

  return out;
}

void strsplit_free(char **sstr, size_t sstrsz) {
  for (int i = 0; i < sstrsz; i++) {
    free(sstr[i]);
  }
  free(sstr);
}
