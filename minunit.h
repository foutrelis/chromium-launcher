#include <stdio.h>

#define MU_ERR_LEN 1024

#define mu_assert(test, ...)                                                   \
  do {                                                                         \
    if (!(test)) {                                                             \
      int size = snprintf(mu_err, MU_ERR_LEN, "%s:%d (%s) ", __FILE__,         \
                          __LINE__, #test);                                    \
      if (size > 0 && size < MU_ERR_LEN) {                                     \
        size = snprintf(mu_err + size, MU_ERR_LEN - size, __VA_ARGS__);        \
        if (size > 0 && size < MU_ERR_LEN) {                                   \
          return mu_err;                                                       \
        }                                                                      \
      }                                                                        \
      mu_err[MU_ERR_LEN - 1] = 0;                                              \
      return mu_err;                                                           \
    }                                                                          \
  } while (0)

#define mu_run_test(test)                                                      \
  do {                                                                         \
    char *message = test();                                                    \
    tests_run++;                                                               \
    if (message)                                                               \
      return message;                                                          \
  } while (0)

int tests_run = 0;
char mu_err[MU_ERR_LEN] = {0};
