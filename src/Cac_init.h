#ifndef CAC_INIT_H
#define CAC_INIT_H


#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <random>

extern const size_t SUL ;
extern const size_t SI  ;
extern const size_t SPN ;
extern const size_t SRN ;




struct info_prog
{
  std::string * varnames;
  std::string * tgt_name;
  std::string * tab_tgt_complet_name;
  std::string * tab_tgt_short_name;
  char * pathexport;
  int  take_neg;
  int nb_var_target;
  int treshold_eta;
  char reg_or_pred;
  double rel_minsup;  
  double time;
  int nb_freq;
  int minsup;
  int nvar;
  int nrows; 
  int maxul;
  int mem_param;
  int n_param;
  char delim;
  char ordre;
};

struct dta 
{
  uint64_t ** X; 
  uint64_t ** Names;  
  std::string * varnames;
  int * order_var; 
  int nvar;
  int nb_ind;
  int maxul;
  int maxul_ind;
};

struct rule 
{
  uint64_t * name;
  rule * brother;

};

struct stock_rules
{
  rule *** TSR; // Tab Size Rule
  rule ** FR; // Final Rule 
};

struct ecv 
{
  rule * r;
  int * ptkeep;
  int size;
  int * tab_val;
};

struct qual_var
{
  short num_mod;
  qual_var * brother;
};


struct info_var
{
   std::unordered_map<int,int> * mappy_qual;  
   std::string * qual_var_name;
   qual_var ** tab_qual_var; 
   int nb_qualvar;
   int * rdm_order;
   int nb_rdm_var;

};


struct info_tree
{

  uint64_t ** X_train;
  uint64_t * Y_selec;
  uint64_t * no_Y_selec;

  int * TAB_MSP ; // le minsup //  
  int * TAB_NB_TREE;
  int * TAB_NB_VAR_TREE;

  int MSP;
  int NB_TREE;
  int NB_VAR_TREE;
  int NB_REL; // Le nombre de relance 
  int NB_BLOCK;
  int * order;
  int * order_var;
  int nvar_selec;
};

struct target
{
  uint64_t * Y_base; // Y non modifié
  uint64_t * Y; // Y eta 
  uint64_t * no_Y; //  non Y eta 
  
};

struct info_red
{
  int nb_rule_red;
  rule ** rules_red;
  std::string * Tab_name;
  int * Tab_nb_mod;
  int * qual_var_used;
  int * binary_var_used;
  int nb_mod;
  int nb_qualvar;
};

struct info_eta
{
  char reg_or_pred ;
  int nb_eta_values;
  int treshold_eta;
  uint64_t ** tab_Y_eta;
  int * eta_values;
  int * sum_Y_eta;
  int cur_eta;
  int cur_sum_Y;
};


struct Ynumbers 
{
  int nb_Y;
  int nb_no_Y;
};

struct Loul 
{
  info_red *** Tab_IR;
  info_red ** Tab_IRF;
};


struct info_mining
{
  char * pathname;
  dta * DT_train; // le nrow est destiné à bouger, il prend initialement celui du train 
  stock_rules * STKR;
  rule *** TSR;
  rule ** FR;
  info_var * IFV;
  info_tree * IT;
  info_red * IR;
  info_red * IRF;
  target * tgt_train;
  uint64_t ** Y_1_eta;
  std::string * type_red;
  info_eta * IE;
  info_prog * IP;
  Loul * LL;

};






void Init_data (char * pathfile,  info_prog * ip, dta * dt ) ;
void destruct_dta (dta * D);
void set_negativ (uint64_t * T, uint64_t * no_T, int deb, int fin);
void Init_info_mining (dta * train, info_mining * im , info_prog * ip);
void create_transpose_mat (uint64_t ** Bitdata, uint64_t ** Ind_Data, int nb_M,int maxul);
void set_nb_yorno (uint64_t ** Ind_Data, uint64_t * y_base, int max_ind, int maxul_ind, int * is_set, int i, int * num_commun , Ynumbers * Ynb );
void set_target_reg (info_mining * im);
void set_target_pred (info_mining * im);
void export_result (info_mining * im);
void export_IRF (info_mining * im);
void export_IRF_LL (info_mining * im);
void final_gest_mem (info_mining * im);


#endif