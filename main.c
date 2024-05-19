/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoinemura <antoinemura@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 13:03:30 by antoinemura       #+#    #+#             */
/*   Updated: 2024/05/19 20:59:23 by antoinemura      ###   ########.fr       */
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

int	exec_command(char *command, char **env)
{
	char	**path;
	char	**cmd_args;
	char	*full_path;
	char	*cmd_with_path;
	int		ret;

	path = get_all_path(env);
	cmd_args = ft_split(command, ' ');
	ret = 0;
	write(STDERR_FILENO, "command\n", 9);
	while (path && *path != NULL)
	{
		full_path = ft_strjoin(*path, "/");
		cmd_with_path = ft_strjoin(full_path, cmd_args[0]);
		ret = execve(cmd_with_path, cmd_args, env);
		free(full_path);
		free(cmd_with_path);
		if (ret != -1)
		{
			ft_freetab((void **)cmd_args);
			return (1);
		}
		path++;
	}
	ft_freetab((void **)cmd_args);
	exit(127);
}

int	fork_input_output(int fd_in, int fd_out)
{
	int		pid;

	pid = fork();
	if (pid == 0)
	{
		if (fd_in != STDIN_FILENO)
		{
			dup2(fd_in, STDIN_FILENO);
			close(fd_in);
		}
		if (fd_out != STDOUT_FILENO)
		{
			dup2(fd_out, STDOUT_FILENO);
			close(fd_out);
		}
	}
	return (pid);
}

void	create_procs(t_list *procs, char **env)
{
	int	pid;
	static int c = 0;

	c++;
	if (procs == NULL || (c > 5))
		return ;
	pid = fork_input_output(((t_process *)procs->content)->stdin_fd, ((t_process *)procs->content)->stdout_fd);
	if (pid != 0)
		create_procs(procs->next, env);
	if (pid == 0)
	{
		exec_command(((t_process *)(procs->content))->command, env);
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

	proc = ft_calloc(sizeof(t_process), 1);
	proc->command = argv;
	proc->args = NULL;
	proc->stdin_fd = -1;
	proc->stdout_fd = -1;
	return (proc);
}

int	main(int argc, char **argv, char **env)
{
	t_list		*procs;
	t_process	*proc;
	int			i;

	if (argc != 5)
	{
		ft_printf("Usage: %s <inputf> <cmd1> <cmd2> <outputf>\n", argv[0]);
		return (1);
	}
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
	return (0);
}
