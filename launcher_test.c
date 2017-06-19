#include <stdio.h>
#include <string.h>

#include <glib.h>

#include "minunit.h"

static int exec_count = 0;
static const char *exec_path;
static char *const *exec_argv;

int execv(const char *path, char *const argv[]) {
  exec_count++;
  exec_path = g_strdup(path);
  exec_argv = g_strdupv((char **)argv);
  return 0;
}

#ifdef PEPPER_FLASH_DIR
#undef PEPPER_FLASH_DIR
#endif
#define PEPPER_FLASH_DIR "testdata"

#define TESTING
#include "launcher.c"

static char *test_default_user_flags_conf_path() {
  const char *home = getenv("HOME");
  char *path, *expected;

  g_unsetenv("XDG_CONFIG_HOME");
  path = default_user_flags_conf_path();
  expected =
      g_build_filename(home, ".config", CHROMIUM_NAME "-flags.conf", NULL);
  mu_assert(strcmp(path, expected) == 0, "path = %s (wanted: %s)", path,
            expected);
  free(expected);
  free(path);

  g_setenv("XDG_CONFIG_HOME", "testdata", 1);
  path = default_user_flags_conf_path();
  expected = g_build_filename("testdata", CHROMIUM_NAME "-flags.conf", NULL);
  mu_assert(strcmp(path, expected) == 0, "path = %s (wanted: %s)", path,
            expected);
  free(expected);
  free(path);

  return NULL;
}

static char *test_get_user_flags() {
  char *expected[] = {"--if", "--it", "--builds", "--it --ships", NULL};
  GSList *args = get_user_flags("testdata/chromium-flags.conf");
  int i, length;

  for (i = 0; expected[i]; i++) {
    char *arg = g_slist_nth_data(args, i);
    mu_assert(arg, "end of list (wanted: \"%s\")", expected[i]);
    mu_assert(strcmp(arg, expected[i]) == 0, "arg = \"%s\" (wanted: \"%s\")",
              arg, expected[i]);
  }

  length = g_slist_length(args);
  mu_assert(length == i, "got %d args, expected %d", length, i);
  g_slist_free_full(args, g_free);

  length = g_slist_length(get_user_flags("fakedata/chromium-flags.conf"));
  mu_assert(length == 0, "got %d args, expected 0", length);

  length = g_slist_length(NULL);
  mu_assert(length == 0, "got %d args, expected 0", length);

  return NULL;
}

static char *test_get_flash_flags() {
  char *expected[] = {"--ppapi-flash-version=4.5.6",
                      "--ppapi-flash-path=testdata/libpepflashplayer.so", NULL};
  GSList *args = get_flash_flags("testdata");
  int i, length;

  for (i = 0; expected[i]; i++) {
    char *arg = g_slist_nth_data(args, i);
    mu_assert(arg, "end of list (wanted: \"%s\")", expected[i]);
    mu_assert(strcmp(arg, expected[i]) == 0, "arg = \"%s\" (wanted: \"%s\")",
              arg, expected[i]);
  }

  length = g_slist_length(args);
  mu_assert(length == i, "got %d args, expected %d", length, i);
  g_slist_free_full(args, g_free);

  length = g_slist_length(get_flash_flags("fakedata"));
  mu_assert(length == 0, "got %d args, expected 0", length);

  return NULL;
}

static char *test_launcher() {
  char *expected[] = {CHROMIUM_BINARY,
                      "--ppapi-flash-version=4.5.6",
                      "--ppapi-flash-path=testdata/libpepflashplayer.so",
                      "--if",
                      "--it",
                      "--builds",
                      "--it --ships",
                      "--test-arg",
                      NULL};
  const char *argv[] = {"launcher()", "--test-arg", NULL};
  int ret, i;

  g_setenv("XDG_CONFIG_HOME", "testdata", 1);
  ret = launcher((sizeof argv / sizeof argv[0]) - 1, argv);

  mu_assert(ret == 0, "launcher() returned %d, expected 0", ret);
  mu_assert(exec_count == 1, "exec_count = %d, expected 1", exec_count);

  for (i = 0; expected[i]; i++) {
    mu_assert(exec_argv[i], "end of list (wanted: \"%s\")", expected[i]);
    mu_assert(strcmp(exec_argv[i], expected[i]) == 0,
              "exec_argv[i] = \"%s\" (wanted: \"%s\")", exec_argv[i],
              expected[i]);
  }

  return NULL;
}

static char *all_tests() {
  mu_run_test(test_default_user_flags_conf_path);
  mu_run_test(test_get_user_flags);
  mu_run_test(test_get_flash_flags);
  mu_run_test(test_launcher);

  return NULL;
}

int main() {
#if !GLIB_CHECK_VERSION(2, 36, 0)
  g_type_init();
#endif

  char *result = all_tests();
  if (result)
    fprintf(stderr, "%s\n", result);
  else
    fprintf(stderr, "ALL TESTS PASSED\n");

  fprintf(stderr, "Tests run: %d\n", tests_run);

  return result != NULL;
}
