/**
This file contains functions for expanding environment variables within strings.
It provides utilities to identify variables, extract their names, and replace
them with corresponding values from the environment.
*/

#include "minishell.h"

// IN FILE:

int	expand_variables(char **str, t_data *data, int expand_in_single_quotes);

/**
Used in expand_variables().

This function extracts the variable name following a '$' character.
Variable names are delimited by any character that is not alphanumerical.

 @param str 	The string from which to extract the variable name.
 @param i 		The position in the string where the '$' character is located.

 @return	A newly allocated string containing the variable name
 			(excluding the '$').
			`NULL` if memory allocation fails.
*/
static char	*get_var_name(char *str, int i)
{
	const char	*var_start;
	const char	*var_end;
	size_t		var_len;
	char		*var_name;

	var_start = &str[i + 1];
	if (*var_start == '?')
	{
		var_name = ft_strdup("?");
		return (var_name);
	}
	var_end = var_start;
	while (ft_isalnum(*var_end))
		var_end++;
	var_len = var_end - var_start;
	var_name = malloc(var_len + 1);
	if (!var_name)
		return (NULL);
	ft_strlcpy(var_name, var_start, var_len + 1);
	return (var_name);
}

/**
Used in replace_var_with_val().

Extracts the value of a specified environment variable (`env_var_search`)
from the minishell-specific environment variables linked list (t_env).
Also extracts the last exit status for '$?'.

 @return	The value the environment variable, if found.
			An empty string, if the specified environment variable is not found.
			`NULL` if memory allocation for the empty or exit status string
			failed.

If an exit status string is returned, it is the user's responsibilty to
free it when done using.
*/
static char	*get_var_value(const char *var_name, t_data *data)
{
	t_env	*current_node;
	char	*exit_status;
	char	*empty_str;

	current_node = data->envp_temp;
	if (ft_strcmp(var_name, "?") == 0)
	{
		exit_status = ft_itoa(data->exit_status);
		return (exit_status);
	}
	while (current_node)
	{
		if (ft_strcmp(var_name, current_node->e_var) == 0)
			return (current_node->value);
		current_node = current_node->next;
	}
	empty_str = ft_strdup("");
	if (!empty_str)
		return (NULL);
	return (empty_str);
}

/**
Used in expand_variables().

Replaces the variable at the specified position in the string with its value
from the environment list.

This function locates the variable name in the string starting from position
`i`, retrieves the corresponding value from the environment list, and replaces
the variable in the string with its value. The string is split into three parts:
the left part before the variable, the variable value, and the right part
after the variable. These parts are then concatenated into a new string.

 @param str 		The original string containing the variable to be replaced.
 @param i 			The position in the string where the '$' character of the
 					variable is located.
 @param var_name 	The name of the variable to be replaced (excluding the '$').
 @param env_list 	The environment list containing variable names and their
 					corresponding values.

 @return	`1` if the variable was successfully replaced with its value.
			`0` if an error occurred during memory allocation or if the variable
			does not exist in the environment list.
 */
static int	replace_var_with_val(char **str, int i, char *var_name,
		t_data *data)
{
	char	*var_val;
	char	*str_l;
	char	*str_r;

	str_l = NULL;
	str_r = NULL;
	var_val = get_var_value(var_name, data);
	if (!var_val)
		return (0);
	if (!get_str_l(str, i, &str_l) || !get_str_r(str, i, var_name, &str_r))
	{
		free_vars(&var_val, &str_l, &str_r);
		return (0);
	}
	if (!join_str_l(str, str_l, var_val) || !join_str_r(str, str_r))
	{
		free_vars(&var_val, &str_l, &str_r);
		return (0);
	}
	free_vars(&var_val, &str_l, &str_r);
	if (ft_strcmp(var_name, "?") == 0)
		free(var_val);
	return (1);
}

/**
Processes a variable in the string and replaces it with its corresponding value
from the minishell environment list.

This function identifies if the current position in the string (`*str` at `*i`)
starts a valid variable (indicated by a `$` character).
If the variable is valid and either expansion within single quotes is allowed
or the string is not within single quotes, the function replaces the
variable name with its value from the environment list.

 @param str 	A pointer to the string where the variable expansion is to be
 				performed. The string may be modified by the function.
 @param i 		A pointer to the current index in the string.
 				This index is used to locate the variable and will be adjusted to
				accommodate changes in the string.
 @param data 	A pointer to a data structure containing the environment
 				variables.
 @param expand_in_single_quotes 	A flag indicating whether variables should be
 									expanded within single quotes
									(`0` for no expansion, otherwise: expansion).

 @return	`1` if the variable was successfully processed and replaced;
 			`0` if there was an error during variable processing or
			memory if allocation failed.
 */
static int	process_variable(char **str, int *i, t_data *data,
	int expand_in_single_quotes)
{
	char	*var_name;

	if (is_variable(*str, *i) && (expand_in_single_quotes
			|| !data->quote.in_single))
	{
		var_name = get_var_name(*str, *i);
		if (!var_name)
			return (0);
		if (!replace_var_with_val(str, *i, var_name, data))
		{
			free(var_name);
			return (0);
		}
		free(var_name);
		(*i)--;
	}
	return (1);
}

/**
Expands all environment variables (including nestes ones) in the given string
with their corresponding values from the environment list.

This function traverses the input string, identifies variables starting with
a '$' character, and replaces each variable with its corresponding value from the
environment list.

 @param str 	The original string containing variables to be expanded.
 @param data 	Data structure which includes the local list of envp.
 @param expand_in_single_quotes	Flag to indicate if variables are expanded within
								single quotes (`0`: no expansion;
								otherwise: expansion).

 @return	`1` if all variables were successfully expanded.
			`0` if an error occurred during memory allocation or no string was
			passed.
*/
int	expand_variables(char **str, t_data *data, int expand_in_single_quotes)
{
	int		i;

	i = 0;
	if (!str || !*str)
		return (0);
	while ((*str)[i])
	{
		if (process_quote((*str)[i], &data->quote.in_single,
			&data->quote.in_double))
		{
			i++;
			continue ;
		}
		if (!process_variable(str, &i, data, expand_in_single_quotes))
			return (0);
		i++;
	}
	return (1);
}
