## Compile

#### Cadical
./configure && make

#### Kissat
./configure && make

#### Maple
./starexec_build

#### pakis-mab-FO
make clean; make

#### p-mcomsps-FO
make clean; make


## Run
#### Cadical
build/cadical $instance

#### Kissat
build/kissat $instance

#### Maple
bin/MapleLCMDistChrBt-DL-v3 $instance


#### pakis-mab-FO
./pakissat $instance $threads $time-limit

#### p-mcomsps-FO
./painless-mcomsps -shr-sleep=750000 -c=$threads $instance