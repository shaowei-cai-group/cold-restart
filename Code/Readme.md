## Compile

#### Cadical
./configure && make

#### Kissat
./configure && make

#### Glucose
./starexec_build

#### Maple
./starexec_build


## Run
#### Cadical
build/cadical $instance

#### Kissat
build/kissat $instance

#### Glucose
bin/glucose $instance

#### Maple
bin/MapleLCMDistChrBt-DL-v3 $instance
