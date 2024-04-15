#include "Cac_Proc_selection.h"


void order_variables (info_mining * im, int tip)
{
  int i,j,n,k;
  int nvar = im->IT->nvar_selec;
  int * order_var = im->IT->order_var;
  uint64_t * Y = im->IT->Y_selec;
  uint64_t * ptul;
  float * values = (float*)malloc(nvar*sizeof(float));
  int maxul = im->DT_train->maxul;
  uint64_t ** X = im->DT_train->X;
  int p,r;
  int q = 0;
  float un = (float)1.00;
  
  for (n = 0; n < maxul; n++) {q += __builtin_popcountl(Y[n]);}

  float conf1,conf2,power1,power2;
 
  for (i = 0; i < nvar; i++)
    {
      p = 0; r = 0;
      ptul = X[order_var[i]];
      conf1 = 0; conf2 = 0; power1 = 0; power2 = 0;
      for (n = 0; n < maxul; n++)
        {
          p += __builtin_popcountl(ptul[n]);
          r += __builtin_popcountl(ptul[n] & Y[n]);
        }
        power1 = (float)r/(float)q;
        conf2  = (float)r/(float)p;
        power2 = (un-(float)p-(float)q+(float)r)/(un -(float)q);
        conf1 = (un-(float)p-(float)q+(float)r)/(un -(float)p);
        if ( tip == 1) {values[i] = conf2; continue;}
        if ( tip == 2) {values[i] = conf2; continue;}
        if ( tip == 3) {values[i] = power1; continue;}
        if ( tip == 4) {values[i] = conf2;}
        if ( tip == 10) {values[i] = (float)r;}
    }
  
  float val,temp;
  for (i = 0; i<nvar-1;i++)
  {
    n = i; val=values[i];
    for (j=i+1;j<nvar;j++) 
    {
      if (values[j]>val) {val = values[j];n=j;}
    }
    temp = values[n];values[n]=values[i];values[i]=temp;
    k = order_var[n]; order_var[n] = order_var[i]; order_var[i]=k;
  }

  free(values);

}



void set_info_mining (info_mining * im)
{
  im->IT->X_train = im->DT_train->X;
  im->Y_1_eta = (uint64_t**)malloc(2*sizeof(uint64_t*)); // en 0 Y , en 1 1-Y

  for (int i = 0; i < 2; i++)
    {
      im->Y_1_eta[i] = (uint64_t*)calloc(im->DT_train->maxul,sizeof(uint64_t));
    }

  for (int n = 0; n < im->DT_train->maxul; n++)
    {
      im->Y_1_eta[0][n] = im->tgt_train->Y[n];
    }

  for (int n = 0; n < im->DT_train->nb_ind; n++)
    {
      if ((im->Y_1_eta[0][n/64] >> n %64) & 1UL) {continue;}
      im->Y_1_eta[1][n/64] |= 1UL << n%64;
    }

  int nb_mod = im->DT_train->nvar; 

  
  im->IT->order = (int*)malloc(im->DT_train->nvar*sizeof(int));
  im->IT->order_var = (int*)malloc(im->DT_train->nvar*sizeof(int));
  im->STKR = (stock_rules*)malloc(sizeof(stock_rules));
  im->STKR->FR = (rule**)malloc((nb_mod+1)*sizeof(rule*));
  for (int k = 0; k < nb_mod+1; k++) {im->STKR->FR[k] = NULL;}
  im->STKR->TSR = (rule***)malloc((nb_mod+1)*sizeof(rule**));
  for (int k = 0; k < nb_mod; k++)
    {
      im->STKR->TSR[k] = (rule**)malloc((nb_mod+1)*sizeof(rule*));
      for (int i = 0; i < nb_mod+1; i++) {im->STKR->TSR[k][i] = NULL;}
    }
   
}



void init_info_var (info_mining * im)
{
  int i,j,k;
  im->IFV = (info_var*)malloc(sizeof(info_var));
  info_var * ifv = im->IFV;
  ifv->mappy_qual  = new std::unordered_map<int,int>();
  std::unordered_map<std::string,int> mappy;
  std::string * varnames = im->DT_train->varnames;

  int nb_qualvar = 1;
  std::string str; std::string str2;
  for (i = 0; i < im->DT_train->nvar; i++)
    {
      str = varnames[i];
      str2 = "";
      j = 0;
      while (str[j] != '.') {str2 += str[j]; j++;}
      if (mappy.count(str2) == 0) { mappy[str2] = nb_qualvar; nb_qualvar++;}
    }

  ifv->tab_qual_var = (qual_var**)malloc(nb_qualvar*sizeof(qual_var*));
  ifv->qual_var_name = new std::string [nb_qualvar];
  for (int i = 0; i < nb_qualvar; i++) {ifv->tab_qual_var[i] = NULL;}  
  for (i = 0; i < im->DT_train->nvar; i++)
    {
      str = varnames[i];
      str2 = "";
      j = 0;
      while (str[j] != '.') {str2 += str[j]; j++;}
      k = mappy[str2];
      ifv->qual_var_name[k] = str2;
      qual_var * qv = (qual_var*)malloc(sizeof(qual_var));
      qv->num_mod = i;
      qv->brother = ifv->tab_qual_var[k];
      ifv->tab_qual_var[k] = qv;
      ifv->mappy_qual->operator[](i) = k;
    }
  
  ifv->nb_qualvar = nb_qualvar-1;  
  ifv->rdm_order = (int*)malloc(ifv->nb_qualvar*sizeof(int));
  for (i = 0; i < ifv->nb_qualvar; i++) {ifv->rdm_order[i] = i+1;}
  for (int z = 1; z < ifv->nb_qualvar+1; z++)
    {
      qual_var * qv = ifv->tab_qual_var[z];
      std::cout << varnames[qv->num_mod] ;
      qv = qv->brother;
      while (qv) {std::cout << "  " << varnames[qv->num_mod]; qv = qv->brother;}
      std::cout << "\n";
    }
}

