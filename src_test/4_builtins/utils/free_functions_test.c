#include "minishell.h"

/*Frees the memory of a struct of type t_env. See minishell.h for declaration*/
void	free_env_struct(t_env **head)
{
	t_env	*current;
	t_env	*next_node;

	current = *head;
	if (current)
	{
		while (current)
		{
			next_node = current->next;
			if (current->value)
				free(current->value);
			if (current->e_var)
				free(current->e_var);
			if (current)
				free(current);
			current = NULL;
			current = next_node;
		}
	}
}

/*Frees the memory of a struct of type t_cd. See minishell.h for declaration*/
int	free_cd_struct(t_cd **cd, int exit_status)
{
	if (*cd)
	{
		if ((*cd)->subdirectory)
			free((*cd)->subdirectory);
		if ((*cd)->parentdirectory)
			free((*cd)->parentdirectory);
		if ((*cd)->home_user)
			free((*cd)->home_user);
		free(*cd);
	}
	return (exit_status);
}

/*Frees the memory of any char array.*/
int	ft_freearray(char **arr)
{
	int	crstr;
	int	max;

	crstr = 0;
	max = 0;
	while (arr[crstr])
	{
		crstr++;
		max++;
	}
	crstr = 0;
	while (crstr < max)
	{
		free(arr[crstr]);
		arr[crstr] = NULL;
		crstr++;
	}
	free(arr);
	arr = NULL;
	return (0);
}
