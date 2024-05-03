#include "Cac_init.h"
#include "Cac_Proc_selection.h"

#pragma optimization_level 3
#pragma GCC optimize("Ofast,no-stack-protector,unroll-loops,fast-math,O3")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx")
#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2,fma")
#pragma GCC optimization ("unroll-loops")

int main (int argc , char ** argv )
{

  info_prog * ip = (info_prog*)malloc(sizeof(info_prog));
  info_mining * im = (info_mining*)malloc(sizeof(info_mining));
  info_tree * IT = (info_tree*)malloc(sizeof(info_tree));
  im->pathname = (char*)argv[1];
  char  * trainpath = (char*)argv[1];
  std::string target ((char*)argv[2]);
  std::string delimparam ((char*)argv[3]);
  std::string MSP_param ((char*)argv[4]);
  std::string NB_TREE_param ((char*)argv[5]);
  std::string NB_VAR_TREE_param ((char*)argv[6]);
  std::string NB_BLOCK_param ((char*)argv[7]);
  std::string type_red ((char*)argv[8]);
  std::string type_tgt_param ((char*)argv[9]);
  ip->pathexport = NULL;
  if (argc > 10) {ip->pathexport = (char*)argv[10];}
 
  int j = 0; std::string param = "";
  while (NB_BLOCK_param[j] != '=') {j++;} j++;
  while (j < NB_BLOCK_param.size()) {param += NB_BLOCK_param[j]; j++;} IT->NB_BLOCK = std::stoi(param);

  im->type_red = &type_red;

  IT->TAB_MSP = (int*)malloc(IT->NB_BLOCK*sizeof(int));
  IT->TAB_NB_TREE = (int*)malloc(IT->NB_BLOCK*sizeof(int));
  IT->TAB_NB_VAR_TREE = (int*)malloc(IT->NB_BLOCK*sizeof(int));
 
  j = 0; int k = 0; param = "";
  while (MSP_param[j] != '=') {j++;} j++;
  while (j < MSP_param.size()) 
    {
     if (MSP_param[j] == '-') 
      {
        IT->TAB_MSP[k] = std::stoi(param);
        param = "";
        k++;
        j++;
        continue;
      }
     param += MSP_param[j]; j++;
    }
  IT->TAB_MSP[IT->NB_BLOCK-1] = std::stoi(param);
  
  j = 0;  k = 0; param = "";
  while (NB_TREE_param[j] != '=') {j++;} j++;
  while (j < NB_TREE_param.size()) 
    {
     if (NB_TREE_param[j] == '-') 
      {
        IT->TAB_NB_TREE[k] = std::stoi(param);
        param = "";
        k++;
        j++;
        continue;
      }
     param += NB_TREE_param[j]; j++;
    }
  IT->TAB_NB_TREE[IT->NB_BLOCK-1] = std::stoi(param);


  j = 0;  k = 0; param = "";
  while (NB_VAR_TREE_param[j] != '=') {j++;} j++;
  while (j < NB_VAR_TREE_param.size()) 
    {
     if (NB_VAR_TREE_param[j] == '-') 
      {
        IT->TAB_NB_VAR_TREE[k] = std::stoi(param);
        param = "";
        k++;
        j++;
        continue;
      }
     param += NB_VAR_TREE_param[j]; j++;
    }
  IT->TAB_NB_VAR_TREE[IT->NB_BLOCK-1] = std::stoi(param);    
  for (j = 0; j < IT->NB_BLOCK;j++)
    {
      std::cout << "NB TREE = " << IT->TAB_NB_TREE[k] << " NB VAR TREE = " << IT->TAB_NB_VAR_TREE[j] << " MSP = " << IT->TAB_MSP[j] << "\n";
    }


  //
  ip->take_neg = 0; if (target[0] == '-') {ip->take_neg = 1; target.erase(0,1);}
  ip->nb_var_target = 1;
  for (j = 0; j < target.size(); j++) {if(target[j] == ',') {ip->nb_var_target = ip->nb_var_target+1;}}
  ip->tab_tgt_complet_name = new std::string [ip->nb_var_target];
  int a = 0; param = "";
  for (j = 0; j < target.size(); j++)
    {
      if (target[j] == ',') {ip->tab_tgt_complet_name[a] = param; param = ""; a++; continue;}
      param += target[j];      
    }
  ip->tab_tgt_complet_name[a] = param;
  ip->tab_tgt_short_name = new std::string [ip->nb_var_target];
  for (j = 0; j < ip->nb_var_target;j++)
    {
      param = ""; a = 0;
      while (ip->tab_tgt_complet_name[j][a] != '.') {param += ip->tab_tgt_complet_name[j][a]; a++;}
      ip->tab_tgt_short_name[j] = param;
    }

  ip->rel_minsup = 0.0;
  if (delimparam[0]=='d' && delimparam[1]=='=')
    {
      ip->delim = delimparam[2];
    }
  else { std::cout << "Wrong second argument, please set d=yourdelim for separate item transactions \n"; return 0;}

  ip->reg_or_pred = type_tgt_param[0];
  ip->treshold_eta = 0.5;
  param = "";
  if (ip->reg_or_pred == 'P')
    {
      for (j = 1; j < type_tgt_param.size();j++) {param += type_tgt_param[j];}
      float tr_tresh = std::stof(param);
      ip->treshold_eta = (int) (tr_tresh*(float)100000);
    }

  dta * train_dta = (dta*)malloc(sizeof(dta));
  im->IT = IT;
  im->IP = ip;
  im->IP->tgt_name = &target;
  std::cout << "ip->nvar_target = " << ip->nb_var_target << "\n";
  Init_data (trainpath,ip, train_dta);
  std::cout << "avant test data \n";
  Init_info_mining (train_dta, im , ip);
  
  for (int i = 0; i < IT->NB_BLOCK; i++) 
    {
      std::cout << "Tab NB VAR TREE  = " <<  IT->TAB_NB_VAR_TREE[i] << " et nvar = " << im->DT_train->nvar << "\n";
    }

  if (ip->reg_or_pred == 'P') {set_target_pred(im);}  
  if (ip->reg_or_pred == 'R') {set_target_reg(im);} 
  if (ip->reg_or_pred == 'L') {set_target_reg(im);}
  im->IE->reg_or_pred = ip->reg_or_pred;
  CAC_selec(im);
  final_gest_mem (im);
  return 0;
}

