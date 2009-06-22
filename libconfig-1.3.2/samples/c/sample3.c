/*************************************************************************
 ** Sample3
 ** Load sample.cfg and try to add a setting "foo"..
 **   on success save to testfoo.cfg
 *************************************************************************/

#include <stdio.h>
#include <libconfig.h>

/***************************************************************************/

/*
 */

int print_path(config_setting_t *setting)
{
  if(setting)
  {
    const char *name = config_setting_name(setting);
    
    if(print_path(config_setting_parent(setting)))
      putchar('.');

    if(! config_setting_is_root(setting))
    {
      if(! name)
        printf("[%d]", config_setting_index(setting));
      else
        printf(name);

      return(1);
    }
  }

  return(0);
}

/*
 */

int main()
{
  struct config_t cfg;
  config_setting_t *setting = NULL;
  int i;
  
  do
  {
    /* Initialize the configuration */
    config_init(&cfg);
    
    /* Load the file */
    printf("loading [sample.cfg]...");
    if (!config_read_file(&cfg, "sample.cfg"))
    {
      puts("failed");
      break;
    }
    
    puts("ok");

    /* Add setting "foo" */
    printf("add setting \"foo\"...");
    setting = config_setting_add(cfg.root, "foo", CONFIG_TYPE_INT);
    if (!setting)
    {
      puts("failed");
      break;
    }
    
    config_setting_set_int(setting, 1234);
    puts("ok");

    /** Look up an array element */
    printf("looking up array element...");
    setting = config_lookup(&cfg, "arrays.values.[0]");
    if(! setting)
    {
      puts("failed");
      break;
    }

    printf("value is: %d\n", config_setting_get_int(setting));
    printf("path is: ");
    print_path(setting);
    putchar('\n');
    
    /* Save to "samplefoo.cfg" */
    printf("saving [samplefoo.cfg]...");
    config_write_file(&cfg, "samplefoo.cfg");
    puts("ok");
    
    puts("Done!");
  }
  while(0);

  /* Free the configuration */
  config_destroy(&cfg);

  return 0;
}


/***************************************************************************/
