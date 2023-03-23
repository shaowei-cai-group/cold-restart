# Parameter settings for cold restarts

All the solvers in this directory are set to their best parameters in default.
The parameters set for each solver are listed as follows:

## FO
#### sc20
kissat_mab: 800000
cadical: 1000000
maple: 1000000
#### sc21
kissat_mab: 400000
cadical: 800000
maple: 1000000

## FP
#### sc20
kissat_mab: 1000000
cadical: 300000
maple: 800000
#### sc21
kissat_mab: 1000000
cadical: 300000
maple: 1000000

## FC
#### sc20 & sc21
kissat_mab: 700000
cadical: 600000
maple: 1000000

## Others (best version)
#### sc20
kissat_mab+FO+FC: 400000
cadical+FO+FC: 800000
maple+FO+FC: 1000000
maple+FO+FP: 400000
#### sc21
kissat_mab+FO+FC: 400000
cadical+FO+FC: 1000000
cadical+FO+FP: 1000000

## Parallel
pakis-mab-FO: 600000
pakis-mab-FO: 400000