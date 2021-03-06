/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_script.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfrisby <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/17 13:51:09 by mfrisby           #+#    #+#             */
/*   Updated: 2017/11/22 17:22:03 by mfrisby          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_script.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <utmp.h>

static int		read_master(int fd_master, int fd_file, fd_set fd_in)
{
	int			rc;
	char		input[150];

	if (FD_ISSET(fd_master, &fd_in))
	{
		rc = read(fd_master, input, sizeof(input));
		if (rc > 0)
		{
			write(fd_file, input, rc);
			write(1, input, rc);
		}
		else if (rc <= 0)
			return (-1);
	}
	return (0);
}

static int		read_user(int fd_master, fd_set fd_in)
{
	int			rc;
	char		input[150];

	if (FD_ISSET(0, &fd_in))
	{
		rc = read(0, input, sizeof(input));
		if (rc > 0)
		{
			if (write(fd_master, input, rc) <= 0)
				return (-1);
		}
		else if (rc < 0)
			return (-1);
	}
	return (0);
}

static void		fork_parent(int fd_master, int fd_file)
{
	int			rc;
	fd_set		fd_in;

	FD_ZERO(&fd_in);
	FD_SET(0, &fd_in);
	FD_SET(fd_master, &fd_in);
	rc = select(fd_master + 1, &fd_in, NULL, NULL, NULL);
	if (rc == -1)
		return ;
	else if (rc > 0)
	{
		if (read_user(fd_master, fd_in) == -1)
			return ;
		if (read_master(fd_master, fd_file, fd_in) == -1)
			return ;
	}
}

static void		fork_child(int fd_master, int fd_slave, char **envp, t_env *env)
{
	int			i;

	i = 0;
	close(fd_master);
	dup2(fd_slave, 0);
	dup2(fd_slave, 1);
	dup2(fd_slave, 2);
	close(fd_slave);
	setsid();
	ioctl(0, TIOCSCTTY, 1);
	if (env->path && env->command != NULL)
	{
		while (env->path[i])
		{
			execve(env->path[i], env->command, envp);
			i++;
		}
	}
	if (env->command != NULL)
		execve(env->command[0], env->command, envp);
	exit(-1);
}

void			ft_script(int fd_file, char **envp, t_env *env)
{
	int			fd_master;
	int			fd_slave;
	int			pid;
	int			i;

	if (create_pty(&fd_master, &fd_slave) == -1)
		return ;
	if (!fd_file || !fd_master || !fd_slave || (pid = fork()) < 0)
		return ;
	if (!pid)
		fork_child(fd_master, fd_slave, envp, env);
	set_raw_mode(env);
	close(fd_slave);
	while (waitpid(pid, &i, WNOHANG) != pid)
		fork_parent(fd_master, fd_file);
	ft_tcsetattr(0, &env->saved_term);
}
