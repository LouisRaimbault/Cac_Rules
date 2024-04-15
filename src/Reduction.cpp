#include "Reduction.h"



void fill_x (uint64_t * x,rule * R_FR, uint64_t Bone, info_mining * im)
{
  int n,m;
  int maxul_train = im->DT_train->maxul;
  uint64_t ** X_train = im->DT_train->X;
  int nb_mod = im->DT_train->nvar;
  uint64_t * ptul;
  uint64_t * name = R_FR->name;
  for (n = 0; n < maxul_train; n++) {x[n] = Bone;}
    for (m = 0; m < nb_mod; m++)
      {
        if ((name[m/64] >> m%64) & 1UL) 
          {
            ptul = X_train[m];
            for (n = 0; n < maxul_train; n++) {x[n] = x[n] & ptul[n];}
          }
      }
}


int RDP_short_reduction (rule ** TOR, int * is_in_supreme,int nb_rules, info_mining * im)
{
  int n;
  int maxul_train = im->DT_train->maxul;
  uint64_t * Supreme = (uint64_t*)calloc(maxul_train,sizeof(uint64_t));
  uint64_t * x_train = (uint64_t*)malloc(maxul_train*sizeof(uint64_t));
  int nb_rule_red = 0; int sum; int sum_supreme = 0;
  uint64_t  Bone = 0; for (n = 0; n < 64; n++) {Bone |= 1UL << n;}
  for (int i = 0; i < nb_rules; i++) {is_in_supreme[i] = 0;}
  for (int i = 0; i < nb_rules; i++)
    {
      if (is_in_supreme[i] == 2)
        {
          fill_x(x_train, TOR[i],Bone,im);
          for (n = 0; n < maxul_train; n++) {Supreme[n] = Supreme[n] | x_train[n];}
        }
    }
  sum_supreme = 0; for (n = 0; n < maxul_train; n++) {sum_supreme += __builtin_popcountl(Supreme[n]);}
  for (int i = nb_rules-1; i >= 0; i-- )
    {
      fill_x(x_train,TOR[i],Bone,im);
      sum = 0;
      for (n = 0; n < maxul_train; n++) {sum += __builtin_popcountl(x_train[n]|Supreme[n]);}
      if (sum == sum_supreme) {continue;}
      is_in_supreme[i] = 1;
      nb_rule_red = nb_rule_red+1;
      sum_supreme = sum;
      for (n = 0; n < maxul_train; n++) {Supreme[n] = Supreme[n] | x_train[n];}
    }
  free(Supreme);
  free(x_train);
  return nb_rule_red;
}




