#include "Order_Rules.h"
#pragma optimization_level 3
#pragma GCC optimize("Ofast,no-stack-protector,unroll-loops,fast-math,O3")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx")
#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2,fma")
#pragma GCC optimization ("unroll-loops")



void Cfer_tri_groupe (int * sumvec, int * tab_help,  int nb )
{
  int i,j;
  
  int treme,itreme,temp;
  int pt_int;
  for (i = 0; i<nb-1;i++)
  {
    itreme = i; treme=sumvec[i];
    for (j=i+1;j<nb;j++) 
    {
      if (sumvec[j]<treme) {treme = sumvec[j];itreme=j;}
    }
    temp=sumvec[itreme];sumvec[itreme]=sumvec[i];sumvec[i]=temp;
    pt_int = tab_help[itreme]; tab_help[itreme] = tab_help[i]; tab_help[i]=pt_int;
  }

}

void order_scale_1 (int * tab_help, int * tab_val, int nb, int min, int max)
{
  
  int i;
  int j = nb-1;
  int * cpy_help = (int*)malloc(nb*sizeof(int));
  for (i = 0; i < nb; i++) {cpy_help[i] = tab_help[i];}
  int k = 0;
  for (i = 0; i < nb;i++)
    {
      if (tab_val[i] == min) 
        {
          tab_help[k] = cpy_help[i]; k++; continue;
        }
      tab_help[j] = cpy_help[i];
      j--;
    }

 free(cpy_help);

}


void Cfer_r_order (int * tr_sumvec, int * tab_help, int * tr_help, int iinf,int isup,int nb )
{
  
  int i,j,k;

  
  if (nb <= 200)
  {
    Cfer_tri_groupe (tr_sumvec,tr_help,nb);
    
    k = 0;
    for (int i = iinf; i < isup; i++,k++)
    {
      tab_help[i] = tr_help[k];
    }    
    return;
  }

  int max = tr_sumvec[0];
  int min = tr_sumvec[0];
  for (i = 1; i < nb;i++)
    {
      if (tr_sumvec[i] < min) {min = tr_sumvec[i]; continue;}
      if (tr_sumvec[i] > max) {max = tr_sumvec[i];}
    }
  if (max == min)
    {
      k = 0;
      for (int i = iinf; i < isup; i++,k++)
      {
        tab_help[i] = tr_help[k];
      }
      return;       
    }

  if ((max-min) < 2) // en valeur absolue ?
    {
      order_scale_1 (tr_help,tr_sumvec,nb,min,max);
      k = 0;
      for (int i = iinf; i < isup; i++)
      {
        tab_help[i] = tr_help[k++];
      }      
      return;
    }
  
  int avg = (max + min)/2;
  int nb_inf_avg = 0;
  int nb_sup_avg = 0;
  
  for (i = 0; i < nb;i++)
  {
    if (tr_sumvec[i] <= avg) {nb_inf_avg++; continue;}
    nb_sup_avg++;
  }
  
  int * tr_sumvec_inf = (int*)malloc(nb_inf_avg*sizeof(int));
  int * tr_sumvec_sup = (int*)malloc(nb_sup_avg*sizeof(int));

  int * tr_help_inf = (int*)malloc(nb_inf_avg*sizeof(int));
  int * tr_help_sup = (int*)malloc(nb_sup_avg*sizeof(int));
  

  j = 0; k = 0;
  
  for (i = 0; i < nb;i++)
  {
    if (tr_sumvec[i] <= avg) 
    {
      tr_help_inf[j] = tr_help[i];
      tr_sumvec_inf[j++] = tr_sumvec[i]; 
      continue;
    }
     tr_help_sup[k] = tr_help[i]; 
     tr_sumvec_sup[k++] = tr_sumvec[i];
  }
 
  Cfer_r_order (tr_sumvec_inf, tab_help, tr_help_inf ,iinf, iinf+nb_inf_avg,nb_inf_avg);
  Cfer_r_order (tr_sumvec_sup, tab_help, tr_help_sup , iinf+nb_inf_avg, isup,nb_sup_avg);    
  
  free(tr_sumvec_inf);
  free(tr_sumvec_sup);
  free(tr_help_inf);
  free(tr_help_sup);

  
}



