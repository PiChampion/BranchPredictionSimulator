#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim_bp_classes.h"
#include "sim_bp.h"

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/

void initialize_params(bp_params* params, char* name);

int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;      // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file
    unsigned int predictions = 0;   // Variable to hold the total number of predictions so far
    unsigned int mispredictions = 0;   // Variable to hold the total number of mispredictions so far
    float mispredictionrate;      // Variable to hold the rate of mispredictions
    bool prediction = false;

    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }

    initialize_params(&params, argv[1]);

    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);

    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }

    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    char str[2];

    // Initialize the predictors for later use
    BranchPredictor myBimodialPredictor(params.N, params.M2);
    BranchPredictor myGsharePredictor(params.N, params.M1);
    HybridBranchPredictor myHybridPredictor(params.N, params.M1, params.M2, params.K);

    while(fscanf(FP, "%lx %s", &addr, str) != EOF) {
        outcome = str[0];       // Parse the actual outcome of the branch

        if(strcmp(params.bp_name, "bimodal") == 0) {
            prediction = myBimodialPredictor.predict(addr);     // Make a prediction for the address
            myBimodialPredictor.update(addr, outcome);          // Update the branch prediction based on the actual outcome
        }
        else if(strcmp(params.bp_name, "gshare") == 0) {
            prediction = myGsharePredictor.predict(addr);       // Make a prediction for the address
            myGsharePredictor.update(addr, outcome);            // Update the branch prediction based on the actual outcome
        }
        else if(strcmp(params.bp_name, "hybrid") == 0) {
            prediction = myHybridPredictor.predict(addr, outcome);  // Make a prediction for the address
        }

        // Check is there was a wrong prediction
        if((prediction && (outcome == 'n')) || (!prediction && (outcome == 't'))) {
            mispredictions++;
        }
        predictions++;
    }

    mispredictionrate = (float(mispredictions)/float(predictions))*100;     // Calculate misprediction rate

    printf("OUTPUT\n"
           " number of predictions:    %d\n"
           " number of mispredictions: %d\n"
           " misprediction rate:       %0.2f%%\n",
           predictions, mispredictions, mispredictionrate);

    if(strcmp(params.bp_name, "hybrid") == 0) myHybridPredictor.print();
    else if(strcmp(params.bp_name, "bimodal") == 0) myBimodialPredictor.print();
    else if(strcmp(params.bp_name, "gshare") == 0) myGsharePredictor.print();

	
	//printf("%lu\t%lu\t%0.2f",params.M1, params.N, mispredictionrate);
    return 0;
}

// Function for initializing the bp_params struct
void initialize_params(bp_params* params, char* name) {
    params->bp_name = name;
    params->M2 = 0;
    params->M1 = 0;
    params->N = 0;
    params->K = 0;
}



