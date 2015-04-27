## Global
  * Change approach to non tristate and boolean configs. For now they are ignored and in their dependencies are not solved.
  * Implement modules support. For now modules are disabled.

##parse_kconfig
  * Rewrite symlist_find. We need faster implementation. Now implemented as linear search.
  * Write cnf_reduction. This function should reduce cnf expression. (A or A, A and A, not A or A, not A and A,...)
