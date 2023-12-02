# Optimizations

- Count the number of lines minus the number of blank lines and allocate that many SF_AST_NODE's up front in the \_sf_build_ast function. Then request a copy out of the pool when you need one. Currently we allocate then as need need them which is generally bad for performance.
