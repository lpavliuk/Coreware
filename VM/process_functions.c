#include "corewar.h"

int		check_valid_codage(char opcode, char *codage)
{
	char	input_arg;
	char	possible_arg;
	char	it_was;

	input_arg = 0;
	while(input_arg < COUNT_ARGS(opcode))
	{
		possible_arg = 0;
		it_was = 0;
		while(possible_arg < 3)
		{
			if (ARG(opcode, input_arg, possible_arg)
				&& codage[input_arg] == possible_arg + 1)
				it_was = 1;
			possible_arg++;
		}
		if (!it_was)
			return (0);
		input_arg++;
	}
	return (1);
}

void	change_process_position(char opcode, char *codage, t_process *process)
{
	char	offset;
	char	i;

	i = 0;
	while (i < 3)
	{
		(codage[i] == IND_CODE) ? (offset += T_IND_SIZE) : 0;
		(codage[i] == REG_CODE) ? (offset += T_REG_SIZE) : 0;
		(codage[i] == DIR_CODE) ? (offset += LABEL_SIZE(opcode)) : 0;
		i++;
	}
	process->position += (offset + 1 + CODAGE(opcode)) % MEM_SIZE;
}

void	ft_live(t_process *process, t_vm *vm)
{
	process->live = 1;
	if (get_arg((process->position + 1) % MEM_SIZE, LABEL_SIZE(OPCODE(0)))
		== process->parent->id)
	{
		process->parent->lives_cur_period++;
		process->parent->last_live = vm->cur_cycle;
	}
	if (vm->flag_visual)
		g_pixels[process->position]->live = 50;
}

void	ft_ld(t_process *process, t_vm *vm)
{
	char			codage[4];
	unsigned int	arg2;
	unsigned int	result;

	decipher_codage(codage, COUNT_ARGS(2), (process->position + 1) % MEM_SIZE);
	if (check_valid_codage(OPCODE(1), codage)
	&& (arg2 = get_arg((process->position + ((codage[0] == IND_CODE) ? 2 : 4))
			% MEM_SIZE, T_REG_SIZE)) > 1 && arg2 < 17)
	{
		result = (codage[0] == IND_CODE) ? (get_arg((process->position
		+ (get_arg((process->position + 2) % MEM_SIZE, T_IND_SIZE) % IDX_MOD))
		% MEM_SIZE, T_IND_READ)) : (get_arg((process->position + 2) % MEM_SIZE,
			T_DIR_SIZE));
		process->registries[arg2] = result;
		process->carry = (result) ? 0 : 1;
	}
	change_process_position(OPCODE(1), codage, process);
}


int		ft_st_sti_check_args(unsigned int *args, char *codage,
	t_process *process, char offset)
{

}

void	set_map_value(t_process *process, unsigned int val,
		unsigned int new_pstn)
{
	int		j;

	j = 0;
	while (j < 4)
	{
		g_map[(process->position + new_pstn + j) % MEM_SIZE] =
			((unsigned char *)&val)[j];
		g_pixels[(process->position + new_pstn + j) % MEM_SIZE]->counter = 50;
		g_pixels[(process->position + new_pstn + j) % MEM_SIZE]->color =
			process->parent->player_counter;
		g_pixels[(process->position + new_pstn + j) % MEM_SIZE]->bold = 1;
	}
}

void	ft_st(t_process *process, t_vm *vm)
{
	char			codage[4];
	unsigned int	position_arg2;
	unsigned int	result;

	decipher_codage(codage, COUNT_ARGS(3), (process->position + 1) % MEM_SIZE);
	if (check_valid_codage(OPCODE(2), codage) && (result = get_arg(
		(process->position + 2) % MEM_SIZE, T_REG_SIZE) > 0) && result < 17)
	{
		position_arg2 = get_arg((process->position + 3) % MEM_SIZE, ((codage[1]
			== IND_CODE) ? T_IND_SIZE : T_REG_SIZE));
		if (codage[1] == IND_CODE)
		{
			position_arg2 %= IDX_MOD;
			set_map_value(process, result, position_arg2);
		}
		else if (position_arg2 > 0 && position_arg2 <= REG_NUMBER)
			process->registries[position_arg2] = result;
	}
	change_process_position(OPCODE(2), codage, process);
}

// void	ft_add(t_process *process, t_vm *vm)
// {

// }

// void	ft_sub(t_process *process, t_vm *vm)
// {

// }

// void	ft_and(t_process *process, t_vm *vm)
// {

// }

// void	ft_or(t_process *process, t_vm *vm)
// {

// }

// void	ft_xor(t_process *process, t_vm *vm)
// {

// }

// void	ft_zjmp(t_process *process, t_vm *vm)
// {

// }

