#include "Caracterisation.h"

void print_rule_name (rule * R, int sup,info_mining * im)
{
  std::string * varnames = im->DT_train->varnames;
  for (int i = 0; i < im->DT_train->nvar; i++)
    {
      if ((R->name[i/64] >> i%64) & 1UL)
        {
          std::cout << varnames[i] << " ";
        }
    }
  std::cout << "\n";

}

void check_rule_name (rule ** TOR, int nb_rules ,info_mining * im)
{
  std::string * varnames = im->DT_train->varnames;
  int maxul_train = im->DT_train->maxul;
  uint64_t * x = (uint64_t*)malloc(maxul_train*sizeof(uint64_t));
  int sum;
  uint64_t  Bone = 0; for (int n = 0; n < 64; n++) {Bone |= 1UL << n;}
  int sup_to_check = 400;
  for (int i = 0; i < nb_rules; i++)
    {
      sum = 0;
      fill_x(x,TOR[i],Bone,im);
      for (int n = 0; n < maxul_train; n++)
        {
          sum += __builtin_popcountl(x[n]);
        }
      if (sum == sup_to_check) {print_rule_name(TOR[i],sum,im);}
    }

  free(x);

}






void finish_SufRules (info_mining * im)
{
  int i,j,n,m;
  rule ** FR;
  rule * R_FR;
  int maxul_train = im->DT_train->maxul;
  int nb_mod = im->DT_train->nvar;
  uint64_t * supreme = (uint64_t*)malloc(maxul_train*sizeof(uint64_t));
  uint64_t * x = (uint64_t*)malloc(maxul_train*sizeof(uint64_t));
  uint64_t Bone = 0; for (i = 0; i < 64; i++) {Bone |= 1UL << i;}
  int nb_ind_train = im->DT_train->nb_ind;

  // Preparation de la recherche 
  im->IT->nvar_selec = nb_mod;
  for (i = 0; i < nb_mod;i++) {im->IT->order_var[i]=i;}
  im->IT->MSP = 1;
  //


  // recherche du nombre de manquants 
  FR = im->STKR->FR;
  for (n = 0; n < maxul_train; n++) {supreme[n] = 0;}

  for (i = 0; i < nb_mod+1; i++)
    {
      R_FR = FR[i];
      while (R_FR)
        {
          fill_x(x,R_FR,Bone,im);
          for (n = 0; n < maxul_train; n++) {supreme[n] = supreme [n] | x [n];}  
          R_FR = R_FR->brother;                
        }
    }

  uint64_t * Y = im->Y_1_eta[0]; 
  uint64_t *  tr_Y = (uint64_t*)calloc(maxul_train,sizeof(uint64_t));
  im->IT->Y_selec = tr_Y;
  im->IT->no_Y_selec = im->Y_1_eta[1];

  int nb_missing = 0;
   
  int sum_Y = 0; for (n = 0; n < maxul_train;n++) {sum_Y += __builtin_popcountl(Y[n]);} 
  for (n = 0; n < nb_ind_train;n++)
    {
      if ((Y[n/64] >> n%64) & 1UL) 
        {
          if ((supreme[n/64] >> n%64) & 1UL) {continue;}
          nb_missing++;
        }
    }

  ///////////////  
  sum_Y = 0; for (n = 0; n < maxul_train;n++) {sum_Y += __builtin_popcountl(Y[n]);} 
  int k = 0;
  order_variables(im,3);
  for (n = 0; n < nb_ind_train;n++)
    {
      if ((Y[n/64] >> n%64) & 1UL) 
        {
          if ((supreme[n/64] >> n%64) & 1UL) {continue;}
          search_rule_1 (n, supreme,im);    
        }
    }

// A partir d'ici c'est QUE DU CHECK 

FR = im->STKR->FR;
for (n = 0; n < maxul_train; n++) {supreme[n] = 0;}

  for (i = 0; i < nb_mod+1; i++)
    {
      R_FR = FR[i];
      while (R_FR)
        {
          fill_x(x,R_FR,Bone,im);
          for (n = 0; n < maxul_train; n++) {supreme[n] = supreme [n] | x [n];}  
          R_FR = R_FR->brother;                
        }
    }


 sum_Y = 0; for (n = 0; n < maxul_train;n++) {sum_Y += __builtin_popcountl(Y[n]);} 
 nb_missing = 0;;
 for (n = 0; n < nb_ind_train;n++)
    {
      if ((Y[n/64] >> n%64) & 1UL) 
        {
          if ((supreme[n/64] >> n%64) & 1UL) {continue;}
          nb_missing++;
        }
    }

  free(supreme); free(x); free(tr_Y);  
}

