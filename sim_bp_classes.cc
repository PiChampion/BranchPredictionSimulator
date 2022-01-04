#include "sim_bp_classes.h"
#include <stdio.h>

int iteration = 0;
int found_error = 0;

// Constructor for Bimodal/Gshare branch predictor
BranchPredictor::BranchPredictor(unsigned long int n_in, unsigned long int m_in) {
    unsigned int i;
    n = n_in;
    m = m_in;
    unsigned long int number_of_counters = (1 << m);

    global_history_mask = (1 << n) - 1;     // Initialize mask for only n bits of history register
    global_history = 0;
    counters = new unsigned long int[number_of_counters];

    // Initialize counters to 2
    for (i = 0; i < number_of_counters; i++)
        counters[i] = 2;
}

// Destructor class for the Branch Predictor
BranchPredictor::~BranchPredictor() {
    delete counters;
}

// Function for predicting the outcome of a branch based on the PC address
bool BranchPredictor::predict(unsigned long int address) {
    unsigned long int index;
    unsigned long int lower_address_mask = (1 << (m - n)) - 1;      // Mask for the lower bits of the PC which are concatenated
    bool final_prediction;

    // Calculate index for accessing counter array
    index = (((global_history & global_history_mask) ^ ((address >> (m - n + 2)) & global_history_mask)) << (m - n)) | ((address >> 2) & lower_address_mask);

    if(counters[index] >= 2)
        final_prediction = true;
    else
        final_prediction = false;

    return final_prediction;
}

// Function for updating the 2-bit counter array in a branch predictor
void BranchPredictor::update(unsigned long int address, char outcome) {
    unsigned long int index;
    unsigned long int lower_address_mask = (1 << (m - n)) - 1;

    index = (((global_history & global_history_mask) ^ ((address >> (m - n + 2)) & global_history_mask)) << (m - n)) | ((address >> 2) & lower_address_mask);

    if (outcome == 't') {
        if (counters[index] < 3) {
            counters[index]++;
        }
        global_history = (global_history >> 1) | (1 << (n - 1));
    }
    else {
        if (counters[index]) {
            counters[index]--;
        }
        global_history = (global_history >> 1) | (!(1 << (n - 1)));
    }
}

// Function for only updating the global history of a branch predictor
void BranchPredictor::updateHistory(char outcome) {
    if (outcome == 't') {
        global_history = (global_history >> 1) | (1 << (n - 1));
    }
    else {
        global_history = (global_history >> 1) | (!(1 << (n - 1)));
    }
}

// Function for printing the contents of a branch predictor
void BranchPredictor::print() {
    unsigned int i;
    unsigned long int number_of_counters = (1 << m);

    if(n)
        printf("FINAL GSHARE CONTENTS\n");
    else
        printf("FINAL BIMODAL CONTENTS\n");

    for (i = 0; i < number_of_counters; i++)
        printf(" %d\t%lu\n", i, counters[i]);
}

// Constructor for Hybrid branch predictor
HybridBranchPredictor::HybridBranchPredictor(unsigned long int n_in, unsigned long int gshare_m1_in, unsigned long int bimodal_m2_in, unsigned long int k_in) {
    unsigned int i;
    unsigned long int number_of_counters;

    n = n_in;
    m1 = gshare_m1_in;
    m2 = bimodal_m2_in;
    k = k_in;
	number_of_counters = (1 << k);
    gsharePredictor = new BranchPredictor(n, m1);            // Initialize the gshare branch predictor
    bimodalPredictor = new BranchPredictor(0, m2);      // Initialize the bimodal branch predictor
    counters = new unsigned long int[number_of_counters];

    // Initialize all entries in the chooser table to be 1
    for (i = 0; i < number_of_counters; i++)
        counters[i] = 1;
}

// Destructor for Hybrid branch predictor
HybridBranchPredictor::~HybridBranchPredictor() {
    delete counters;
    delete gsharePredictor;
    delete bimodalPredictor;
}


// Function for predicting the outcome of a branch based on the PC address
bool HybridBranchPredictor::predict(unsigned long int address, char outcome) {
    unsigned long int index;
    bool bimodal_prediction;
    bool gshare_prediction;
    bool final_prediction;

    // Obtain predictions from both branch predictors
    bimodal_prediction = bimodalPredictor->predict(address);
    gshare_prediction = gsharePredictor->predict(address);

    index = (address >> 2) & ((1 << k) - 1);
	
	if((counters[3] > 3) && (found_error == 0)) {
		printf("Found error on iteration %d\n", iteration);
		found_error = 1;
	}
	iteration++;
	
    if(counters[index] >= 2) {
        final_prediction = gshare_prediction;
        gsharePredictor->update(address, outcome);
    }
    else {
        final_prediction = bimodal_prediction;
        bimodalPredictor->update(address, outcome);
        gsharePredictor->updateHistory(outcome);        // Still need to update gshare history register no matter what
    }

    // Check to see if gshare was correct and bimodal wasn't correct
    if((bimodal_prediction && !gshare_prediction && (outcome == 'n')) || (!bimodal_prediction && gshare_prediction && (outcome == 't'))) {
        if (counters[index] < 3) {
            counters[index]++;
        }
    }
    // Check to see if bimodal was correct and gshare wasn't correct
    else if((!bimodal_prediction && gshare_prediction && (outcome == 'n')) || (bimodal_prediction && !gshare_prediction && (outcome == 't'))) {
        if (counters[index]) {
            counters[index]--;
        }
    }

    return final_prediction;
}

// Function for printing the contents of a branch predictor
void HybridBranchPredictor::print() {
    unsigned int i;
    unsigned long int number_of_counters = (1 << k);

    printf("FINAL CHOOSER CONTENTS\n");

    for (i = 0; i < number_of_counters; i++)
        printf(" %d\t%lu\n", i, counters[i]);

    // Also print the contents of the other branch predictors used
    gsharePredictor->print();
    bimodalPredictor->print();
}

