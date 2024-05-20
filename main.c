/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoinemura <antoinemura@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 13:03:30 by antoinemura       #+#    #+#             */
/*   Updated: 2024/05/20 02:18:02 by antoinemura      ###   ########.fr       */
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
	execve(proc->command, proc->args, env);
	while (path && *path != NULL)
	{
		full_path = ft_strjoin(*path, "/");
		execve(proc->command, proc->args, env);
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

void	create_procs(t_list *procs, char **env)
{
	int	pid;

	if (procs == NULL)
		return ;
	pid = fork_input_output(procs->content);
	if (pid != 0)
	{
		close(((t_process *)procs->content)->stdin_fd);
		close(((t_process *)procs->content)->stdout_fd);
		create_procs(procs->next, env);
	}
	if (pid == 0)
	{
		ft_lstclear(&(procs->next), free_t_process);
		exec_command((procs->content), env);
		ft_lstdelone(procs, free_t_process);
		write(STDERR_FILENO, "Command not found\n", 19);
		exit(127);
	}
}

void	handle_files(t_list *list, char **argv)
{
	t_list	*current;
	int		fd_input;
	int		fd_output;

	current = list;
	fd_input = open(argv[1], O_RDONLY);
	if (fd_input == -1)
	{
		perror("open input file");
		exit(1);
	}
	((t_process *)current->content)->stdin_fd = fd_input;
	while (current->next != NULL)
		current = current->next;
	fd_output = open(argv[ft_lstsize(list) + 2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_output == -1)
	{
		perror("open output file");
		exit(1);
	}
	((t_process *)current->content)->stdout_fd = fd_output;
}

void	pipe_proc_struct(t_list *list)
{
	t_list	*current;
	int		pipefd[2];

	current = list;
	while (current->next != NULL)
	{
		pipe(pipefd);
		if (((t_process *)current->content)->stdout_fd == -1)
			((t_process *)current->content)->stdout_fd = pipefd[1];
		else
			close(pipefd[1]);
		current = current->next;
		if (((t_process *)current->content)->stdin_fd == -1)
			((t_process *)current->content)->stdin_fd = pipefd[0];
		else
			close(pipefd[0]);
	}
}

t_process	*create_proc_struct(char *argv)
{
	t_process	*proc;
	char		**command_with_args;

	proc = ft_calloc(sizeof(t_process), 1);
	command_with_args = ft_split(argv, ' ');
	proc->command = command_with_args[0];
	proc->args = command_with_args;
	proc->stdin_fd = -1;
	proc->stdout_fd = -1;
	proc->pid = -1;
	return (proc);
}

int	main(int argc, char **argv, char **env)
{
	t_list		*procs;
	t_process	*proc;
	int			i;
	int			ret;

	if (argc != 5)
		return (ft_printf("Usage: %s <inputf> <cmd1> <cmd2> <outputf>\n", argv[0]), 1);
	procs = NULL;
	i = 2;
	while (i < argc - 1)
	{
		proc = create_proc_struct(argv[i]);
		ft_lstadd_back(&procs, ft_lstnew(proc));
		i++;
	}
	handle_files(procs, argv);
	pipe_proc_struct(procs);
	create_procs(procs, env);
	while (procs->next != NULL)
	{
		waitpid(((t_process *)procs->content)->pid, &ret, 0);
		procs = procs->next;
	}
	return (WEXITSTATUS(ret));
}
