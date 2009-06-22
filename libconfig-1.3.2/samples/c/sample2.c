/*************************************************************************
 ** Sample2
 ** Load sample.cfg and access the "values" array
 *************************************************************************/

#include <stdio.h>
#include <libconfig.h>

struct config_t cfg;

/***************************************************************************/

int main()
{
  /* Initialize the configuration */
  config_init(&cfg);

  /* Load the file */
  printf("loading [sample.cfg]..");
  if (!config_read_file(&cfg, "sample.cfg"))
    printf("failed\n");
  else
  {
    config_setting_t *array = NULL;
    
    printf("ok\n");
    
    /* Display the "values" array */
    printf("display \"values\"..");
    array = config_lookup(&cfg, "values");
    if (!array)
      printf("failed\n");
    else
    {
      long value1,value2;
      value1 = config_setting_get_int_elem(array, 0);
      value2 = config_setting_get_int_elem(array, 1);
      printf("[%lu %lu]..ok\n", value1, value2);

      printf("Done!\n");
    }

  }

  /* Free the configuration */
  config_destroy(&cfg);

  return 0;
}


/***************************************************************************/
