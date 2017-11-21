/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfrisby <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/17 13:48:34 by mfrisby           #+#    #+#             */
/*   Updated: 2017/11/17 13:50:15 by mfrisby          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_script.h"
#include <sys/mman.h>

static void		init_env(t_env **env)
{
	(*env)->filename = NULL;
	(*env)->command = NULL;
	(*env)->cmd_size = 0;
	(*env)->opt_a = 0;
	(*env)->opt_d = 0;
	(*env)->opt_f = 0;
	(*env)->opt_p = 0;
	(*env)->opt_q = 0;
	(*env)->opt_r = 0;
	(*env)->opt_k = 0;
	(*env)->opt_t = 0;
}

static void		free_stuff(t_env *env)
{
	int			i;

	i = 0;
	if (env->filename)
		munmap(env->filename, ft_strlen(env->filename) + 1);
	while (env->command && env->command[i])
	{
		munmap(env->command[i], ft_strlen(env->command[i] + 1));
		i++;
	}
	if (env->command)
		munmap(env->command, env->cmd_size);
	if (env)
		munmap(env, sizeof(env + 1));
}

static void		msg_start(t_env *env)
{
	ft_putstr("Script started, output file is ");
	if (env->filename != NULL)
		ft_putendl(env->filename);
	else
		ft_putendl("typescript");
}

static void		msg_end(t_env *env)
{
	ft_putstr("Script done, output file is ");
	if (env->filename != NULL)
		ft_putendl(env->filename);
	else
		ft_putendl("typescript");
}

int				main(int ac, char **av, char **envp)
{
	t_env		*env;
	int			ret;

	ret = 0;
	env = ft_mmap(sizeof(t_env) + 1);
	init_env(&env);
	ret = get_params(ac, av, &env);
	ft_putendl("la");
	if (env->command == NULL)
		get_shell(env, envp);
	if (ret == -1)
		return (ret);
	if (ret == ac)
		ret = 0;
	if (env->opt_q == 0)
		msg_start(env);
	ft_script(env);
	if (env->opt_q == 0)
		msg_end(env);
	free_stuff(env);
	return (0);
}
