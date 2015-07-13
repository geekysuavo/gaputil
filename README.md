# gaputil

A small utility to construct multidimensional nonuniform sampling schedules
using the generalized gap sampling framework published in:

> Worley, B., Powers, R., _Deterministic Multidimensional Nonuniform
> Gap Sampling_, Journal of Magnetic Resonance, 2015, In revision.

## Introduction

Gap sampling came into vogue in the field of Nuclear Magnetic Resonance (NMR)
when Hyberts and Wagner introduced their Poisson-gap (PG) sampler for building
Nonuniform Sampling (NUS) schedules. This project is the software realization
of a full generalization of their PG method to admit _any_ gap equation.

Initial versions of **gaputil** contained hard-coded Poisson-gap, sine-gap,
and sine-burst (_cf._ Worley and Powers, above) gap equations. The generality
of the proposed framework was a tempting opportunity for me to embed the
[Julia programming language] (http://julialang.org/) into one of my software
projects. By embedding Julia into my existing gap sampling algorithm, this
utility enables the construction of NUS schedules from _completely arbitrary_
gap equations.

### Examples

The **gaputil** program reads a gap equation from the command line in string
form, passes it to Julia for just-in-time compilation, and then evaluates it
as needed until an optimal schedule is arrived at. The function prototype
used by **gaputil** looks like this in Julia:

```julia
g(x::Float64, d::Int32,
  O::Array{Float64,1},
  N::Array{Float64,1},
  L::Float64)
```

where **x** holds the current gap sequence term, **d** is the currently
sampled grid dimension, **O** is the current origin in the grid, **N**
is the size of the grid, and **L** is a scaling factor that **gaputil**
will optimize in its attempts to create a schedule of the correct
global sampling density.

The simplest possible gap equation merely uses the scaling factor,
resulting in a uniform lattice:

```julia
g(x, d, O, N, L) = L
```

The sine-gap equation is fairly simple as well:

```julia
g(x, d, O, N, L) = L * sin((pi/2) * (x + sum(O)) / sum(N))
```

The sine-burst equation gets a bit more complicated:

```julia
g(x, d, O, N, L) = L * sin((pi/2) * (x + sum(O)) / sum(N)) *
sin((pi/4) * N[d] * (x + sum(O)) / sum(N))^2
```

In short, **gaputil** accepts almost any inline function having
Julia syntax.

### Installing

You will need to have Julia 0.4.0-dev compiled and installed in
order to build **gaputil**. It is recommended that you compile
and install from the latest GitHub nightly commit.

Once Julia is installed into the path, you can compile and install
**gaputil** as follows:

> git clone git://github.com/geekysuavo/gaputil.git

> cd gaputil

> make

> sudo make install

## Licensing

The **gaputil** project is released under the [GNU GPL 2.0] (
http://www.gnu.org/licenses/old-licenses/gpl-2.0.html).

Have fun with it,

*~ Brad.*

