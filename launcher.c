#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

static char *default_user_flags_conf_path() {
  const char *xdg_config_home = getenv("XDG_CONFIG_HOME"),
             *home = getenv("HOME");
  char *path = NULL;

  if (xdg_config_home)
    path = g_build_filename(xdg_config_home, CHROMIUM_NAME "-flags.conf", NULL);
  else if (home)
    path = g_build_filename(home, ".config", CHROMIUM_NAME "-flags.conf", NULL);

  return path;
}

static char *default_system_flags_conf_path() {
  char *path = g_build_filename("/etc/", CHROMIUM_NAME "-flags.conf", NULL);

  return path;
}

static GSList *get_flags(const char *conf_path) {
  GSList *flags = NULL;

  if (!conf_path)
    return NULL;

  FILE *file = fopen(conf_path, "r");
  if (!file)
    return NULL;

  {
    char buf[LINE_MAX];
    int argcp;
    char **argvp;
    int i;

    while (fgets(buf, sizeof buf, file)) {
      if (!g_shell_parse_argv(buf, &argcp, &argvp, NULL))
        continue;

      for (i = 0; i < argcp; i++)
        flags = g_slist_append(flags, g_strdup(argvp[i]));

      g_strfreev(argvp);
    }
  }

  fclose(file);
  return flags;
}

static void show_help(const char *user_flags_conf_path, const char *system_flags_conf_path, GSList *flags) {
  int num_args, i;

  fprintf(
      stderr,
      "\n"
      "Chromium launcher %s -- for Chromium help, see `man %s`\n"
      "\n"
      "Custom flags are read from the following file:\n\n"
      "  %s\n  %s\n\n"
      "Arguments contained in that file are split on whitespace and shell "
      "quoting\n"
      "rules apply but no further parsing is performed. Lines starting with a "
      "hash\n"
      "symbol (#) are skipped. Lines with unbalanced quotes are skipped as "
      "well.\n\n",
      LAUNCHER_VERSION, CHROMIUM_NAME, user_flags_conf_path, system_flags_conf_path);

  if ((num_args = g_slist_length(flags))) {
    fprintf(stderr, "Currently detected flags:\n\n");
    for (i = 0; i < num_args; i++)
      fprintf(stderr, "  %s\n", (char *)g_slist_nth_data(flags, i));
    fprintf(stderr, "\n");
  }
}

static int launcher(int argc, char const *argv[]) {
  char *user_flags_conf_path = default_user_flags_conf_path();
  char *system_flags_conf_path = default_system_flags_conf_path();
  GSList *flags = get_flags(user_flags_conf_path);
  g_slist_append(flags, get_flags(system_flags_conf_path));
  GSList *args = NULL;
  int i;
  
  if (argc > 1 &&
      (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
    show_help(user_flags_conf_path, system_flags_conf_path, flags);
    return 0;
  }
  free(user_flags_conf_path);
  free(system_flags_conf_path);

  args = g_slist_append(args, g_strdup(CHROMIUM_BINARY));
  args = g_slist_concat(args, flags);

  for (i = 1; i < argc; i++)
    args = g_slist_append(args, g_strdup(argv[i]));

  int num_args = g_slist_length(args);
  char *exec_args[num_args + 1];

  for (i = 0; i < num_args; i++)
    exec_args[i] = g_strdup(g_slist_nth_data(args, i));
  exec_args[num_args] = NULL;
  g_slist_free_full(args, g_free);

  g_setenv("CHROME_WRAPPER", argv[0], TRUE);
  g_setenv("CHROME_DESKTOP", CHROMIUM_NAME ".desktop", TRUE);
  g_setenv("CHROME_VERSION_EXTRA", CHROMIUM_VENDOR, TRUE);

  return execv(CHROMIUM_BINARY, exec_args);
}

#ifndef TESTING
int main(int argc, char const *argv[]) {
#if !GLIB_CHECK_VERSION(2, 36, 0)
  g_type_init();
#endif
  return launcher(argc, argv);
}
#endif