int SET_ORDER_RED (rule ** TOR, int * TOS , int * is_in_supreme ,int nb_rules, info_mining * im)
{
  int i,j,n;
  int nb_rules_red = 0;
  int maxul_train = im->DT_train->maxul;
  uint64_t * Supreme = (uint64_t*)calloc(maxul_train,sizeof(uint64_t));
  uint64_t * x_train = (uint64_t*)calloc(maxul_train,sizeof(uint64_t));
  int * optim_add_rule = (int*)calloc(nb_rules,sizeof(int));
  for (i = 0; i < nb_rules; i++) {optim_add_rule[i] = maxul_train*64;}
  uint64_t Bone = 0; for (n = 0; n < 64; n++) {Bone |= 1Ul << n;}
  for (i = 0; i < nb_rules; i++) {is_in_supreme[i] = 0;}
  int max_add = 2;
  int sum_add;
  int sum;
  int sum_SRED = 0;
  int nma;
  int sz,szb;
  uint64_t * ptul;
  for (int i = 0; i < nb_rules; i++)
  {
    if (is_in_supreme[i] == 2)
      {
        fill_x(x_train, TOR[i],Bone,im);
        for (n = 0; n < maxul_train; n++) {Supreme[n] = Supreme[n] | x_train[n];}
      }
  }
  sum_SRED = 0; for (n = 0; n < maxul_train; n++) {sum_SRED += __builtin_popcountl(Supreme[n]);}

  while (max_add > 0)
    {
      max_add = 0;
      for (j = nb_rules-1; j>= 0; j--)
        {
          if (optim_add_rule[j] <= max_add) {continue;}
          if (is_in_supreme[j] != 0) {continue;}
          sum = 0;
          fill_x(x_train,TOR[j],Bone,im);
          for (n = 0; n < maxul_train; n++) {sum += __builtin_popcountl(x_train[n]|Supreme[n]);}
          sum_add = sum-sum_SRED;
          optim_add_rule[j] = sum_add;
          if (sum_add < max_add || sum_add == 0) {continue;}
          if (sum_add == max_add)
            {
              if (TOS[j] < TOS[nma]) {continue;}
              sz = 0; szb = 0;
              for (n = 0; n < im->DT_train->maxul_ind; n++)
                {
                  sz  += __builtin_popcountl(TOR[nma]->name[n]);
                  szb += __builtin_popcountl(TOR[j]->name[n]);
                }
              if (szb >= sz ) {continue;}

            }
          nma = j;
          max_add = sum_add;
        }
      if (max_add == 0) {break;}
      is_in_supreme[nma] = 1;
      fill_x(x_train,TOR[nma],Bone,im);
      nb_rules_red = nb_rules_red+1;
      for (n = 0; n < maxul_train; n++) {Supreme[n] = Supreme[n] | x_train[n];}
      sum_SRED = 0;
      for (n = 0; n < maxul_train; n++) {sum_SRED += __builtin_popcountl(Supreme[n]);}
    }
 
  free(Supreme);
  free(x_train);
  free(optim_add_rule);
  return nb_rules_red;
}


int MIR_RED (rule ** TOR, int * is_in_supreme ,int nb_rules, info_mining * im)
{
  int n;
  int maxul_train = im->DT_train->maxul;
  uint64_t * Supreme = (uint64_t*)calloc(maxul_train,sizeof(uint64_t));
  uint64_t * x_train = (uint64_t*)calloc(maxul_train,sizeof(uint64_t));
  int sum; int sumb; int sum_supreme = 0;
  for (int i = 0; i < nb_rules; i++) {is_in_supreme[i] = 0;}
  uint64_t Bone = 0; for (n = 0; n < 64; n++) {Bone |= 1Ul << n;}
  int * TR_TOR = (int*)malloc(nb_rules*sizeof(int));
  uint64_t ** TR_Supreme = (uint64_t**)malloc(nb_rules*sizeof(uint64_t*));
  for (int i = 0; i < nb_rules; i++) {TR_Supreme[i] = NULL;}
  int nb_rule_red = 0;
  sum_supreme = 0; 


  for (int i = nb_rules-1; i >= 0; i--)
    {
      fill_x(x_train,TOR[i],Bone,im);
      sum = 0;
      for (n = 0; n < maxul_train; n++) {sum += __builtin_popcountl(x_train[n]|Supreme[n]);}
      if (sum > sum_supreme) 
        {
          is_in_supreme[i] = 1;
          TR_TOR[nb_rule_red] = i;
          TR_Supreme[i] = (uint64_t*)malloc(maxul_train*sizeof(uint64_t));
          for (n = 0; n < maxul_train; n++) {TR_Supreme[i][n] = Supreme[n];}
          nb_rule_red = nb_rule_red+1;
          sum_supreme = sum;          
        }

      for (n = 0; n < maxul_train; n++) {Supreme[n] = Supreme[n] | x_train[n];}
    }
  for (n = 0; n < maxul_train; n++) {Supreme[n] = 0;}

  sum_supreme = 0; 


  int final_rule_red = 0;
  for (int i = nb_rule_red-1 ; i >= 0; i--)
    {
      //print_rule_name(TOR[TR_TOR[i]],0,im);
      uint64_t * S = TR_Supreme[TR_TOR[i]];
      fill_x(x_train,TOR[TR_TOR[i]],Bone,im);
      sum = 0; sumb = 0;
      for (n = 0; n < maxul_train; n++) 
        {
          sum += __builtin_popcountl(x_train[n] | Supreme[n] | S[n]);
          sumb += __builtin_popcountl(Supreme[n] | S[n]);
        }

      if (sum > sumb)
        {
          for (n = 0; n < maxul_train; n++) {Supreme[n] = Supreme[n] | x_train[n];}
          final_rule_red++;
          continue;
        }
      is_in_supreme[TR_TOR[i]] = 0;
    }

  free(Supreme); free(x_train);
  for (int i = 0; i < nb_rules; i++) { if(TR_Supreme[i] != NULL) {free(TR_Supreme[i]);}}
  free(TR_Supreme); free(TR_TOR);
  return final_rule_red;

}

