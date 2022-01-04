#ifndef BRANCHPREDICTOR_SIM_BP_CLASSES_H
#define BRANCHPREDICTOR_SIM_BP_CLASSES_H

class BranchPredictor {
public:
    bool predict(unsigned long int address);
    void update(unsigned long int address, char outcome);
    void updateHistory(char outcome);
    BranchPredictor(unsigned long int n_in, unsigned long int m_in);
    void print();
    ~BranchPredictor();
private:
    unsigned long int global_history_mask;
    unsigned long int global_history;
    unsigned long int* counters;
    unsigned long int n;
    unsigned long int m;
};

class HybridBranchPredictor {
public:
    bool predict(unsigned long int address, char outcome);
    HybridBranchPredictor (unsigned long int n_in, unsigned long int gshare_m1_in, unsigned long int bimodal_m2_in, unsigned long int k_in);
    void print();
    ~HybridBranchPredictor();
private:
    BranchPredictor *gsharePredictor;
    BranchPredictor *bimodalPredictor;
    unsigned long int* counters;
    unsigned long int n;
    unsigned long int m1;
    unsigned long int m2;
    unsigned long int k;
};

#endif
