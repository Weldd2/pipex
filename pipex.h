/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amura <amura@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 14:35:07 by antoinemura       #+#    #+#             */
/*   Updated: 2024/05/26 01:21:18 by amura            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <stdio.h>
# include <stdlib.h>
# include "libft/libft.h"
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <stdlib.h>
# include <stdio.h>

typedef struct s_process
{
	char	*command;
	char	**args;
	int		stdout_fd;
	int		stdin_fd;
	int		pid;
}	t_process;

void		create_procs(t_list *procs, char **env);
char		**get_all_path(char **env);
void		exec_command(t_process *proc, char **env);
int			fork_input_output(t_process *proc);
void		free_t_process(void *t_proc);
int			handle_files(t_list *list, char **argv);
int			pipe_proc_struct(t_list *list);
t_process	*create_proc_struct(char *argv);
int			wait_procs(t_list *procs);

#endif