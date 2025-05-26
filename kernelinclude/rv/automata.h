/* SPDX-License-Identifier: GPL-2.0 */



#define DECLARE_AUTOMATA_HELPERS(name, type)					\
										\
 										\
static char *model_get_state_name_##name(enum states_##name state)		\
{										\
	if ((state < 0) || (state >= state_max_##name))				\
		return "INVALID";						\
										\
	return automaton_##name.state_names[state];				\
}										\
										\
										\
static char *model_get_event_name_##name(enum events_##name event)		\
{										\
	if ((event < 0) || (event >= event_max_##name))				\
		return "INVALID";						\
										\
	return automaton_##name.event_names[event];				\
}										\
										\
										\
static inline type model_get_initial_state_##name(void)				\
{										\
	return automaton_##name.initial_state;					\
}										\
										\
										\
static inline type model_get_next_state_##name(enum states_##name curr_state,	\
					       enum events_##name event)	\
{										\
	if ((curr_state < 0) || (curr_state >= state_max_##name))		\
		return INVALID_STATE;						\
										\
	if ((event < 0) || (event >= event_max_##name))				\
		return INVALID_STATE;						\
										\
	return automaton_##name.function[curr_state][event];			\
}										\
										\
										\
static inline bool model_is_final_state_##name(enum states_##name state)	\
{										\
	if ((state < 0) || (state >= state_max_##name))				\
		return 0;							\
										\
	return automaton_##name.final_states[state];				\
}