void RDP_Rec_SufRule (info_mining * im)
{
  im->IT->Y_selec = im->Y_1_eta[0];
  im->IT->no_Y_selec = im->Y_1_eta[1];
  
  // Passage de check a supprimer 
  int sum = 0; 
  int sumb = 0;
  
  for (int n = 0; n < im->DT_train->maxul; n++) 
    {
      sum += __builtin_popcountl(im->IT->Y_selec[n]);
      sumb += __builtin_popcountl(im->IT->no_Y_selec[n]);
    }

  int hekp = 0;
  if (im->IE->cur_eta != 0)
    {
      hekp = (int)(2*(float)1/(float)im->IE->cur_eta*(float)100000);
    }
  if ((float)sum <= hekp) {goto tsk_hekp;}

  for (int b = 0; b < im->IT->NB_BLOCK; b++)
   {
    
    im->IT->MSP = (int)((float)im->IT->TAB_MSP[b]*(float)im->IE->cur_sum_Y/(float)1000);
    if (im->IT->MSP == 0) {im->IT->MSP=2;}
    im->IT->NB_TREE = im->IT->TAB_NB_TREE[b];
    im->IT->NB_VAR_TREE = im->IT->TAB_NB_VAR_TREE[b];
    std::cout << "//////////////////////////////////////////////////////////////////////// \n";
    std::cout << "NB TREE = " << im->IT->NB_TREE << " NB VAR TREE = " << im->IT->NB_VAR_TREE << " MSP = " << im->IT->MSP << "\n";
    for (int s = 0; s < im->IT->NB_TREE; s++)
      {
        random_selection_var(im);
        order_variables(im,3);
        SufRules(im);
        update_FR_selec_2(im);
        clean_next_step_im(im);
      }
        
    }
   
  final_elag_simple(im);
  tsk_hekp : ;
  finish_SufRules(im);
  
}






int get_and_order_rules ( rule *** TOR, int ** TOS ,info_mining * im)
{
  int i,j,n;
  int nb_mod = im->DT_train->nvar;
  int maxul_train = im->DT_train->maxul;
  int nb_rules = 0;
  rule * R_FR;
  uint64_t * x_train = (uint64_t*)malloc(maxul_train*sizeof(uint64_t));
  uint64_t Bone; for (n = 0; n < 64; n++) {Bone |= 1UL << n;}

  for (i = 0; i < nb_mod+1;i++)
    {
      R_FR = im->STKR->FR[i];
      while (R_FR) {nb_rules++; R_FR = R_FR->brother;}
    }

  rule ** TOR_init = (rule**)malloc(nb_rules*sizeof(rule*));
  int *   TOS_init = (int*)malloc(nb_rules*sizeof(int));
  j = 0; int sum_rule = 0;
  for (i = 0; i < nb_mod+1; i++)
      {
        R_FR = im->STKR->FR[i];
        while (R_FR) // Premier bloc , on créé les valeurs et les vecteurs, et on calcule le full P1 
          {
            fill_x (x_train,R_FR,Bone, im);
            sum_rule = 0;
            for (n = 0;  n < maxul_train; n++) {sum_rule += __builtin_popcountl(x_train[n]);}
            TOR_init[j] = R_FR; 
            TOS_init[j] = sum_rule;
            j++;
            R_FR = R_FR->brother;
          }          
      }

  special_rule_order(TOR_init, TOS_init, nb_rules, maxul_train);
  (*TOR) = TOR_init;
  (*TOS) = TOS_init;
  free(x_train);
  return nb_rules;

}


