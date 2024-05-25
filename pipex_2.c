/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoinemura <antoinemura@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:08:20 by antoinemura       #+#    #+#             */
/*   Updated: 2024/05/25 15:30:27 by antoinemura      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	**get_all_path(char **env)
{
	char	**path;

	while (*env && ft_str_start_with(*env, "PATH") != 1)
		env++;
	if (!*env)
		return (NULL);
	path = ft_split(*env + 5, ':');
	return (path);
}

void	exec_command(t_process *proc, char **env)
{
	char	**path;
	char	*full_path;

	path = get_all_path(env);
	while (path && *path != NULL)
	{
		full_path = ft_strvjoin(*path, "/", proc->command, NULL);
		execve(full_path, proc->args, env);
		free(full_path);
		path++;
	}
}

int	fork_input_output(t_process *proc)
{
	int		pid;

	pid = fork();
	proc->pid = pid;
	if (pid == 0)
	{
		if (proc->stdin_fd != STDIN_FILENO)
		{
			dup2(proc->stdin_fd, STDIN_FILENO);
			close(proc->stdin_fd);
		}
		if (proc->stdout_fd != STDOUT_FILENO)
		{
			dup2(proc->stdout_fd, STDOUT_FILENO);
			close(proc->stdout_fd);
		}
	}
	return (pid);
}

void	free_t_process(void *t_proc)
{
	close(((t_process *)t_proc)->stdout_fd);
	close(((t_process *)t_proc)->stdin_fd);
	ft_freetab((void **)(((t_process *)t_proc)->args));
	ft_free((void *)&t_proc);
}
