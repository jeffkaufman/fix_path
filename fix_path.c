
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern char** environ;

int starts_with(char* haystack, char* needle)
{
   return !strncmp(haystack, needle, strlen(needle));
}

int fail()
{
   printf("\\w");
   return 1;
}

/* sometimes there are characters before /home/ that can be completely ignored */
int fix_home(char* in_path)
{
   if (starts_with(in_path, "/d4m/home/"))
      return sizeof("/d4m") - 1;
   if (starts_with(in_path, "/nfs/emc5/home/"))
      return sizeof("/nfs/emc5") - 1;
   return 0;
}

int main(int argc, char** argv)
{
   char in_path_arr[2048];
   char out_path[2048];
   char cur_env_name[2048];
   char best_env_name[2048];
   int  best_env_index = -1;

   int  best_env_name_match_amt = 0;
   int index_of_equals;
   char* cur_env_value;

   int e_idx, idx_c;
   char* cur_e;
   char* in_path;
   

   if (!getcwd(in_path_arr, sizeof(in_path_arr)))
       return fail();

   in_path = in_path_arr + fix_home(in_path_arr);

   for(e_idx = 0; cur_e = environ[e_idx] ; e_idx++)
   {
      for(idx_c = 0 ;
          cur_e[idx_c] &&
             cur_e[idx_c] != '=' &&
             idx_c < sizeof(cur_env_name)-1;
          idx_c++)
         cur_env_name[idx_c] = cur_e[idx_c];
      cur_env_name[idx_c] = 0;

      if (cur_e[idx_c] != '=')
         continue;
      else
         index_of_equals = idx_c;

      cur_env_value = cur_e + index_of_equals + 1;


      if (cur_env_value[0] == '/' &&
          strlen(cur_env_value) > best_env_name_match_amt &&
          starts_with(in_path, cur_env_value) &&
          strcmp("PWD", cur_env_name) &&
          strcmp("OLDPWD", cur_env_name) &&
          strcmp("HOME", cur_env_name))
      {
         strncpy(best_env_name, cur_env_name, sizeof(best_env_name));
         best_env_name[sizeof(best_env_name)-1] = 0;

         best_env_index = e_idx;
         best_env_name_match_amt = strlen(cur_env_value);
      }
   }

   if (!best_env_name_match_amt ||
       !(strlen(best_env_name)+1<best_env_name_match_amt))
   {
      int off = 0;

      if (starts_with(in_path, "/home/jkaufman"))
         off = sizeof("/home/jkaufman");
      else if (starts_with(in_path, "/home/"))
         off = sizeof("/home/");

      if (off)
         printf("~%s", in_path + off - 1);
      else
         printf("%s", in_path);
   }
   else
      printf("$%s%s", best_env_name, in_path +
             best_env_name_match_amt);
   return 0;
}
