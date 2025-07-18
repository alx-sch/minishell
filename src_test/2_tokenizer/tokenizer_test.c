/**
This file implements functions for tokenizing input strings in our minishell.
Commands, operators, and redirections are parsed into distinct tokens stored
in a linked list.
*/

#include "minishell.h"

// IN FILE:

int		get_tokens(t_data *data);
t_list	*create_tok(t_data *data, t_token_type type, const char *lexeme,
			int *i);

/**
Used in create_tok().

To handle memory allocation failures in create_token() more efficiently,
this helper function cleans up the allocated memory and outputs an error message
to the standard error stream.
*/
static void	malloc_fail_in_create_token(t_data *data)
{
	if (data->tok.tok) // Check if the token pointer within data is not NULL
	{
		if (data->tok.tok->lexeme) // If the token's lexeme has been allocated, free it and set to NULL
		{
			free(data->tok.tok->lexeme);
			data->tok.tok->lexeme = NULL;
		}
		// Free the token itself and set the pointer to NULL
		free(data->tok.tok);
		data->tok.tok = NULL;
	}
	print_err_msg(ERR_MALLOC);
}

/**
Creates a new token structure and stores it in a `t_list` node,
so libft functions for list manipulation can be used.
The token contains information about its type, lexeme, and position.

If the token creation is successful, the index *i is updated to point to the
position immediately after the lexeme in the input string.

 @param data 	Pointer to the data structure containing token-related info.
 @param type 	The type of the token.
 @param lexeme 	The content of the token (substring of input string)
 @param i 		Pointer to the current index in the input string.

 @return	A pointer to the newly created `t_list` node if successful.
			`NULL` if memory allocation or creating the linked list node fails.
*/
t_list	*create_tok(t_data *data, t_token_type type, const char *lexeme, int *i)
{
	data->tok.tok = malloc(sizeof(t_token));
	if (!data->tok.tok)
	{
		print_err_msg(ERR_MALLOC);
		return (NULL);
	}
	// Write lexeme from the stack into the token structure
	data->tok.tok->lexeme = ft_strdup(lexeme);
	if (!data->tok.tok->lexeme)
	{
		malloc_fail_in_create_token(data);
		return (NULL);
	}
	// Create a new linked list node with the token as its content
	data->tok.new_node = ft_lstnew(data->tok.tok);
	if (!data->tok.new_node)
	{
		malloc_fail_in_create_token(data);
		return (NULL);
	}
	// Set token type and position
	data->tok.tok->type = type;
	data->tok.tok->position = *i;
	// Update the position pointer to the end of the lexeme
	*i = *i + ft_strlen(lexeme);
	return (data->tok.new_node);
}

/**
Used in get_tokens().

Extracts a token from the input string starting at position *i until a
delimiter is encountered.

 @return	`0` if memory allocation fails during substring creation or token
 			node creation.
			`1` if the token is successfully added OR if no token was added as
			data->input[*i] is whitespace or is at the end of the input string.
*/
static int	add_other_token(t_data *data, int *i)
{
	int	start;

	start = *i;
	if (data->input[*i] && !is_whitespace(data->input[*i])) // Skip token creation if end of string or whitespace
	{
		while (!is_delimiter(data, data->input[*i])) // Find the end of the token
			(*i)++;
		data->tok.tmp = ft_substr(data->input, start, (*i) - start); // Extract the token substring
		if (!data->tok.tmp)
			return (0); // Substring extraction failed.
		data->tok.new_node = create_tok(data, OTHER, data->tok.tmp, &start); // Create a new token node and add it to the token list
		if (!data->tok.new_node)
		{
			free(data->tok.tmp);
			free_unlinked_token(data); // Frees dangling token not added to linked list
			return (0); // Token creation failed.
		}
		ft_lstadd_back(&data->tok.tok_lst, data->tok.new_node);
		free(data->tok.tmp);
	}
	return (1); // Token added or no token added as *i points to whitespace or '\0'
}

/**
Updates the position of each token in the token list to reflect its position
within the list.

This function traverses the token list and assigns the `position` member of each
token to its index in the list.

 @param data 	Pointer to the data structure containing the token list.

 @return 		`1` if all positions were successfully updated, `0` otherwise.
*/
static int	update_token_positions(t_data *data)
{
	t_list	*current;
	t_token	*token;
	int		position;

	if (!data || !data->tok.tok_lst)
		return (0);
	current = data->tok.tok_lst;
	token = NULL;
	position = 0;
	while (current)
	{
		token = (t_token *)current->content;
		if (!token)
			return (0);
		token->position = position;
		position++;
		current = current->next;
	}
	return (1);
}

/**
Parses the input string to extract tokens and builds a token list (t_list type).

This function iterates over the input string and extracts tokens based on
specific criteria:
- Skips leading whitespace characters.
- Checks for redirection operators and adds tokens if found.
- Adds a token for the pipe character `|` if found.
- Treats remaining parts of the input string as OTHER tokens.

For each token, it creates a new token node and adds it to the token list.
If a token cannot be created due to a memory allocation failure, the function
stops processing further tokens.

 @return	`0` if tokenization failed or if input is whitespace only.
			`1` if tokenization was sucessful.
*/
int	get_tokens(t_data *data)
{
	int	i;

	i = 0;
	while (data->input[i])
	{
		while (is_whitespace(data->input[i]))
			i++;
		data->tok.r_redir = is_redirection(data, &i);
		data->tok.r_pipe = is_pipe(data, &i);
		data->tok.r_other = add_other_token(data, &i);
		if (data->tok.r_redir <= 0 || data->tok.r_pipe <= 0
			|| data->tok.r_other == 0)
		{
			if (data->tok.r_redir == 0 || data->tok.r_pipe == 0
				|| data->tok.r_other == 0)
				print_err_msg(ERR_TOKEN);
			return (0);
		}
	}
	if (!update_token_positions(data))
	{
		print_err_msg(ERR_TOKEN);
		return (0);
	}
	return (1);
}