int MDR_RED (rule ** TOR, int * is_in_supreme ,int nb_rules, info_mining * im)
{
  int n;
  int maxul_train = im->DT_train->maxul;
  uint64_t * Supreme = (uint64_t*)calloc(maxul_train,sizeof(uint64_t));
  uint64_t * x_train = (uint64_t*)calloc(maxul_train,sizeof(uint64_t));
  int sum; int sumb; int sum_supreme = 0;
  for (int i = 0; i < nb_rules; i++) {is_in_supreme[i] = 0;}
  uint64_t Bone = 0; for (n = 0; n < 64; n++) {Bone |= 1Ul << n;}

  int * TR_TOR = (int*)malloc(nb_rules*sizeof(int));
  uint64_t ** TR_Supreme = (uint64_t**)malloc(nb_rules*sizeof(uint64_t*));
  for (int i = 0; i < nb_rules; i++) {TR_Supreme[i] = NULL;}
  int nb_rule_red = 0;


  sum_supreme = 0;
  for (int i = 0; i < nb_rules; i++)
    {
      fill_x(x_train,TOR[i],Bone,im);
      sum = 0;
      for (n = 0; n < maxul_train; n++) {sum += __builtin_popcountl(x_train[n]|Supreme[n]);}
      if (sum > sum_supreme) 
        {
          is_in_supreme[i] = 1;
          TR_TOR[nb_rule_red] = i;
          TR_Supreme[i] = (uint64_t*)malloc(maxul_train*sizeof(uint64_t));
          for (n = 0; n < maxul_train; n++) {TR_Supreme[i][n] = Supreme[n];}
          nb_rule_red = nb_rule_red+1;
          sum_supreme = sum;          
        }

      for (n = 0; n < maxul_train; n++) {Supreme[n] = Supreme[n] | x_train[n];}
    }
  for (n = 0; n < maxul_train; n++) {Supreme[n] = 0;}

  sum_supreme = 0; 

  int final_rule_red = 0;
  for (int i = nb_rule_red-1; i >= 0 ; i--)
    {
      //print_rule_name(TOR[TR_TOR[i]],0,im);
      uint64_t * S = TR_Supreme[TR_TOR[i]];
      fill_x(x_train,TOR[TR_TOR[i]],Bone,im);
      sum = 0; sumb = 0;
      for (n = 0; n < maxul_train; n++) 
        {
          sum += __builtin_popcountl(x_train[n] | Supreme[n] | S[n]);
          sumb += __builtin_popcountl(Supreme[n] | S[n]);
        }

      if (sum > sumb)
        {
          for (n = 0; n < maxul_train; n++) {Supreme[n] = Supreme[n] | x_train[n];}
          final_rule_red++;
          continue;
        }
      is_in_supreme[TR_TOR[i]] = 0;
    }
  free(Supreme); free(x_train); 
  for (int i = 0; i < nb_rules; i++) {if (TR_Supreme[i] != NULL) {free(TR_Supreme[i]);}}
  free(TR_Supreme); free(TR_TOR);
  return final_rule_red;

}


