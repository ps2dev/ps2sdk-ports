/*************************************************************************
 ** Sample1
 ** Load sample.cfg and increment the "X" setting
 *************************************************************************/

#include <stdio.h>
#include <libconfig.h>

struct config_t cfg;

/***************************************************************************/

int main(int argc, char **argv)
{
  char *file = "sample.cfg", *var = "x";

  if(argc >= 2)
    file = argv[1];

  if(argc >= 3)
    var = argv[2];

/* Initialize the configuration */
  config_init(&cfg);

  /* Load the file */
  printf("loading [%s]...", file);
  if(!config_read_file(&cfg, file))
    printf("failed\n");
  else
  {
    config_setting_t *setting = NULL;

    printf("ok\n");

    /* Get the variable setting from the configuration.. */
    printf("increment \"%s\"...", var);
    setting = config_lookup(&cfg, var);
    if(!setting)
      printf("failed\n");
    else
    {
      long x = config_setting_get_int(setting);
      x++;
      config_setting_set_int(setting, x);
      printf("ok (%s=%lu)\n", var, x);

      /* Save the changes */
      printf("saving [%s]...", file);
      config_write_file(&cfg, file);
      printf("ok\n");

      printf("Done!\n");
    }
  }

  /* Free the configuration */
  config_destroy(&cfg);

  return 0;
}

/***************************************************************************/
