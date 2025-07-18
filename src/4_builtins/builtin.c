#include "minishell.h"

// Checking if the input is a builtin command, checks if the arguments are
// valid and executes the builtin.
int	builtin(t_data *data, t_exec *exec)
{
	if (!ft_strcmp(exec->cmd, "pwd"))
		return (pwd(exec));
	else if (!ft_strcmp(exec->cmd, "cd"))
		return (cd(data, exec));
	else if (!ft_strcmp(exec->cmd, "env"))
		return (env(exec, data->envp_temp));
	else if (!ft_strcmp(exec->cmd, "exit"))
		exit(exit_with_code(data, exec));
	else if (!ft_strcmp(exec->cmd, "unset"))
	{
		if (exec->flags[1] && unset_err_invalid_option(exec->flags[1], 0))
			return (2);
		unset(exec, &data->envp_temp);
		return (unset(exec, &data->export_list));
	}
	else if (!ft_strcmp(exec->cmd, "export"))
		return (export(data, exec));
	else if (!ft_strcmp(exec->cmd, "echo"))
		return (minishell_echo(exec));
	return (0);
}