void set_IR (rule ** TOR, int * is_in_supreme, int nb_rules , info_mining * im)
{
  int i,j,k,n;
  int nb_mod = im->DT_train->nvar;
  int nb_qualvarplus = im->IFV->nb_qualvar+1;
  std::unordered_map<int,int> * mappy_qual = im->IFV->mappy_qual;
  std::string * varnames = im->DT_train->varnames;

  int nb_rule_red = 0;
  for (i = 0; i < nb_rules; i++) {nb_rule_red = nb_rule_red + is_in_supreme[i];}
  
  rule ** rules_red = (rule**)malloc(nb_rule_red*sizeof(rule));
  std::string * Tab_name = new std::string [nb_rule_red];
  int * Tab_nb_mod = (int*)malloc(nb_rule_red*sizeof(int));
  int * qual_var_used = (int*)calloc(nb_qualvarplus,sizeof(int));
  int * binary_var_used = (int*)calloc(nb_mod,sizeof(int));

  std::string st;
  uint64_t * name;
  int nb_mod_used; int nb_qualvar;
  j = 0;

  for (i = nb_rules-1; i >= 0; i--)
    {
      if (is_in_supreme[i] == 0) {continue;}
      st = "";
      nb_mod_used = 0; nb_qualvar = 0;
      name = TOR[i]->name;

      for (n = 0; n < nb_mod; n++) 
        {
          if ((name[n/64] >> n%64) & 1UL)
            {
              st += varnames[n];
              st += ",";
              nb_mod_used++;
              k = mappy_qual->operator[](n);
              qual_var_used[k] = 1;
              binary_var_used[n] = 1;
            }
        } 

      rules_red [j] = TOR[i];
      Tab_nb_mod[j] = nb_mod_used;
      Tab_name[j] = st;
      j++;
    }

  ////
  
  nb_qualvar = 0; nb_mod_used = 0;
  for (n = 0; n < nb_qualvarplus; n++) {nb_qualvar = nb_qualvar + qual_var_used[n];}
  for (n = 0; n < nb_mod; n++) {nb_mod_used = nb_mod_used + binary_var_used[n];}  

  ///  

  im->IR->rules_red = rules_red;
  im->IR->nb_rule_red = nb_rule_red;
  im->IR->nb_mod = nb_mod_used;
  im->IR->nb_qualvar = nb_qualvar;  
  im->IR->Tab_nb_mod = Tab_nb_mod;
  im->IR->binary_var_used = binary_var_used;
  im->IR->qual_var_used = qual_var_used;
  im->IR->Tab_name = Tab_name;

}

void add_1_item_rule (rule ** TOR, int * is_in_supreme, int nb_rules ,info_mining * im)
{
  int i,n;
  int maxul_train = im->DT_train->maxul;
  uint64_t * x_train = (uint64_t*)malloc(maxul_train*sizeof(uint64_t));
  uint64_t * Supreme = (uint64_t*)calloc(maxul_train,sizeof(uint64_t));
  uint64_t Bone; for (n = 0; n < 64; n++) {Bone |= 1UL << n;}
  int size_rule = 0; 
  int nb_mod = im->DT_train->nvar;
  int sum_supreme = 0; int sum;

  for (i = nb_rules-1; i >= 0; i--)
    {
      size_rule = 0;
      for (n=0; n < nb_mod; n++)
        {
          if ((TOR[i]->name[n/64] >> n%64) & 1UL) {size_rule = size_rule+1;}
        }

      if (size_rule != 1) {continue;}
      fill_x(x_train,TOR[i],Bone,im);
      sum = 0;
      for (n = 0; n < maxul_train; n++) {sum += __builtin_popcountl(Supreme[n]|x_train[n]);}
      if ( sum == sum_supreme) {continue;}
      for (n = 0; n < maxul_train; n++) {Supreme[n] = Supreme[n] | x_train[n];}
      sum_supreme = sum;
      is_in_supreme[i] = 2;
    }

  free(Supreme);
  free(x_train);
}

void reduce_SR (rule *** TOR, int nb_rules, int nb_rule_SR ,int * is_in_supreme)
{
  rule ** TOR_init = *(TOR);
  rule ** TOR_reduce = (rule**)malloc(nb_rule_SR*sizeof(rule*));
  int k = 0;
  for (int i = 0; i < nb_rules;i++)
    {
      if (is_in_supreme[i] == 0) {continue;}
      TOR_reduce[k] = TOR_init[i];
      k++;
    }

  free(TOR_init);
  *(TOR) = TOR_reduce;
}

void update_IRF (info_mining * im)
{
  for (int i = 0; i < im->DT_train->nvar;i++)
    {
      if (im->IR->binary_var_used[i] == 1) {im->IRF->binary_var_used[i] = 1;}
    }

  for (int i = 0; i < im->IFV->nb_qualvar+1;i++)
    {
      if (im->IR->qual_var_used[i] == 1) {im->IRF->qual_var_used[i] = 1;}
    }

}

