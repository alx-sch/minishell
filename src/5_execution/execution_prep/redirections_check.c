#include "minishell.h"

/*In the case of several redirections, and a mix of append and redir_out,
this function ensures that not both "append" and "redir_out" are set to true,
but only the last redirection type.*/
static void	reset_redir_or_append_out(t_exec *exec, bool redir_type)
{
	if (redir_type == 0)
	{
		exec->redir_out = 1;
		if (exec->append_out == 1)
			exec->append_out = 0;
	}
	if (redir_type == 1)
	{
		exec->append_out = 1;
		if (exec->redir_out == 1)
			exec->redir_out = 0;
	}
}

/*If a redirection symbol '>' meaning the output should be redirected, 
it saves the information in the bool exec->redir_out and stores the filename 
of the new output source in exec->outfile.*/
static t_list	*redirect_out(t_data *data, t_exec *exec, t_list *current, \
t_token *token)
{
	move_current_and_update_token(&current, &token);
	if (exec->outfile)
		free(exec->outfile);
	exec->outfile = ft_strdup(token->lexeme);
	if (!exec->outfile)
		exec_errors(data, exec, 1);
	check_file_exist_child(data, exec);
	return (current);
}

/*If a redirection symbol '<' meaning the input should be redirected, 
it saves the information in the bool exec->redir_in and stores the filename 
of the new input source in exec->infile.*/
static t_list	*redirect_in(t_data *data, t_exec *exec, t_list *current, \
t_token *token)
{
	exec->redir_in = 1;
	move_current_and_update_token(&current, &token);
	if (exec->infile)
		free(exec->infile);
	exec->infile = ft_strdup(token->lexeme);
	if (!exec->infile)
		exec_errors(data, exec, 1);
	check_file_exist_child(data, exec);
	return (current);
}

/*Checks if there are any redirections '>' or '<' happening in the input.*/
void	check_redirections(t_data *data, t_exec *exec, int position)
{
	t_list	*current;
	t_token	*token;

	current = (t_list *)data->tok.tok_lst;
	token = (t_token *)current->content;
	while (current && token->position != position)
		move_current_and_update_token(&current, &token);
	while (current && token->type != PIPE)
	{
		if (token->type == REDIR_IN)
			current = redirect_in(data, exec, current, token);
		else if (token->type == REDIR_OUT)
		{
			reset_redir_or_append_out(exec, 0);
			current = redirect_out(data, exec, current, token);
		}
		else if (token->type == APPEND_OUT)
		{
			reset_redir_or_append_out(exec, 1);
			current = redirect_out(data, exec, current, token);
		}
		move_current_and_update_token(&current, &token);
	}
}
