#ifdef _MSC_VER
#pragma warning (disable: 4996)
#endif

#include <stdio.h>
#include <stdlib.h>
#include "libconfig.h"

int main(int argc, char **argv)
{
  config_t cfg;
  FILE *fp;
  int i, x;
  const char *s;
  const config_setting_t *array;
  config_setting_t *old;

  fp = fopen("test.cfg", "rb");

  if(! fp)
  {
    printf("Unable to open test.cfg\n");
    exit(1);
  }
  
  config_init(&cfg);
  x = config_read(&cfg, fp);

  fclose(fp);

  if(! x)
  {
    printf("error on line %d: %s\n", cfg.error_line, cfg.error_text);
    exit(1);
  }

  if(! config_lookup_string(&cfg, "application.group1.group2.zzz", &s))
    puts("NOT FOUND");
  else
    printf("found: %s\n", s);

  array = config_lookup(&cfg, "application.group1.my_array");
  if(array)
  {
    for(i = 0; i < config_setting_length(array); i++)
    {
      long val = config_setting_get_int_elem(array, i);
      printf("value [%i]: %ld\n", i, val);
    }
  }

  {
    config_setting_t *root = config_root_setting(&cfg);

    config_setting_t *new = config_setting_add(root, "foobar",
                                               CONFIG_TYPE_LIST);

    config_setting_t *newgrp, *newval;
    

    if(! config_setting_set_float_elem(new, -1, 3.141592654))
      puts("FAILED");
    
    if(! config_setting_set_string_elem(new, -1, "Hello, world!"))
      puts("FAILED");

    newgrp = config_setting_add(new, NULL, CONFIG_TYPE_GROUP);
    
    newval = config_setting_add(newgrp, "baz", CONFIG_TYPE_INT);

    old = config_lookup(&cfg, "application.group1");

    printf("old is: %p\n", old);
    config_setting_remove(old, "group2");
  }

  config_write(&cfg, stdout);

  
  config_destroy(&cfg);

  
  return(0);
}