void Cac_Carac (info_mining * im)
{

  RDP_Rec_SufRule (im);
  rule ** TOR; int * TOS; 
  int nb_rules = get_and_order_rules (&TOR, &TOS, im);
  nb_rules = set_equiv_class (&TOR,&TOS,nb_rules,im);
  int * is_in_supreme = (int*)calloc(nb_rules,sizeof(int));
  int nb_rule_red;

  if (*(im->type_red) == "MDR") {nb_rule_red = MDR_RED (TOR, is_in_supreme, nb_rules,im);}
  if (*(im->type_red) == "MIR") {nb_rule_red = MIR_RED (TOR, is_in_supreme, nb_rules,im);}
  

  if (*(im->type_red) == "GMIR") 
    {
      int nb_rule_SR = SET_ORDER_RED (TOR, TOS ,is_in_supreme, nb_rules,im);
      reduce_SR (&TOR, nb_rules, nb_rule_SR ,is_in_supreme);
      nb_rule_red = MIR_RED (TOR,is_in_supreme, nb_rule_SR,im);
      nb_rules = nb_rule_SR;
    }

  if (*(im->type_red) == "GMDR") 
    {
      int nb_rule_SR = SET_ORDER_RED (TOR, TOS ,is_in_supreme, nb_rules,im);
      reduce_SR (&TOR, nb_rules, nb_rule_SR ,is_in_supreme);
      nb_rule_red = MDR_RED (TOR,is_in_supreme, nb_rule_SR,im);
      nb_rules = nb_rule_SR;
    }

  
  im->IR = (info_red*)malloc(sizeof(info_red));
  set_IR(TOR,is_in_supreme, nb_rules, im);
  if(im->IP->reg_or_pred == 'R') {update_IRF(im);}
  export_result(im);


  for (int i = 0; i < nb_rules; i++) {free(TOR[i]->name); free(TOR[i]);}
  free(TOR); free(TOS); free(is_in_supreme);

  return;
}



void Cac_Carac_all (info_mining * im, int eta_num)
{
  RDP_Rec_SufRule (im);
  rule ** TOR_all; int * TOS; 
  int nb_rules_all = get_and_order_rules (&TOR_all, &TOS, im);
  nb_rules_all = set_equiv_class (&TOR_all,&TOS,nb_rules_all,im);

  for (int i = 0; i < 4; i++)
    {
      rule ** TOR = (rule**)malloc(nb_rules_all*sizeof(rule*));
      for (int j = 0; j < nb_rules_all; j++) {TOR[j] = TOR_all[j];}
      int nb_rule_red;
      int nb_rules = nb_rules_all;

      int * is_in_supreme = (int*)calloc(nb_rules,sizeof(int));
      if (i == 0) {nb_rule_red = MDR_RED (TOR, is_in_supreme, nb_rules,im);}
      if (i == 1) {nb_rule_red = MIR_RED (TOR, is_in_supreme, nb_rules,im);}
      if (i == 2) 
        {
          int nb_rule_SR = SET_ORDER_RED (TOR, TOS ,is_in_supreme, nb_rules,im);
          reduce_SR (&TOR, nb_rules, nb_rule_SR ,is_in_supreme);
          nb_rule_red = MDR_RED (TOR,is_in_supreme, nb_rule_SR,im);
          nb_rules = nb_rule_SR;
        }

      if (i == 3)
        {
          int nb_rule_SR = SET_ORDER_RED (TOR, TOS ,is_in_supreme, nb_rules,im);
          reduce_SR (&TOR, nb_rules, nb_rule_SR ,is_in_supreme);
          nb_rule_red = MIR_RED (TOR,is_in_supreme, nb_rule_SR,im);
          nb_rules = nb_rule_SR;          
        }

      im->IR = im->LL->Tab_IR[i][eta_num];
      im->IRF = im->LL->Tab_IRF[i];
      set_IR(TOR,is_in_supreme, nb_rules, im);
      update_IRF(im);
      free(TOR);
      free(is_in_supreme);
    }

  for (int i = 0; i < nb_rules_all; i++) {free(TOR_all[i]->name); free(TOR_all[i]);}
  free(TOR_all); free(TOS);
}