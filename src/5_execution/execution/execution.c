#include "minishell.h"

/*The main function of the execution. Prepares for execution, executes the
program and exits. If the commmand is a builtin, instead of going into execve,
it rather executes the builtin-function. If it is not a builtin, execve 
takes over to execute the command. If anything fails, it prints out an error
message and returns the right exit code.*/
void	execution(t_data *data, t_exec *exec, int position)
{
	int	builtin_exit_code;

	builtin_exit_code = 0;
	prep_execution(data, exec, position);
	if (is_builtin(exec))
	{
		builtin_exit_code = builtin(data, exec);
		free_exec(exec);
		free_data(data, 1);
		exit(builtin_exit_code);
	}
	execve(exec->current_path, exec->flags, exec->envp_temp_arr);
	exec_errors(data, exec, 4);
	exit(errno);
}
