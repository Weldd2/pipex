/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amura <amura@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 13:03:30 by antoinemura       #+#    #+#             */
/*   Updated: 2024/05/26 00:14:20 by amura            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	create_procs(t_list *procs, char **env)
{
	t_list	*current;
	int 	pid;

	current = procs;
	while (current != NULL)
	{
		pid = fork_input_output(current->content);
		if (pid != 0)
		{
			close(((t_process *)current->content)->stdin_fd);
			close(((t_process *)current->content)->stdout_fd);
			current = current->next;
		}
		else if (pid == 0)
		{
			ft_lstclear(&(current->next), free_t_process);
			exec_command((current->content), env);
			ft_lstclear(&procs, free_t_process);
			write(STDERR_FILENO, "Command not found\n", 19);
			exit(127);
		}
	}
}


int	handle_files(t_list *list, char **argv)
{
	t_list	*current;
	int		fd_input;
	int		fd_output;
	char	*outputf;

	current = list;
	fd_input = open(argv[1], O_RDONLY);
	if (fd_input == -1)
		return (perror("open input file"), 2);
	((t_process *)current->content)->stdin_fd = fd_input;
	while (current->next != NULL)
		current = current->next;
	outputf = argv[ft_lstsize(list) + 2];
	fd_output = open(outputf, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_output == -1)
		return (perror("open output file"), 2);
	((t_process *)current->content)->stdout_fd = fd_output;
	return (0);
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
	if (command_with_args[0] == NULL)
		return (ft_freetab((void **)command_with_args), free(proc), NULL);
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

	if (argc < 5)
		return (ft_printf("Usage: %s <inf> <cmd> <cmd> <outf>\n", argv[0]), 1);
	procs = NULL;
	i = 2;
	while (i < argc - 1)
	{
		proc = create_proc_struct(argv[i]);
		if (proc == NULL)
			return (ft_lstclear(&procs, free_t_process), 2);
		ft_lstadd_back(&procs, ft_lstnew(proc));
		i++;
	}
	if (handle_files(procs, argv) == 2)
		return (ft_lstclear(&procs, free_t_process), 2);
	pipe_proc_struct(procs);
	create_procs(procs, env);
	ret = wait_procs(procs);
	ft_lstclear(&procs, free_t_process);
	return (WEXITSTATUS(ret));
}
