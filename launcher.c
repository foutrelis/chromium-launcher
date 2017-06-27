#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <json-glib/json-glib.h>

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

static GSList *get_user_flags(const char *conf_path) {
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

  return flags;
}

static GSList *get_flash_flags(const char *pepper_flash_dir) {
  GSList *flags = NULL;
  JsonParser *parser = json_parser_new();
  JsonReader *reader = json_reader_new(NULL);

  char *manifest = g_build_filename(pepper_flash_dir, "manifest.json", NULL);
  if (!json_parser_load_from_file(parser, manifest, NULL))
    goto finish;

  json_reader_set_root(reader, json_parser_get_root(parser));
  json_reader_read_member(reader, "version");
  const char *version = json_reader_get_string_value(reader);
  json_reader_end_member(reader);

  if (!version || strcmp(version, "") == 0)
    goto finish;
  flags = g_slist_append(flags,
                         g_strconcat("--ppapi-flash-version=", version, NULL));

  char *path = g_build_filename(pepper_flash_dir, "libpepflashplayer.so", NULL);
  flags = g_slist_append(flags, g_strconcat("--ppapi-flash-path=", path, NULL));
  free(path);

finish:
  free(manifest);
  g_object_unref(reader);
  g_object_unref(parser);
  return flags;
}

static void show_help(const char *user_flags_conf_path, GSList *user_flags,
                      GSList *flash_flags) {
  int num_args, i;

  fprintf(
      stderr,
      "\n"
      "Chromium launcher %s -- for Chromium help, see %s(1)\n"
      "\n"
      "Custom flags are read from the following file:\n\n"
      "  %s\n\n"
      "Arguments contained in that file are split on whitespace and shell "
      "quoting\n"
      "rules apply but no further parsing is performed. Lines starting with a "
      "hash\n"
      "symbol (#) are skipped. Lines with unbalanced quotes are skipped as "
      "well.\n\n",
      LAUNCHER_VERSION, CHROMIUM_NAME, user_flags_conf_path);

  if ((num_args = g_slist_length(user_flags))) {
    fprintf(stderr, "Currently detected flags:\n\n");
    for (i = 0; i < num_args; i++)
      fprintf(stderr, "  %s\n", (char *)g_slist_nth_data(user_flags, i));
    fprintf(stderr, "\n");
  }

  if ((num_args = g_slist_length(flash_flags))) {
    fprintf(stderr, "Flags automatically added for PepperFlash support:\n\n");
    for (i = 0; i < num_args; i++)
      fprintf(stderr, "  %s\n", (char *)g_slist_nth_data(flash_flags, i));
    fprintf(stderr, "\n");
  }
}

static int launcher(int argc, char const *argv[]) {
  char *user_flags_conf_path = default_user_flags_conf_path();
  GSList *user_flags = get_user_flags(user_flags_conf_path);
  GSList *flash_flags = get_flash_flags(PEPPER_FLASH_DIR);
  GSList *args = NULL;
  int i;

  if (argc > 1 &&
      (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
    show_help(user_flags_conf_path, user_flags, flash_flags);
    return 0;
  }
  free(user_flags_conf_path);

  args = g_slist_append(args, g_strdup(CHROMIUM_BINARY));
  args = g_slist_concat(args, flash_flags);
  args = g_slist_concat(args, user_flags);

  for (i = 1; i < argc; i++)
    args = g_slist_append(args, g_strdup(argv[i]));

  int num_args = g_slist_length(args);
  char *exec_args[num_args + 1];

  for (i = 0; i < num_args; i++)
    exec_args[i] = g_strdup(g_slist_nth_data(args, i));
  exec_args[num_args] = NULL;
  g_slist_free_full(args, g_free);

  g_setenv("CHROME_WRAPPER", argv[0], 1);
  g_setenv("CHROME_DESKTOP", CHROMIUM_NAME ".desktop", 1);

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