void Cfer_order(int * tab_help, int * sumvec ,int nb_elem, int maxul)
{
  

  int i,j,k,l;
  int sum = 0;
  int min = maxul * 64 + 1000;
  int max = 0;
  
  int nb_bornes = 100;  
  

  for (i = 0; i < nb_elem; i++)
    {
      if (sumvec[i] > max ) {max = sumvec[i];}
      if (sumvec[i] < min ) {min = sumvec[i];}
    }
  
  int ecart_minmax = max - min;
  int pas_borne = ecart_minmax/nb_bornes;
  
  
  if (pas_borne == 0)
    {
      nb_bornes = 10;
      pas_borne = ecart_minmax/nb_bornes;
    }

  if (pas_borne == 0)
    {
      nb_bornes = 2;
      pas_borne = ecart_minmax/nb_bornes;
    }  

  if (pas_borne == 0)
    {
      order_scale_1(tab_help,sumvec,nb_elem,min,max);
      return;  
    }

  int total_bornes = nb_bornes+1;
  
  int * val_bornes = (int*)malloc(total_bornes*sizeof(int));
  int * nb_val_bornes = (int*)calloc(total_bornes,sizeof(int));
  int ** sum_val_bornes = (int**)malloc(total_bornes * sizeof(int*));
  int ** tabs_help_bornes = (int**) malloc(total_bornes*sizeof(int*));
  int * transi_nb_val_bornes = (int*)calloc(total_bornes,sizeof(int));
  int * indice_bornes = (int*)calloc(total_bornes,sizeof(int));
  int * pt_list_class;
  
  val_bornes[0] = min;
  val_bornes[nb_bornes] = max;

  
  for (i = 1; i < nb_bornes; i++ )
    {
      val_bornes[i] = val_bornes[i-1]+pas_borne;
      
    }
  for (i = 0; i < nb_elem; i++)
    { 
      sum = sumvec[i];
      for (j = 0; j < nb_bornes; j++)
        {
          if (sum <= val_bornes[j+1])
            {
              nb_val_bornes[j] = nb_val_bornes[j]+1;
              break;
            }
        }
          
    }
  

 indice_bornes[0] = 0; // remplissage des indices
 for (j = 1; j < nb_bornes+1; j++)
  {
    indice_bornes[j] = indice_bornes[j-1] + nb_val_bornes[j-1];
  }


  for (i = 0; i < nb_bornes+1; i++) 
    {
      if (nb_val_bornes[i] == 0) {continue;}
      tabs_help_bornes[i] = (int*)malloc(nb_val_bornes[i]*sizeof(int));
      sum_val_bornes [i] = (int*)malloc(nb_val_bornes[i]*sizeof(int));
    }
  
  for (i = 0; i < nb_elem; i++)
    { 
      sum = sumvec[i];
      for (j = 0; j < nb_bornes; j++)
        {
          if (sum <= val_bornes[j+1])
            { 
              tabs_help_bornes[j][transi_nb_val_bornes[j]] = tab_help[i];
              sum_val_bornes[j][transi_nb_val_bornes[j]] = sum;
              transi_nb_val_bornes[j] = transi_nb_val_bornes[j] + 1;
              break;
            }
        }

    }
  
  int tr_sum = 0;
  
  for (i = 0; i < nb_bornes; i++)
    { 
      if (nb_val_bornes[i] > 0) 
        {          
          Cfer_r_order (sum_val_bornes[i],tab_help,tabs_help_bornes[i],indice_bornes[i],indice_bornes[i+1],nb_val_bornes[i]);          
        }
    } 

  free(val_bornes);
  free(transi_nb_val_bornes);
  free(indice_bornes);
  
  for (i = 0; i < nb_bornes+1; i++)
    {
      if (nb_val_bornes[i] == 0) {continue;}
      free (tabs_help_bornes[i]);
      free (sum_val_bornes[i]);
    }
  
  free(tabs_help_bornes);
  free(nb_val_bornes);
  free (sum_val_bornes);
}



