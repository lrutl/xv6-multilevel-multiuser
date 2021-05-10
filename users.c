//< +--------------------------------------------------------------------------------------------------+ >
//| Project: xv6-multiuser
//| File: users.c
//| Description: Provides user-level functions for user management
//| Date: 10 May 2021
//< +--------------------------------------------------------------------------------------------------+ >

#include "types.h"
#include "user.h"
#include "users.h"
#include "fcntl.h"

int get_user(struct user *to_get, char username[])
{
	char path[50];
	strcpy(path, "/etc/users/");
	strcat(path, username);

	int file = open(path, O_RDONLY);

	if(file < 0)
	{
		printf(1, "user was not found in /etc/users/ \n");
		return -1;
	}

	int len = sizeof(*to_get);
	if(read(file, to_get, len) != len)
	{
		printf(1, "failed to get user\n");
		return -1;
	}

	return 0;
}

int save_user(struct user *to_save)
{
	mkdir("/etc/");
	mkdir("/etc/users/");
	char path[50];
	strcpy(path, "/etc/users/");
	strcat(path, to_save -> username);

	int file = open(path, O_CREATE | O_RDWR);
	if (file < 0)
	{
		printf(1, "Failed to open file : save_user.");
		return -1;
	}
	int len = sizeof(*to_save);
	int written = write(file, to_save, len);
	if(written != len)
	{
		printf(1, "Failed to write file : save_user");
		return -1;
	}
	close(file);
	return 0;
}

int login(char username[])
{
	int file = open("utmp", O_CREATE | O_RDWR);
	int len = strlen(username);


	if(write(file, &username, len) != len)
	{
		printf(1, "login failed\n");
		return -1;
	}

//	struct user *to_update;
//	get_user(to_update, username);
//	cmostime(to_update -> last_login);
//	save_user(to_update);

	char dir[100];
	strcpy(dir, "/home/");
	strcat(dir, username);
	chdir(dir);
	printf(1, "Welcome!\n");
	close(file);
	return 0;
}

int attempt_login(char username[], char password[])
{
	struct user to_attempt;

	int found_user = get_user(&to_attempt, username);
	if(found_user < 0)
	{
		printf(1, "username not found : attempt_login : users.c\n");
		return -1;
	}

	printf(1, "password2 testing");
	printf(1, to_attempt.password);

	if(strcmp(to_attempt.password, password) == 0)
	{
		int result = login(username);

		if(result < 0)
		{
			printf(1, "unable to login : attempt_login : login : users.c");
			return -1;
		}

		return 0;
	}
	else
	{
		printf(1, "incorrect password\n");
		return -1;
	}
}

char *w()
{
	char *username = malloc(16);

	int file = open("utmp", O_RDONLY);
	if(file < 0)
	{
		printf(1, "w failed to open file\n");
		exit();
	}

	read(file, &username, 16);
	close(file);

	return username;
}

int su(char username[])
{
	char *password = malloc(16);
	printf(1, "password: ");
	gets(password, 16);

	char last = username[strlen(username) - 1];
	if (last == '\n' || last == '\r')
		username[strlen(username) - 1] = '\0';

	last = password[strlen(password) - 1];
	if (last == '\n' || last == '\r')
		password[strlen(password) - 1] = '\0';

	printf(1, "password testing");
	printf(1, password);

	int result = attempt_login(username, password);

	if(result < 0)
	{
		printf(1, "unable to login : su : users.c.");
		free(password);
		return -1;
	}

	free(password);
	return 0;
}

int useradd(char username[])
{
	char last = username[strlen(username) - 1];
	if (last == '\n' || last == '\r')
		username[strlen(username) - 1] = '\0';

	char home[32];
	strcpy(home, "/home/");
	strcat(home, username);
	struct user to_add;
	strcpy(to_add.username, username);
	strcpy(to_add.home, home);
	save_user(&to_add);
	return 0;
}

int passwd()
{
	char *username= w();
	char *password = malloc(16);
	struct user *to_update = (struct user*) malloc(sizeof(struct user));
	get_user(to_update, username);
	printf(1, "new password: ");
	gets(password, 16);

	char last = password[strlen(password) - 1];
	if (last == '\n' || last == '\r')
		password[strlen(password) - 1] = '\0';

	strcpy(to_update -> password, password);
	save_user(to_update);
	printf(1, "\nupdated.");
	free(username);
	free(password);
	return 0;
}