int		ft_ldi_lldi_check_args(unsigned int *args, char *codage,
	t_process *process, char offset)
{
	char	i;

	i = 0;
	while (i < 3)
	{
		if (codage[i] == IND_CODE && (offset += T_IND_SIZE))
			args[i] = get_arg((process->position + (get_arg((process->position
			+ 2) % MEM_SIZE, T_IND_SIZE) % IDX_MOD)) % MEM_SIZE, T_IND_READ);
		else if (codage[i] == REG_CODE && (offset += T_REG_SIZE))
		{
			args[i] = get_arg((process->position + (offset - T_REG_SIZE))
				% MEM_SIZE, T_REG_SIZE);
			if (args[i] < 1 || args[i] > 16)
				return (0);
		}
		else if (codage[i] == DIR_CODE && (offset += LABEL_SIZE(OPCODE(9))))
			args[i] = get_arg((process->position
			+ (offset - LABEL_SIZE(OPCODE(9)))) % MEM_SIZE, T_DIR_SIZE);
		i++;
	}
	return (1);
}

void	ft_ldi(t_process *process, t_vm *vm)
{
	char			codage[4];
	unsigned int	args[3];
	unsigned int	result;

	decipher_codage(codage, COUNT_ARGS(10), (process->position + 1) % MEM_SIZE);
	if (check_valid_codage(OPCODE(9), codage) && ft_ldi_lldi_check_args(args,
		codage, process, 2))
	{
		result = get_arg((process->position + ((args[0] + args[1]) % IDX_MOD))
			% MEM_SIZE, T_IND_READ);
		process->registries[args[2]] = result;
	}
	change_process_position(OPCODE(9), codage, process);
}

int		ft_sti_check_args(unsigned int *args, char *codage, t_process *process,
		char offset)
{
	char	i;

	i = 0;
	while (i < 3)
	{
		if (codage[i] == REG_CODE)
		{
			args[i] = get_arg((process->position + offset) % MEM_SIZE,
				T_REG_SIZE);
			offset += T_REG_SIZE;
			if (args[i] < 1 || args[i] > 16)
				return (0);
		}
		else if (codage[i] == IND_CODE && (offset += T_IND_SIZE))
			args[i] = get_arg((process->position + (get_arg((process->position
			+ (offset - T_IND_SIZE)) % MEM_SIZE, T_IND_SIZE) % IDX_MOD))
			% MEM_SIZE, T_IND_READ);
		else if (codage[i] == DIR_CODE && (offset += T_DIR_SIZE))
			args[i] = get_arg((process->position + (offset - T_DIR_SIZE))
				% MEM_SIZE, T_DIR_SIZE);
		i++;
	}
	return (1);
}

/*
**	"Значение T_REG (первый аргумент) записывается в ячейку, по адресу (текущая позиция PC плюс ((второй аргумент плюс третий аргумент) % IDX_MOD))
**	 - Если второй аргумент T_IND - то ясное дело, что вместо второго аргумента, в уровнение подставляются те 4 байта, которые мы берём из ячейки (T_IND % IDX_MOD)."
*/

void	ft_sti(t_process *process, t_vm *vm)
{
	char			codage[4];
	unsigned int	args[3];

	decipher_codage(codage, COUNT_ARGS(11), (process->position + 1) % MEM_SIZE);
	if (check_valid_codage(OPCODE(10), codage) && ft_sti_check_args(args,
		codage, process, 2))
	{
		set_map_value(process, process->registries[args[0]],
			((((codage[1] == REG_CODE) ? process->registries[args[1]] : args[1])
			+ ((codage[2] == REG_CODE) ? process->registries[args[2]]
			: args[2])) % IDX_MOD) % MEM_SIZE);
	}
	change_process_position(OPCODE(10), codage, process);
}

// void	ft_fork(t_process *process, t_vm *vm)
// {

// }

void	ft_lld(t_process *process, t_vm *vm)
{
	char			codage[4];
	unsigned int	arg2;
	unsigned int	result;

	decipher_codage(codage, COUNT_ARGS(13), (process->position + 1) % MEM_SIZE);
	if (check_valid_codage(OPCODE(12), codage)
	&& (arg2 = get_arg((process->position + ((codage[0] == IND_CODE) ? 2 : 4))
			% MEM_SIZE, T_REG_SIZE)) > 1 && arg2 < 17)
	{
		result = (codage[0] == IND_CODE) ? (get_arg((process->position
		+ get_arg((process->position + 2) % MEM_SIZE, T_IND_SIZE))
		% MEM_SIZE, T_IND_READ)) : (get_arg((process->position + 2) % MEM_SIZE,
			T_DIR_SIZE));
		process->registries[arg2] = result;
		process->carry = (result) ? 0 : 1;
	}
	change_process_position(OPCODE(12), codage, process);
}

void	ft_lldi(t_process *process, t_vm *vm)
{
	char			codage[4];
	unsigned int	args[3];
	unsigned int	result;

	decipher_codage(codage, COUNT_ARGS(14), (process->position + 1) % MEM_SIZE);
	if (check_valid_codage(OPCODE(13), codage) && ft_ldi_check_args(args,
		codage, process, 2))
	{
		result = get_arg((process->position + (args[0] + args[1]))
			% MEM_SIZE, T_IND_READ);
		process->registries[args[2]] = result;
		process->carry = (result) ? 0 : 1;
	}
	change_process_position(OPCODE(13), codage, process);
}

// void	ft_lfork(t_process *process, t_vm *vm)
// {

// }

// void	ft_aff(t_process *process, t_vm *vm)
// {

// }