void special_rule_order(rule ** tab_to_order, int * sumvec ,int nb_elem, int maxul)
{
  int * help_table = (int*)malloc(nb_elem*sizeof(int));
  rule ** copy_rule = (rule**)malloc(nb_elem*sizeof(rule*));
  int * copy_sum = (int*)malloc(nb_elem*sizeof(int));

  for (int i = 0; i < nb_elem; i++) {help_table[i] = i; copy_rule[i] = tab_to_order[i]; copy_sum[i] = sumvec[i];}
  Cfer_order (help_table, sumvec, nb_elem,maxul);

  
  for (int i = 0; i < nb_elem; i++) 
    {
      tab_to_order[i] = copy_rule[help_table[i]];
      sumvec[i] = copy_sum[help_table[i]];
    }

  free(copy_rule);
  free(copy_sum);  
  free(help_table);
} 

void special_rule_order2(rule ** tab_to_order, uint64_t ** TOV , int * sumvec ,int nb_elem, int maxul)
{
  int * help_table = (int*)malloc(nb_elem*sizeof(int));
  rule ** copy_rule = (rule**)malloc(nb_elem*sizeof(rule*));
  int * copy_sum = (int*)malloc(nb_elem*sizeof(int));
  uint64_t ** copy_tov = (uint64_t**)malloc(nb_elem*sizeof(uint64_t*));

  for (int i = 0; i < nb_elem; i++) 
    {
      help_table[i] = i; copy_rule[i] = tab_to_order[i]; copy_sum[i] = sumvec[i];
      copy_tov[i] = TOV[i];
    }

  Cfer_order (help_table, sumvec, nb_elem,maxul);
  
  
  for (int i = 0; i < nb_elem; i++) 
    {
      tab_to_order[i] = copy_rule[help_table[i]];
      sumvec[i] = copy_sum[help_table[i]];
      TOV[i] = copy_tov[help_table[i]];
    }

  free(copy_rule);
  free(copy_sum);  
  free(help_table);
  free(copy_tov);
} 


void special_TOV_order(uint64_t ** TOV , int * sumvec ,int nb_elem, int maxul)
{
  int * help_table = (int*)malloc(nb_elem*sizeof(int));
  int * copy_sum = (int*)malloc(nb_elem*sizeof(int));
  uint64_t ** copy_tov = (uint64_t**)malloc(nb_elem*sizeof(uint64_t*));

  for (int i = 0; i < nb_elem; i++) 
    {
      help_table[i] = i; copy_sum[i] = sumvec[i];
      copy_tov[i] = TOV[i];
    }

  Cfer_order (help_table, sumvec, nb_elem,maxul);
  
  
  for (int i = 0; i < nb_elem; i++) 
    {
      sumvec[i] = copy_sum[help_table[i]];
      TOV[i] = copy_tov[help_table[i]];
    }

  free(copy_sum);  
  free(help_table);
  free(copy_tov);
} 


void special_float_order (float * tab_to_order, int nb_elem)
{
  int * help_table = (int*)malloc(nb_elem*sizeof(int));
  int * copy_float = (int*)malloc(nb_elem*sizeof(int));
  float * copy_tab_to_order = (float*)malloc(nb_elem*sizeof(float));
  int maxul = 1000000/64;
  for (int i = 0; i < nb_elem; i++) 
    {
      float b = tab_to_order[i]*(float)1000000;
      help_table[i] = i;
      copy_float[i] = (int)b;
      copy_tab_to_order[i] = tab_to_order[i];
    }
  Cfer_order (help_table, copy_float, nb_elem,maxul);
  int a = 0;
  for (int i = nb_elem-1; i >= 0 ; i--) 
    {
      tab_to_order[a] = copy_tab_to_order[help_table[i]];
      a++;
    }

  free(copy_float);
  free(help_table);

}

void special_ecv_order (ecv ** tab_to_order, int * sumvec, int nb_elem, int maxul)
{
  int * help_table = (int*)malloc(nb_elem*sizeof(int));
  ecv ** copy_rule = (ecv**)malloc(nb_elem*sizeof(ecv*));
  int * copy_sum = (int*)malloc(nb_elem*sizeof(int));

  for (int i = 0; i < nb_elem; i++) {help_table[i] = i; copy_rule[i] = tab_to_order[i]; copy_sum[i] = sumvec[i];}
  Cfer_order (help_table, sumvec, nb_elem,maxul);
  for (int i = 0; i < nb_elem; i++) 
    {
      tab_to_order[i] = copy_rule[help_table[i]];
      sumvec[i] = copy_sum[help_table[i]];
    }

  free(copy_rule);
  free(copy_sum);  
  free(help_table);

}