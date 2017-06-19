#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <json-glib/json-glib.h>

#include "config.h"

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
  GSList *list = NULL;

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
        list = g_slist_append(list, g_strdup(argvp[i]));

      g_strfreev(argvp);
    }
  }

  return list;
}

static GSList *get_flash_flags(const char *pepper_flash_dir) {
  GSList *list = NULL;
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
  list = g_slist_append(list,
                        g_strconcat("--ppapi-flash-version=", version, NULL));

  char *path = g_build_filename(pepper_flash_dir, "libpepflashplayer.so", NULL);
  list = g_slist_append(list, g_strconcat("--ppapi-flash-path=", path, NULL));
  free(path);

finish:
  free(manifest);
  g_object_unref(reader);
  g_object_unref(parser);
  return list;
}

static int launcher(int argc, char const *argv[]) {
  GSList *args = NULL;
  char *user_flags_conf_path = default_user_flags_conf_path();
  int i;

  args = g_slist_append(args, g_strdup(CHROMIUM_BINARY));
  args = g_slist_concat(args, get_flash_flags(PEPPER_FLASH_DIR));
  args = g_slist_concat(args, get_user_flags(user_flags_conf_path));
  free(user_flags_conf_path);

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