void CHECK_IECLASS (ecv ** Tab_ecv, int nb, info_mining * im)
{
  int i,j,n,sum;
  int maxul_train = im->DT_train->maxul;

  uint64_t * rule_class = (uint64_t*)malloc(maxul_train*sizeof(uint64_t));
  uint64_t * x = (uint64_t*)malloc(maxul_train*sizeof(uint64_t));
  uint64_t Bone; for (n = 0; n < 64; n++) {Bone |= 1UL << n;}
  fill_x(x,Tab_ecv[0]->r,Bone,im);
  int sum_rc = 0; 
  for (n = 0; n < maxul_train; n++) 
    {
      rule_class[n] = x[n]; 
      sum_rc += __builtin_popcountl(rule_class[n]);
    }
  int curkeep = 0; int cursize = Tab_ecv[0]->size;  
  int nb_other = 0;
  for (i = 1; i < nb; i++)
    {
      sum = 0; 
      fill_x(x,Tab_ecv[i]->r,Bone,im);
      for (n = 0; n < maxul_train; n++) {sum += __builtin_popcountl(x[n] & rule_class[n]);}
      if (sum != sum_rc) {Tab_ecv[nb_other] = Tab_ecv[i]; nb_other++; continue;}
      if (Tab_ecv[i]->size < cursize)
        {
          *(Tab_ecv[curkeep]->ptkeep) = 0;
          curkeep = i;
          cursize = Tab_ecv[i]->size;
          continue; 
        }

      *(Tab_ecv[i]->ptkeep) = 0; 
    }

   free(rule_class);
   free(x);

  if (nb_other > 1) {CHECK_IECLASS(Tab_ecv,nb_other,im);}


}

void REC_IECLASS (ecv ** Tab_ecv, int nb,int num_func, info_mining * im)
{
  int i,j,nbr;
  int val_f;
  int maxul_train = im->DT_train->maxul;
  int * tval_f = (int*)malloc(nb*sizeof(int)); for (i = 0; i < nb;i++) { tval_f[i] = Tab_ecv[i]->tab_val[num_func];}
  special_ecv_order (Tab_ecv, tval_f, nb,maxul_train);
  i = 0;
  while (i < nb-1)
    {
      nbr = 1;
      val_f = tval_f[i];
      j = i+1;
      while (tval_f[j] == val_f) {nbr++; j++; if (j == nb) {break;}}
      ecv ** tr_tab_ecv = (ecv**)malloc(nbr*sizeof(ecv*));
      nbr = 0;
      for (int w = i; w < j; w++) {tr_tab_ecv[nbr] = Tab_ecv[w]; nbr++;}
      if (num_func < 2) {REC_IECLASS(tr_tab_ecv,nbr,num_func+1,im); free(tr_tab_ecv); i = i+nbr; continue;}
      CHECK_IECLASS (tr_tab_ecv,nbr,im);
      free(tr_tab_ecv);
      i = i+nbr;
    }
  free(tval_f);
}

