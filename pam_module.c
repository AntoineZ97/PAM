/*
** pam_module.c for pamela in /home/zaczyk_a/BoxTest
** 
** Made by Antoine Zaczyk
** Login   <antoine.zaczyk@epitech.eu>
** 
** Started on  Fri Nov 23 21:00:21 2017 Antoine Zaczyk
** Last update Wed Nov 29 13:34:26 2017 Antoine Zaczyk
*/

#define PAM_SM_AUTH
#define	BUFSIZE		1024

#include <security/pam_modules.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

PAM_EXTERN int	pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  int		retval;
  const void *item = NULL;
  const char	*pass;
  const char	*pUsername;

  if ((retval = pam_get_user(pamh, &pUsername, "Username: ")) != PAM_SUCCESS)
    return (retval);
  pam_get_item(pamh, PAM_AUTHTOK, &item);
  pass = (const char *)item;
  pam_set_data(pamh, "pam_module_pass", strdup(pass), NULL);
  return (PAM_SUCCESS);
}

PAM_EXTERN int	pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  const char  *pass;
  const char  *user;
  char  cmd[BUFSIZE];
  int	retval;
  const void *item;

  if ((retval = pam_get_user(pamh, (const char **)&user, "Username: ")) != PAM_SUCCESS)
    return (retval);
  if ((retval = pam_get_data(pamh, "pam_module_pass", &item)) != PAM_SUCCESS)
    return (retval);
  pass = (const char *)item;
  if (pass == NULL)
    sprintf(cmd, "sudo cryptsetup luksOpen /home/%s/encrypted crypt_%s && sudo mount /dev/mapper/crypt_%s /home/%s/secure_data-rw", user, user, user, user);
  else
    {
      sprintf(cmd, "echo \"%s\" | sudo cryptsetup luksOpen /home/%s/encrypted crypt_%s && sudo mount /dev/mapper/crypt_%s /home/%s/secure_data-rw", pass, user, user, user, user);
      printf("Mot de passe %s\n", pass);
    }
  system(cmd);
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  char	*user;
  char	cmd[BUFSIZE];
  int	retval;

  if ((retval = pam_get_user(pamh, (const char **)&user, "Username: ")) != PAM_SUCCESS)
    return (retval);
  sprintf(cmd, "sudo umount /dev/mapper/crypt_%s ; sudo cryptsetup luksClose crypt_%s", user, user);
  system(cmd);
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  const char	*user;
  const void	*item = NULL;
  const char	*pass;
  const char	*newPass;
  char cmd[BUFSIZE];
  int		retval;

  if ((retval = pam_get_user(pamh, (const char **)&user, "Username: ")) != PAM_SUCCESS)
    return (retval);
  if ((retval = pam_get_item(pamh, PAM_AUTHTOK, &item)) != PAM_SUCCESS)
    {
      return (retval);
    }
  newPass = (const char *)item;
  if (newPass != NULL)
    {
      sprintf(cmd, "sudo cryptsetup luksAddKey /home/%s/encrypted", user);
      system(cmd);
    }
  return (PAM_SUCCESS);
}
