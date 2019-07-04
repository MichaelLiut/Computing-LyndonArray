# Computing Lyndon Arrays
-----
As part of of my Ph.D. research I worked on developing two algorithms (_BSLA_ 
and _TRLA_) to compute Lyndon arrays. Herein, the C++ source code developed for 
the following algorithms: _IDLA_, _BSLA_, and _TRLA_. This code was used to take 
measurements and for empirical analysis. Details on these algorithms can be 
found on 
[my personal website](https://www.michaelliut.ca/publications--talks.html) as 
they become available (and are published). 

-----
### IDLA
This algorithm, an iterative approach, is based on Duval’s work on Lyndon 
factorization. _See lynarr.hpp_.

### BSLA
This algorithm is based on the ideas of Baier’s Suffix Sort (Phase I); this 
implementation necessarily differs. _See bsla.cpp_.

### TRLA
This algorithm is based on Farach's approach on his linear algorithm for suffix 
tree construction. The first idea of this algorithm was proposed by Paracha, 
but, has since been improved. _See trla.cpp and Tau.hpp_.

-----
## Supervisors and Research Laboratory

**Professor** [Frantisek (Franya) Franek](https://www.cas.mcmaster.ca/~franek/), 
Ph.D., RNDr., L.E.L.\
**Professor** [Antoine Deza](https://www.cas.mcmaster.ca/~deza/), Ph.D., P.Eng., 
Field Institute Fellow\
The Advanced Optimization Laboratory (AdvOL), McMaster University

-----
## Copyright

The work, herein, is Copyright 2016--2019.\
**No rights are given to reproduce or modify this work**.\
If you are seeking rights to use some of the content within, please contact me 
via email (send to: liutm [at] mcmaster.ca).

-----
-----

Michael Liut, M.Eng., B.A.Sc.\
Department of Computing and Software\
McMaster University\
Hamilton, Ontario, Canada\
[www.michaelliut.ca](https://www.michaelliut.ca/)

-----
-----