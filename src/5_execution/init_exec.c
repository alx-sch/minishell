#include "minishell.h"

/*A part of initializing the exec struct, setting all members to NULL.*/
t_exec	*set_exec_members_to_null(t_exec *exec)
{
	exec->curr_child = 0;
	exec->count_flags = 0;
	exec->all_paths = NULL;
	exec->cmd = NULL;
	exec->current_path = NULL;
	exec->flags = NULL;
	exec->input = NULL;
	exec->child = NULL;
	exec->envp_temp_arr = NULL;
	exec->redir_in = 0;
	exec->redir_out = 0;
	exec->append_out = 0;
	exec->infile = NULL;
	exec->outfile = NULL;
	exec->first = 1;
	exec->infile_fd = 0;
	exec->outfile_fd = 1;
	exec->cmd_found = 0;
	return (exec);
}

/*Initializes the exec struct. Allocates memory for an int array that will store
the pid's of the child processes.*/
int	init_exec(t_data *data)
{
	t_exec	*exec;

	exec = malloc(sizeof(t_exec));
	if (!exec)
		exec_errors(data, exec, 1);
	exec = set_exec_members_to_null(exec);
	exec->child = malloc(sizeof(t_child));
	if (!exec->child)
		exec_errors(data, exec, 1);
	if (data->pipe_nr == 0)
		exec->child->nbr = malloc(sizeof(pid_t) * 2);
	else
		exec->child->nbr = malloc(sizeof(pid_t) * (data->pipe_nr + 2));
	if (!exec->child->nbr)
		exec_errors(data, exec, 1);
	if (data->pipe_nr == 0)
	{
		get_flags_and_command(data, exec, 0);
		if (is_parent_builtin(exec))
			return (execution_only_in_parent(data, exec));
		else
			reset_exec(exec);
	}
	create_child_processes(data, exec);
	return (free_exec(exec));
}
