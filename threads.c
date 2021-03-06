/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchelsea <rchelsea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/27 15:52:42 by rchelsea          #+#    #+#             */
/*   Updated: 2021/07/27 15:53:54 by rchelsea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_if_dead(t_philo *ph, int *full_philos, int i)
{
	if ((get_time() - ph[i].last_ate) > (ph[0].all->time_to_die))
	{
		pthread_mutex_lock(&ph[0].all->flag_lock);
		ph[0].all->flag = 1;
		pthread_mutex_unlock(&ph[0].all->flag_lock);
		custom_print(&ph[i], (get_time() - ph->all->begin), "died\n", 2);
		return (1);
	}
	if (ph[0].all->num_of_meal != -1 && \
				(ph[i].num_eat == ph[0].all->num_of_meal \
				|| ph[i].num_eat > ph[0].all->num_of_meal))
		(*full_philos)++;
	return (0);
}

void	*life_check(void *p)
{
	t_philo		*ph;
	int			i;
	int			full_philos;

	ph = (t_philo *)p;
	while (1)
	{
		usleep(1000);
		i = -1;
		full_philos = 0;
		while (++i < ph[0].all->number_of_philo)
		{
			if (check_if_dead(ph, &full_philos, i) == 1)
				return (NULL);
		}
		if (full_philos == ph[0].all->number_of_philo)
		{
			pthread_mutex_lock(&ph[0].all->flag_lock);
			ph[0].all->flag = 1;
			pthread_mutex_unlock(&ph[0].all->flag_lock);
			pthread_mutex_lock(&ph->all->print);
			return (NULL);
		}
	}
}

void	philosopher_eats(t_philo *ph)
{
	pthread_mutex_lock(ph->left_fork);
	custom_print(ph, (long int)(get_time() - ph->all->begin),
		 "has taken a left fork\n", 1);
	pthread_mutex_lock(ph->right_fork);
	custom_print(ph, (long int)(get_time() - ph->all->begin),
		 "has taken a right fork\n", 1);
	custom_print(ph, (long int)(get_time() - ph->all->begin),
		 "is eating\n", 1);
	ph->last_ate = get_time();
	custom_sleep(ph->all->time_to_eat);
	pthread_mutex_unlock(ph->left_fork);
	pthread_mutex_unlock(ph->right_fork);
	ph->num_eat++;
}

void	*routine(void *i)
{
	t_philo	*ph;

	ph = (t_philo *)i;
	ph->last_ate = get_time();
	if (ph->all->number_of_philo == 1)
	{
		custom_print(ph, (long int)(get_time() - ph->all->begin),
			 "has taken a left fork\n", 1);
		custom_sleep(ph->all->time_to_die * 1000);
		return (NULL);
	}
	while (ph->all->flag == 0)
	{
		philosopher_eats(ph);
		custom_print(ph, (long int)(get_time() - ph->all->begin),
			 "is sleeping\n", 1);
		custom_sleep(ph->all->time_to_sleep);
		custom_print(ph, (long int)(get_time() - ph->all->begin),
			 "is thinking\n", 1);
	}
	return (NULL);
}

void	threads_init(t_philo *philo)
{
	pthread_t	checker;
	int			i;

	if (pthread_mutex_init(&philo->all->print, NULL))
		ft_error_exit("Failed to create mutex print");
	philo->ph = malloc(sizeof(pthread_t) * philo->all->number_of_philo);
	if (!philo->ph)
		ft_error_exit("Malloc error");
	i = -1;
	philo->all->begin = get_time();
	while (++i < philo->all->number_of_philo)
	{
		if (pthread_create(&philo->ph[i], NULL, &routine, (void *) &philo[i]))
			ft_free_error("Pthread_create error", philo);
		usleep(50);
	}
	if (pthread_create(&checker, NULL, &life_check, (void *)&philo[0]))
		ft_free_error("Pthread_create error", philo);
	philo->all->death_checker = checker;
}
