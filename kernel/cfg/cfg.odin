package cfg

DBG :: #config(DEBUG, false)
SMP :: #config(SMP, false)

dbg_assert :: proc "contextless" (
	condition: bool,
	message := #caller_expression(condition),
	loc := #caller_location,
) {
	when DBG {
		assert(condition, message, loc)
	}
}