void random_selection_var (info_mining * im)
{ 
  int i,j,k;
  im->IFV->nb_rdm_var = im->IT->NB_VAR_TREE;
  int  * tab = im->IFV->rdm_order;
  int nb_qualvar = im->IFV->nb_qualvar;
  int nb_rdm_var = im->IFV->nb_rdm_var;
  srand(time(0));
  std::random_shuffle(&tab[0],&tab[im->IFV->nb_qualvar-1]);
  int nb_tot_mod = 0;
  qual_var * qv;
  for (i = 0; i < nb_rdm_var; i++)
    {
      qv = im->IFV->tab_qual_var[im->IFV->rdm_order[i]];
      while (qv) 
        {
          im->IT->order_var[nb_tot_mod++] = qv->num_mod;
          qv = qv->brother;
        }
    }
  im->IT->nvar_selec = nb_tot_mod;
}


void set_negation_Y_1_eta (info_mining * im)
{
  for (int i = 0; i < im->DT_train->maxul; i++) {im->Y_1_eta[1][i] = 0;}

  for (int i = 0; i < im->DT_train->nb_ind; i++)
    {
      if ((im->Y_1_eta[0][i/64] >> i%64) & 1UL) {continue;}
      im->Y_1_eta[1][i/64] |= 1UL << i%64;
    }

}

void clean_FR(info_mining * im)
{
  stock_rules * STKR = im->STKR;
  for (int k = 0; k < im->DT_train->nvar; k++)
    {
      for (int i = 0; i < im->DT_train->nvar+1; i++) {STKR->TSR[k][i] = NULL;}
    }

  for (int k = 0; k < im->DT_train->nvar+1; k++) {STKR->FR[k] = NULL;}

}


void CAC_selec (info_mining * im)
{
  set_info_mining(im);
  init_info_var(im);
  for (int i = 0; i < im->IT->NB_BLOCK; i++) {if (im->IT->TAB_NB_VAR_TREE[i] > im->IFV->nb_qualvar) {im->IT->TAB_NB_VAR_TREE[i] = im->IFV->nb_qualvar;}}
  if (im->IE->reg_or_pred == 'P')
    {
      im->IE->cur_eta = im->IP->treshold_eta;
      Cac_Carac (im);
      return;
    }

  if (im->IE->reg_or_pred == 'R')
    {
      im->IRF = (info_red*)malloc(sizeof(info_red));
      im->IRF->binary_var_used = (int*)calloc(im->DT_train->nvar,sizeof(int));
      im->IRF->qual_var_used = (int*)calloc(im->IFV->nb_qualvar+1,sizeof(int));
      for (int i = 0; i < im->IE->nb_eta_values; i++)
        {
          im->IE->cur_eta = im->IE->eta_values[i];
          for (int n = 0; n < im->DT_train->maxul; n++)
            {
              im->Y_1_eta[0][n] = im->IE->tab_Y_eta[i][n];
            }     
          im->IE->cur_sum_Y = im->IE->sum_Y_eta[i];
          set_negation_Y_1_eta(im);
          Cac_Carac(im);
          clean_FR (im); // a coder 
        }
      export_IRF(im);
    }

  if (im->IE->reg_or_pred == 'L')
    {

      im->LL = (Loul*)malloc(sizeof(Loul));
      im->LL->Tab_IR = (info_red***)malloc(4*sizeof(info_red**));
      im->LL->Tab_IRF = (info_red**)malloc(4*sizeof(info_red*));
      for (int i = 0; i < 4; i++)
        {
          im->LL->Tab_IR[i] = (info_red**)malloc(im->IE->nb_eta_values*sizeof(info_red*));
          for (int j = 0; j < im->IE->nb_eta_values; j++) {im->LL->Tab_IR[i][j] = (info_red*)malloc(sizeof(info_red));}
          im->LL->Tab_IRF[i] = (info_red*)malloc(sizeof(info_red)); 
          im->LL->Tab_IRF[i]->binary_var_used = (int*)calloc(im->DT_train->nvar,sizeof(int));
          im->LL->Tab_IRF[i]->qual_var_used = (int*)calloc(im->IFV->nb_qualvar+1,sizeof(int));
        }
;
      for (int i = 0; i < im->IE->nb_eta_values; i++)
        {
          im->IE->cur_eta = im->IE->eta_values[i];
          for (int n = 0; n < im->DT_train->maxul; n++)
            {
              im->Y_1_eta[0][n] = im->IE->tab_Y_eta[i][n];
            }          
          im->IE->cur_sum_Y = im->IE->sum_Y_eta[i];
          set_negation_Y_1_eta(im);
          Cac_Carac_all(im,i);
          clean_FR (im); // a coder 
        }
      export_IRF_LL(im);
    }

}



