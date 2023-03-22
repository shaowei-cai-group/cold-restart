# Parameter settings for cold restarts

All the solvers in this directory are set to their best parameters in default.
The parameters set for each solver are listed as follows:

## FO
#### sc20
kissat_mab: shuffle=800000
cadical: reset_intv=1000000
maple: reset=1000000
#### sc21
kissat_mab: shuffle=400000
cadical: reset_intv=800000
maple: reset=1000000


## FP
#### sc20
kissat_mab: shuffle=1000000
cadical: reset_intv=300000
maple: reset=800000
#### sc21
kissat_mab: shuffle=1000000
cadical: reset_intv=300000
maple: reset=1000000

## FC
#### sc20 & sc21
kissat_mab: shuffle=700000
cadical: reset_intv=600000
maple: reset=1000000

## Others (best version)
#### sc20
kissat_mab+FO+FC: shuffle=400000
cadical+FO+FC: 800000
maple+FO+FC: 1000000
maple+FO+FP: reset=400000
#### sc21
kissat_mab+FO+FC: shuffle=400000
cadical+FO+FC: 1000000
cadical+FO+FP: 1000000