void IECLASS (rule ** TR_TOR, int * TR_TOSR ,int nb, int ** pt_tokeep, int sup ,info_mining * im)
{
  int i,j,n;
  ecv ** Tab_ecv =  (ecv**) malloc(nb*sizeof(ecv*));
  int maxul_train= im->DT_train->maxul;
  uint64_t * x = (uint64_t*)malloc(maxul_train *sizeof(uint64_t));
  uint64_t Bone; for (n = 0; n < 64; n++) {Bone |= 1UL << n;}
  ecv * pt_ecv;

  for (i = 0; i < nb; i++)
    {
      Tab_ecv[i] = (ecv*)malloc(sizeof(ecv));
      Tab_ecv[i]->tab_val = (int*)calloc(3,sizeof(int));
      Tab_ecv[i]->r = TR_TOR[i];
      Tab_ecv[i]->ptkeep = pt_tokeep[i];
      Tab_ecv[i]->size = TR_TOSR[i];
      pt_ecv = Tab_ecv[i];

      fill_x(x,TR_TOR[i],Bone,im);
      j = 0;
      for (n = 0; n < (maxul_train*64); n++)
        {
          if ((x[n/64] >> n%64) & 1UL)
            {
              pt_ecv->tab_val[0] = pt_ecv->tab_val[0] + n;
              pt_ecv->tab_val[1] = pt_ecv->tab_val[1] + j*n;
              pt_ecv->tab_val[2] = pt_ecv->tab_val[2] + n*n;
              j = j+1;
            }
        }

    }
  free(x);  
  REC_IECLASS (Tab_ecv, nb,0,im);
  for (i = 0; i < nb; i++) {free(Tab_ecv[i]->tab_val); free(Tab_ecv[i]);}
  free(Tab_ecv);

  
}


int set_equiv_class (rule *** TOR, int ** TOS, int nb_rules  , info_mining * im)
{
  int i,j,k,l,n,sum,sumb,size;
  int nb_equiv_class = 0;
  int maxul = im->DT_train->maxul;

  rule ** TOR_init = *(TOR);
  int * TOS_init = *(TOS);
  int * TOSR = (int*)malloc(nb_rules*sizeof(int));
  int * tokeep = (int*)malloc(nb_rules*sizeof(int));
  uint64_t ** Tabul = (uint64_t**)malloc(nb_rules*sizeof(uint64_t*));
  uint64_t * x;
  uint64_t * y;
  uint64_t Bone; for (n = 0; n < 64; n++) {Bone |= 1UL << n;}
  for (i = 0; i < nb_rules; i++)
    {
      tokeep[i] = 1;
      size = 0;
      for (n = 0; n < im->DT_train->maxul_ind;n++) {size += __builtin_popcountl(TOR_init[i]->name[n]);}
      TOSR[i] = size;
    }
  
  for (i = 0; i < nb_rules-1; i++)
    {
    
    if (tokeep[i] == 0) {continue;}
    sum = TOS_init[i];
    j = i+1; k = 1;
    while (TOS_init[j] == sum) 
      { 
        k++; j++; if (j >= nb_rules) {break;}
      }
    rule ** TR_TOR = (rule**)malloc(k*sizeof(rule*));
    int ** pt_tokeep = (int**)malloc(k*sizeof(int*));
    int * TR_TORS = (int*)malloc(k*sizeof(int));
    l = 0;
    for (j = i; j < i+k; j++)
      {
        TR_TOR[l] = TOR_init[j];
        pt_tokeep[l] = &(tokeep[j]);
        TR_TORS[l] = TOSR[j];
        l = l+1;
      }
    
    IECLASS (TR_TOR, TR_TORS , k, pt_tokeep, sum , im);
    free(TR_TOR); free(pt_tokeep); free(TR_TORS);
    i = i+k-1;

    }
  
  
  for (i = 0; i < nb_rules; i++) {nb_equiv_class = nb_equiv_class + tokeep[i];}
  rule ** TOR_class = (rule**)malloc(nb_equiv_class*sizeof(rule*));
  int * TOS_class = (int*)malloc(nb_equiv_class*sizeof(int));
  nb_equiv_class = 0;
  for (i = 0; i < nb_rules; i++)
    {
      if (tokeep[i] == 0) {free(TOR_init[i]->name); free(TOR_init[i]);continue;}
      TOR_class[nb_equiv_class] = TOR_init[i];
      TOS_class[nb_equiv_class] = TOS_init[i];
      nb_equiv_class = nb_equiv_class+1;
    }

  
  free(TOR_init); free(TOS_init);   
  free(tokeep); free(TOSR); free(Tabul);
  *(TOR) = TOR_class; *(TOS) = TOS_class;
  return nb_equiv_class;


}