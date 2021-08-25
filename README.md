# HPC simulation

Parallelization of a simulation of temperature variation on a metal plate, developed entirely in C.

The developement includes the necessary functionality to export the results (in terms of processing stats and time spent) to CSV in order to perform a posterior study of the efficiency of the libraries used.

The following have been used for this purpose
- phtreads
- MPI
- OpenMP
- Hybrid between MPI and OpenMP

**NOTE**: If it is necessary to see the results between iterations, change in `utils.h` the value of the constant `LOG_LEVEL` to `DEBUG`.

## Authors
<table>
<tr>
   <td align="center"><a href="https://github.com/GandalFran"><img src="https://avatars2.githubusercontent.com/u/29973536?s=460&u=b45b09f015e310153cd146b8903443c9d0080494&v=4" width="100px;" alt=""/><br /><sub><b>Francisco Pinto Santos</b></sub></a>
<td align="center"><a href="https://github.com/luisblazquezm"><img src="https://avatars0.githubusercontent.com/u/40697133?s=460&u=82f3e7d01e88b27ea481e57791fa62c9d519d2ac&v=4" width="100px;" alt=""/><br /><sub><b>Luis Blázquez Miñambres</b></sub></a>
  <td align="center"><a href="https://github.com/MiguelCabezasPuerto"><img src="https://avatars.githubusercontent.com/u/47638681?v=4" width="100px;" alt=""/><br /><sub><b>Miguel Cabezas Puerto</b></sub></a>
