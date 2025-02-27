/*
 * Copyright (C) 2007 by
 * 
 * 	Xuan-Hieu Phan
 *	hieuxuan@ecei.tohoku.ac.jp or pxhieu@gmail.com
 * 	Graduate School of Information Sciences
 * 	Tohoku University
 *
 * GibbsLDA++ is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * GibbsLDA++ is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GibbsLDA++; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

/* 
 * References:
 * + The Java code of Gregor Heinrich (gregor@arbylon.net)
 *   http://www.arbylon.net/projects/LdaGibbsSampler.java
 * + "Parameter estimation for text analysis" by Gregor Heinrich
 *   http://www.arbylon.net/publications/text-est.pdf
 */

#ifndef	_MODEL_H
#define	_MODEL_H

#include "constants.h"
#include "dataset.h"

using namespace std;

// LDA model
class model {
public:
    // fixed options
    string wordmapfile;		// file that contains word map [string -> integer id]
    string trainlogfile;	// training log file
    string tassign_suffix;	// suffix for topic assignment file
    string theta_suffix;	// suffix for theta file
    string phi_suffix;		// suffix for phi file
    string others_suffix;	// suffix for file containing other parameters
    string twords_suffix;	// suffix for file containing words-per-topics

    string aassign_suffix;  ////////// suffix for author assignment file
    string atopics_suffix;   // suffix for file containing words-per-topics

    string dir;			// model directory
    string dfile;		// data file    
    string model_name;		// model name
    int model_status;		// model status:
				// MODEL_STATUS_UNKNOWN: unknown status
				// MODEL_STATUS_EST: estimating from scratch
				// MODEL_STATUS_ESTC: continue to estimate the model from a previous one
				// MODEL_STATUS_INF: do inference

    dataset * ptrndata;	// pointer to training dataset object
    dataset * pnewdata; // pointer to new dataset object

    mapid2word id2word; // word map [int => string]
    
    // --- model parameters and variables ---    
    int M; // dataset size (i.e., number of docs)
    int V; // vocabulary size
    int K; // number of topics
    double alpha, beta; // LDA hyperparameters 
    int A; 
    int A_perD; 
    int niters; // number of Gibbs sampling iterations
    int liter; // the iteration at which the model was saved
    int savestep; // saving period
    int twords; // print out top words per each topic
    int withrawstrs;
    double * p_pa;
    double * p; // temp variable for sampling
    double * pa;
    int ** z; // topic assignments for words, size M x doc.size()
    int ** x; ////// author assignments for words, , size M x doc.size()
    int ** nw; // cwt[i][j]: number of instances of word/term i assigned to topic j, size V x K
    int ** nd; // nd[i][j]: number of words in document i assigned to topic j, size M x K
    int ** na;  /////////// na[i][j]: number of words in author i assigned to topic j size A x K
    int * nwsum; // nwsum[j]: total number of words assigned to topic j, size K
    int * ndsum; // nasum[i]: total number of words in document i, size M
    int * nasum; ////// size A 
    double ** theta; // theta: author-topic distributions, size A x K  ////////////
    double ** phi; // phi: topic-word distributions, size K x V
    int tem_iter;
    float * alpha_tim;
    float * beta_tim;


    double * beta_phi;
    double * alpha_theta;
    
    // for inference only
    int inf_liter;
    int newM;
    int newV;
    int newA;  ///////
    int ** newz;
    int ** newx;  ///////
    int ** newnw;
    int ** newnd;
    int ** newna;    //////
    int * newnwsum;
    int * newndsum;
    int * newnasum;   //////
    double ** newtheta;
    double ** newphi;
    // --------------------------------------
    
    model() {
	set_default_values();
    }
          
    ~model();
    
    // set default values for variables
    void set_default_values();   

    // parse command line to get options
    int parse_args(int argc, char ** argv);
    
    // initialize the model
    int init(int argc, char ** argv, int start, int end);
    
    // load LDA model to continue estimating or to do inference
    int load_model(string model_name);
    ///  /////////////////////// load author of lda model
    int load_model_author(string model_name);
    
    // save LDA model to files
    // model_name.tassign: topic assignments for words in docs
    // model_name.theta: document-topic distributions
    // model_name.phi: topic-word distributions
    // model_name.others: containing other parameters of the model (alpha, beta, M, V, K)
    int save_model(string model_name,int start, int end);
    int save_model_temp(string model_name,int start, int end);

    int save_model_tassign(string filename,int start, int end);
    int save_model_theta(string filename,int start, int end);
    int save_model_phi(string filename,int start, int end);
    int save_model_others(string filename,int start, int end);
    int save_model_twords(string filename,int start, int end);

    int save_model_aassign(string filename,int start, int end);   //////////
    int save_model_atopics(string filename,int start, int end);    ///////////
    
    // saving inference outputs
    int save_inf_model(string model_name);
    int save_inf_model_tassign(string filename);
    int save_inf_model_newtheta(string filename);
    int save_inf_model_newphi(string filename);
    int save_inf_model_others(string filename);
    int save_inf_model_twords(string filename);

    int save_inf_model_aassign(string filename);   //////////
    int save_inf_model_atopics(string filename);    ///////////
    
    // init for estimation
    int init_est(int start, int end);
    int init_estc();
	
    // estimate LDA model using Gibbs sampling
    void estimate(int start, int end);
    pair<int, int> sampling(int m, int n);   /////////////////
    void compute_theta();
    void compute_phi();

    void update_theta();
    void update_phi(int start, int end);
    void update_alpha();
    void update_beta(int start, int end);

    
    // init for inference
    int init_inf();
    // inference for new (unseen) data based on the estimated LDA model
    void inference();
    pair<int, int> inf_sampling(int m, int n);
    void compute_newtheta();
    void compute_newphi();
    long double digammal(long double x);
};

#endif

