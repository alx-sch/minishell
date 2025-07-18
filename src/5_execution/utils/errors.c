#include "minishell.h"

/*Prints an error message if a file doesn't exist, or something goes 
wrong in the redirection.*/
void	redirections_errors(t_data *data, t_exec *exec, int std, int parent)
{
	ft_putstr_fd(ERR_PREFIX, 2);
	if (!std)
		ft_putstr_fd(exec->infile, 2);
	else if (std)
		ft_putstr_fd(exec->outfile, 2);
	ft_putstr_fd(": ", 2);
	perror("");
	if (!parent)
	{
		free_exec(exec);
		free_data(data, 1);
		exit(1);
	}
}

/*Prints an error message an exits the process if the command is not found*/
void	error_incorrect_path(t_data *data, t_exec *exec)
{
	if (exec->cmd_found)
	{
		ft_putstr_fd(ERR_PREFIX, 2);
		ft_putstr_fd("Command '", 2);
		ft_putstr_fd(exec->cmd, 2);
		ft_putstr_fd("' not found\n", 2);
		free_exec(exec);
		free_data(data, 1);
		errno = EKEYEXPIRED;
		exit(errno);
	}
	free_exec(exec);
	free_data(data, 1);
	exit(0);
}

/*Makes sure to NULL-terminate the envp_temp_arr in case of memory allocation
failure somewhere along the way- so the whole array can safely be freed.*/
void	conversion_errors(t_data *data, t_exec *exec, int i)
{
	exec->envp_temp_arr[i] = NULL;
	exec_errors(data, exec, 1);
}

/*Hardsets exit codes if execve fails, and prints an error-message based
on the errno value. Frees all allocated memory and exits the child process.*/
static void	execve_failure(t_data *data, t_exec *exec)
{
	int	exit_code;

	exit_code = 0;
	if (errno == 2)
		exit_code = 127;
	else if (errno == 20 || errno == 13)
		exit_code = 126;
	perror("");
	close(exec->outfile_fd);
	close(exec->infile_fd);
	free_exec(exec);
	free_data(data, 1);
	exit(exit_code);
}

/*Error handling during execution, prints an error message, cleans up allocated 
memory and exits the child process.*/
void	exec_errors(t_data *data, t_exec *exec, int error_code)
{
	if (error_code == 1)
		print_err_msg_prefix("minishell: exec: Cannot allocate memory\n");
	if (error_code == 2 || error_code == 4)
	{
		ft_putstr_fd(ERR_PREFIX, 2);
		ft_putstr_fd(exec->cmd, 2);
		ft_putstr_fd(": ", 2);
		if (error_code == 2)
			errno = ENOENT;
		if (error_code == 4)
			execve_failure(data, exec);
		perror("");
	}
	if (error_code == 3)
	{
		ft_putstr_fd(ERR_PREFIX, 2);
		ft_putstr_fd("Piping failed", 2);
		perror("");
	}
	free_exec(exec);
	free_data(data, 1);
	exit(errno);
}
