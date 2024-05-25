/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amura <amura@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:08:20 by antoinemura       #+#    #+#             */
/*   Updated: 2024/05/25 23:47:38 by amura            ###   ########.fr       */
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
	int		i;

	i = 0;
	path = get_all_path(env);
	while (path && path[i] != NULL)
	{
		full_path = ft_strvjoin(path[i], "/", proc->command, NULL);
		execve(full_path, proc->args, env);
		free(full_path);
		i++;
	}
	ft_freetab((void **)path);
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
	else
	{
		close(proc->stdout_fd);
		close(proc->stdin_fd);
	}
	return (pid);
}

void	free_t_process(void *t_proc)
{
	if (((t_process *)t_proc)->stdout_fd > 2)
		close(((t_process *)t_proc)->stdout_fd);
	if (((t_process *)t_proc)->stdin_fd > 2)
		close(((t_process *)t_proc)->stdin_fd);
	ft_freetab((void **)(((t_process *)t_proc)->args));
	ft_free((void *)&t_proc);
}

int	wait_procs(t_list *procs)
{
	int		ret;
	t_list	*current;

	current = procs;
	while (current != NULL)
	{
		waitpid(((t_process *)current->content)->pid, &ret, 0);
		current = current->next;
	}
	return (ret);
}