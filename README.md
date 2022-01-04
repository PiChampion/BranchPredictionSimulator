# Branch Prediction Simulator

The project was completed as part of ECE 563 (Microprocessor Architecture) at NC State. In this project, I implemented a branch prediction simulator in C++ and used it to evaluate different configurations of branch predictors.

The simulator is able to simulate a bimodal pranch predictor, gshare branch predictor, and hybrid branch predictor. 
The bimodal branch predictor contains a prediction table which contains entries of 2-bit counters and is initialized to 2 ("weakly taken") when the simulation begins. The index into the prediction table is simply the branch's PC. 
For the gshare branch predictor, the index is based on both the branch’s PC and the global branch history register. The global branch history register is initialized to all zeroes at the beginning of the simulation. There is also the same prediction table consisting of 2-bit counters. 
The hybrid predictor selects between a bimodal and gshare predictor using a chooser table of 2-bit counters. All counters in the chooser table are initialized to 1 at the beginning of the simulation. If both the bimodal and gshare predictors were incorrect or both correct, no change is made in the chooser counter. If the bimodal predictor was incorrect and gshare predictor was correct, the chooser counter increments. If the bimodal predictor was correct and gshare predictor was incorrect, the chooser counter decrements.

### Command Line Specifications:
* To simulate a bimodal predictor: sim bimodal <M2> <tracefile>, where M2 is the number of PC bits used to index the bimodal table.
* To simulate a gshare predictor: sim gshare <M1> <N> <tracefile>, where M1 and N are the number of PC bits and global branch history register bits used to index the gshare table, respectively.
* To simulate a hybrid predictor: sim hybrid <K> <M1> <N> <M2> <tracefile>, where K is the number of PC bits used to index the chooser table, M1 and N are the number of PC bits and global branch history register bits used to index the gshare table (respectively), and M2 is the number of PC bits used to index the bimodal table.

### The simulator outputs the following:
* The simulator command.
* The following measurements:
  * number of predictions
  * number of branch mispredictions
  * branch misprediction rate
 * The final contents of the branch predictor.
