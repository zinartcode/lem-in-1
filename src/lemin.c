/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lemin.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbouillo <hbouillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/30 19:24:35 by hbouillo          #+#    #+#             */
/*   Updated: 2018/03/06 06:15:33 by hbouillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lemin.h"

int				error(int errcode, char const *const errmsg, char *errtype,
						int errexit)
{
	if (get_arg(ARG_VERBOSE))
	{
		ft_printf("%rgb%s (ERRCODE=%d, ERRMSG=\"%s\")%0rgb\n",
			errexit ? 0xff3835 : 0xf7ba2c, errtype, errcode, errmsg);
		if (errexit)
			exit(1);
	}
	else if (errexit)
	{
		ft_printf("ERROR\n");
		exit(1);
	}
	return (1);
}

static void		free_data(t_data *data)
{
	int			i;

	i = -1;
	while (++i < data->rooms_count)
		free(data->id_tubes[i]);
	i = -1;
	while (++i < ROOM_LISTS)
		ft_llist_del(data->hash_rooms + i, NULL);
	free(data->id_tubes_count);
	free(data->id_tubes);
	ft_llist_del(&data->rooms, &free_room);
	ft_llist_del(&data->tubes, &free_tube);
	free(data);
}

static void		free_network(t_network *network)
{
	t_node		**nodes_temp;

	nodes_temp = network->nodes;
	while (*nodes_temp)
	{
		free((*nodes_temp)->name);
		free((*nodes_temp)->nodes);
		free(*nodes_temp);
		nodes_temp++;
	}
	free(network->nodes);
	free(network);
}

static void		free_paths(t_path *paths)
{
	int			i;
	int			j;

	i = 0;
	while (paths[i].length)
	{
		j = -1;
		while (++j < paths[i].length)
			free(paths[i].nodes[j]);
		free(paths[i].nodes);
		i++;
	}
	free(paths);
}

void			lemin(void)
{
	t_data		*data;
	t_network	*network;
	t_path		*paths;
	int			i;
	int			ants;

	verbose("%rgbParsing...\n%0rgb", V_COLOR);
	data = parse_data();
	ants = data->ants;
	verbose("%rgbDone.\n%rgbChecking data...\n%0rgb", V_COLOR, V_COLOR);
	if (!data->start)
		error(ERR_NO_START_NODE, ERR_CRITICAL);
	else if (!data->end)
		error(ERR_NO_END_NODE, ERR_CRITICAL);
	verbose("%rgbDone.\n%rgbBuilding network...\n%0rgb", V_COLOR, V_COLOR);
	network = build_network(data);
	verbose("%rgbDone.\n%0rgb", V_COLOR);
	free_data(data);
	verbose("%d nodes detected in network. Node Name(%s) is entry. \
Node Name(%s) is exit.\n", network->nodes_count, network->entry->name, network->exit->name);
	verbose("%rgbSolving...\n%0rgb", V_COLOR);
	paths = solve(network);
	verbose("%rgbDone.\n%0rgb", V_COLOR);
	free_network(network);
	i = 0;
	while (paths[i].length)
		verbose_path(paths[i++]);
	run_ants(ants, paths);
	free_paths(paths);
}

int				main(int argc, char **argv)
{
	register_arg_ref("verbose", 'v', 0, ARG_VERBOSE);
	if (parse_args(argc, argv))
	{
		ft_putstr("Usage: lem-in [-v]\n");
		return (1);
	}
	lemin();
	destroy_args();
	return (0);
}
