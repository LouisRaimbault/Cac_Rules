#include "Cac_SufRules.h"
int MSP;
int NBR;
int MAXSZ = 30;
int curlabelmin;
void CBF2 (uint64_t * Ultab, int *ind,int* mod,uint64_t * ptul, uint64_t * nlgtabl,  int good_nbc, int rst)
{   
    auto * yb = &Ultab[0]; uint64_t u; int t = 0; auto * ybi = &Ultab[0];
    auto * yd = &Ultab[64];
    int m = 0;
    if (rst)
    {
      for (auto a =0; a < good_nbc-1 ; a++)
      {  
        for (yb = ybi; yb != yd; yb++)
        { 
          u ^= (-((ptul[ind[t]]>>mod[t++]) & 1UL) ^ u) & *yb;
        }
        nlgtabl[a] = u;
        
      }
      
      u = 0;
      for ( yb = ybi; yb != &Ultab[rst]; yb++) { u ^= (-((ptul[ind[t]]>>mod[t++]) & 1UL) ^ u) & *yb;}
      nlgtabl[good_nbc-1] = u;
      

      return;      
    }

    for (auto a =0; a < good_nbc ; a++)
      {  
        for (yb = ybi; yb != yd; yb++)
        { 
          u ^= (-((ptul[ind[t]]>>mod[t++]) & 1UL) ^ u) & *yb;
        }
        nlgtabl[a] = u;
      }

}



void width_Rules (uint64_t ** BDT, uint64_t ** nom_noeuds, short * labelmin, int nb, uint64_t * y ,uint64_t * no_y, short size, int maxul, info_mining * im)
{
  int i,j,k,l,m,n;
  int sum;
  int sum_no;
  int ncm = im->DT_train->maxul_ind;
  uint64_t * ptul;
  uint64_t * ptulb;
  uint64_t * ptulc;
  uint64_t * ptname;
  uint64_t * ptname2;
  uint64_t * ptname3;
  int tr_nb;
 
  for (i = 0; i < nb-1; i++)
    {
      
      short * tr_labelmin = (short*)malloc(nb*sizeof(short));
      uint64_t ** tr_BDT = (uint64_t**)malloc(nb*sizeof(uint64_t*)); 
      uint64_t ** tr_nom_noeuds = (uint64_t**)malloc(nb*sizeof(short*));  
            
      ptul = BDT[i];
      ptname = nom_noeuds[i]; 
      tr_nb = 0;
      
      for (j = i+1; j < nb; j++ )
        { 
          ptname2 = nom_noeuds[j];
          sum = 0;
          ptulb = BDT[j];
          for (n = 0; n < maxul; n++)
            {
              sum += __builtin_popcountl(ptul[n] & ptulb[n] & y[n]); 
            }
          if ( sum < MSP) {continue;}
          sum_no = 0;
          for (n = 0; n < maxul; n++) 
            { 
              sum_no += __builtin_popcountl(ptul[n] & ptulb[n] & no_y[n]); 
            }

          if (sum_no == 0)
            {
              uint64_t * name = (uint64_t*)malloc(ncm*sizeof(uint64_t));
              for (n = 0; n < ncm; n++) {name[n] = ptname[n] | ptname2[n];}           
              NBR ++;
              rule * nouv_R = (rule*)malloc(sizeof(rule));
              nouv_R->name = name;
              nouv_R->brother = im->STKR->TSR[curlabelmin][size];
              im->STKR->TSR[curlabelmin][size] = nouv_R;
              
              continue;
            }
           
           tr_labelmin[tr_nb] = labelmin[j];
           tr_BDT[tr_nb] = (uint64_t*)malloc(maxul*sizeof(uint64_t));     
           tr_nom_noeuds[tr_nb] = (uint64_t*)malloc(ncm*sizeof(uint64_t));
           
           ptulc = tr_BDT[tr_nb];           
           ptname3 = tr_nom_noeuds[tr_nb];
          

           for (n = 0; n < maxul; n++) {ptulc[n] = ptul[n] & ptulb[n];}
           

           for (n = 0; n < ncm; n++) {ptname3[n] = ptname[n] | ptname2[n];}
           tr_nb++;
               
        }
   
      if (tr_nb == 0) 
        {
          free(tr_labelmin);
          free(tr_BDT); 
          free(tr_nom_noeuds);
          continue;
        }
      if (tr_nb ==1) 
        {
          free(tr_labelmin);
          free(tr_nom_noeuds[0]); 
          free(tr_BDT[0]); 
          free(tr_BDT); 
          free(tr_nom_noeuds);  
          continue;
        }
      
      uint64_t * check_max = (uint64_t*)malloc(maxul*sizeof(uint64_t));
      ptul = tr_BDT[0];
      for (n = 0; n < maxul; n++) {check_max[n] = ptul[n];}
      for (int w = 1; w < tr_nb;w++)
        {
          ptul = tr_BDT[w];
          for (n = 0; n < maxul; n++) {check_max[n] = check_max[n] & ptul[n]; }
        }  
      sum_no = 0;
      for (n = 0; n < maxul; n++) 
        {
          sum_no += __builtin_popcountl(check_max[n] & no_y[n]);
          
        }

      free(check_max);
      if (sum_no == 0 && size < MAXSZ) {width_Rules (tr_BDT, tr_nom_noeuds, tr_labelmin, tr_nb,y,no_y,size+1,maxul,im);}
      for (m = 0; m < tr_nb; m++) 
        {
          free(tr_nom_noeuds[m]); 
          free(tr_BDT[m]); 
         }
        free(tr_labelmin);
        free(tr_BDT); 
        free(tr_nom_noeuds);  
    }
  return;
}

void SufRules (info_mining * im)
{
  NBR = 0;
  uint64_t ** Bit_t = im->DT_train->X;
  uint64_t * Y = im->IT->Y_selec;
  uint64_t * no_Y = im->IT->no_Y_selec;
  MSP = im->IT->MSP;
  int maxul = im->DT_train->maxul;
  int nvar = im->IT->nvar_selec;
  int * order_var = im->IT->order_var;
  uint64_t Utab [64];
  for (int i = 0; i < 64; i++) {Utab[i] = (1UL << i);}
  int j,n,m,sum_root,nb_root;
  int sum,sum_no;
  uint64_t * ptul = NULL;
  uint64_t * ptulb = NULL;
  uint64_t * ptulc = NULL;
  uint64_t u;
  int nbc,num_var; float conf2;
  int ncm = im->DT_train->maxul_ind ; 
  short * is_perfect_item = (short*)calloc(im->DT_train->nvar,sizeof(short));

  int sumy = 0; int sumnoy = 0;
  for (n = 0; n < maxul; n++) {sumy += __builtin_popcountl(Y[n]); sumnoy += __builtin_popcountl(no_Y[n]);}

  for (int i = 0; i < nvar; i++)
    {
      nbc = 0;
      num_var = order_var[i];
      curlabelmin = i;
      ptul = Bit_t[order_var[i]];  
      int sum_chk = 0; for (n = 0; n < maxul; n++) {sum_chk += __builtin_popcountl(ptul[n]);}
      if (sum_chk == im->DT_train->nb_ind) { continue;}
      
      sum_root = 0;
      sum_no = 0; int sum_yes = 0;
      for (n = 0; n < maxul; n++) {sum_yes += __builtin_popcountl(ptul[n] & Y[n]);} 
      for (n = 0; n < maxul; n++) { sum_root += __builtin_popcountl(ptul[n]); sum_no += __builtin_popcountl(ptul[n] & no_Y[n]); }

      uint64_t * cur_name = (uint64_t*)calloc(ncm,sizeof(uint64_t));
      cur_name[order_var[i]/64] |= 1UL << order_var[i]%64;
      conf2 = (float)sum_yes/(float)sum_root;
      if (sum_yes < MSP) 
        {
          free(cur_name);
          continue;
         }
      
      if (sum_no == 0) 
        {               
            NBR ++;
            is_perfect_item[num_var]=1; 
            rule * nouv_R = (rule*)malloc(sizeof(rule));
            nouv_R->name = cur_name;
            nouv_R->brother = im->STKR->TSR[curlabelmin][1]; // 
            im->STKR->TSR[curlabelmin][1] = nouv_R;       
            continue;
        }
      
      short * root_labelmin = (short*)malloc((i+1)*sizeof(short));
      uint64_t ** tab_root = (uint64_t**)malloc((i+1)*sizeof(uint64_t*)); 
      uint64_t ** tab_root_name  = (uint64_t**)malloc((i+1)*sizeof(uint64_t*)); 

      nb_root = 0;      
      int * tab_case = (int*)malloc(sum_root*sizeof(int));
      int * tab_modulo = (int*)malloc(sum_root*sizeof(int));
      nbc = sum_root/64;
      if (sum_root%64 != 0) {nbc = nbc+1;}
      int rst = sum_root%64;
      sum_root = 0;
      for (n = 0; n < maxul; n++)
        {
          for (m = 0; m < 64; m++)
            {
              if ((ptul[n] >> m) & 1UL ) {tab_case[sum_root] = n; tab_modulo[sum_root] = m; sum_root = sum_root+1; }
            }
        }  
      
      uint64_t * Y_root = (uint64_t*)malloc(nbc*sizeof(uint64_t));
      CBF2 (Utab, tab_case, tab_modulo, Y ,Y_root,nbc,rst );
      for (j = i-1; j >= 0; j--)
        { 
          if (is_perfect_item[order_var[j]]) {continue;} 
          sum = 0;
          ptulb = Bit_t[order_var[j]];
          for (n = 0; n < maxul; n++ )
            {
              sum += __builtin_popcountl(ptul[n] & ptulb[n] & Y[n]);
            }         
          if (sum < MSP) {continue;}

          
          sum_no = 0;
          for (n = 0; n < maxul; n++) 
            {
              sum_no += __builtin_popcountl(ptul[n] & ptulb[n] & no_Y[n]);
            }

          if (sum_no == 0)
            { 
              NBR++;
              uint64_t * name = (uint64_t*)calloc(ncm,sizeof(uint64_t));
              name[order_var[j]/64] |= 1UL << order_var[j]%64;
              for (n = 0; n < ncm; n++) {name[n] = cur_name[n] | name[n];}
              rule * nouv_R = (rule*)malloc(sizeof(rule));
              nouv_R->name = name;
              nouv_R->brother = im->STKR->TSR[curlabelmin][2];
              im->STKR->TSR[curlabelmin][2] = nouv_R;
              continue;              
            }  
          uint64_t * nouv_ptul = (uint64_t*)calloc(nbc,sizeof(uint64_t));
          CBF2 (Utab, tab_case, tab_modulo, ptulb ,nouv_ptul,nbc,rst );
          sum = 0;
          for (n = 0; n < nbc; n++) {sum += __builtin_popcountl(Y_root[n] & nouv_ptul[n]);}
          if (sum < MSP) { free(nouv_ptul); continue;}
          root_labelmin[nb_root] = j;
          tab_root[nb_root] = nouv_ptul;
          tab_root_name[nb_root] = (uint64_t*)calloc(ncm,sizeof(uint64_t));
          tab_root_name[nb_root][order_var[j]/64] |= 1UL << order_var[j]%64;
          for (n = 0; n < ncm; n++) {tab_root_name[nb_root][n] = tab_root_name[nb_root][n] | cur_name[n];}
          nb_root++;        
        }
      
      if (nb_root == 0) 
        {
          free(root_labelmin);
          free(Y_root); 
          free(tab_root); 
          free(tab_root_name); 
          free(tab_case); free(tab_modulo);
          free(cur_name);
          continue;
        }
      if (nb_root == 1)
        { 
          free(root_labelmin);
          free(Y_root); 
          free(tab_root[0]); 
          free(tab_root_name[0]); 
          free(tab_root); 
          free(tab_root_name);
          free(tab_case); free(tab_modulo);
          free(cur_name);
          continue;
        }

      uint64_t * no_Y_root = (uint64_t*)calloc(nbc,sizeof(uint64_t));
      CBF2 (Utab, tab_case, tab_modulo, no_Y ,no_Y_root,nbc,rst );

      uint64_t * check_max = (uint64_t*)malloc(nbc*sizeof(uint64_t));
      ptul = tab_root[0];
      for (n = 0; n < nbc; n++) {check_max[n] = ptul[n];}
      for (int w = 1; w < nb_root;w++)
        {
          ptul = tab_root[w];
          for (n = 0; n < nbc; n++) {check_max[n] = check_max[n] & ptul[n]; }
        } 
      sum_no = 0;   
      for (n = 0; n < nbc; n++) 
        {
          sum_no += __builtin_popcountl(check_max[n] & no_Y_root[n]);
        }

      free(check_max); free(tab_case); free(tab_modulo);
      if (sum_no == 0) {width_Rules (tab_root,tab_root_name,root_labelmin,nb_root,Y_root,no_Y_root,3,nbc,im);}
      
            
      for ( j = 0; j < nb_root; j++ )
        {
          free(tab_root[j]); 
          free(tab_root_name[j]); 
        }          

      free(root_labelmin);  
      free(cur_name);
      free(tab_root); 
      free(tab_root_name); 
      free(Y_root); 
      free(no_Y_root); 
    }
  int sum_Yolo = 0; for (n = 0; n < maxul; n++) {sum_Yolo += __builtin_popcountl(Y[n]);}
  free(is_perfect_item);
}



void final_elag_simple (info_mining * im)
{
  int i,j,k,l,m,n;

  int maxul_train = im->DT_train->maxul;
  int nvar = im->DT_train->nvar;
  std::string * varnames = new std::string [nvar]; 
  for (i = 0; i < nvar; i++) {varnames[i] = "v" + std::to_string(i);}
  rule ** FR_final = (rule**)malloc((nvar+1)*sizeof(rule*)); // La nouvelle a remplir et a remplacer à la fin
  for (i = 0; i < nvar+1; i++) {FR_final[i] = NULL;}
  rule ** FR = im->STKR->FR;
  rule * R_FR; rule * R_dell;
  int nb_rule_size = 0;
  uint64_t Bone = 0; for (i = 0; i < 64; i++) {Bone |= 1UL << i;}
  uint64_t * name;
  int sum; int Max_Sup;
  int nrules_finales = 0;
  std::unordered_map<std::string,int> mappy;

  for (i = 0; i < nvar+1; i++)
    {
      R_FR = FR[i];
      while (R_FR)
        {
          std::string st = "";
          name  = R_FR->name;
          R_dell = R_FR;
          for (n = 0; n < nvar; n++)
            {
              if ((name[n/64] >> n%64) & 1UL)
                {
                  st += varnames[n];  
                } 
            }

          if (mappy[st]) { R_FR = R_FR->brother; free(R_dell->name); free(R_dell); continue;}
          mappy[st] = 1;
          nrules_finales ++;
          R_dell = R_FR->brother;
          R_FR->brother = FR_final[i];
          FR_final[i] = R_FR;
          R_FR = R_dell;
        }

    }
  std::cout << "NRULES =  " << nrules_finales << "\n";  
  free(im->STKR->FR);
  im->STKR->FR = FR_final;
  delete [] varnames; 
}



int check_if_supset_TR (rule **** MSR, int *** TSS, int ** TSN, int num_sousarbre, int taille_rule ,uint64_t * name, int sup, int ncm)
{
  int i,j,m,n,sum;
  uint64_t * name2;
  int * tab_sup;
  rule ** tab_rule;
  int max_size_poss;


  for (i = 0; i <= num_sousarbre; i++) 
    {
      if (i+2 < taille_rule) {max_size_poss = i+2;}
      else {max_size_poss = taille_rule;}
      for (j = 1; j < max_size_poss; j++)
        {
          m = TSN[i][j];
          if (m == 0) {continue;}
          tab_sup = TSS[i][j];
          tab_rule = MSR[i][j];
          m = m; 
          while (sup <= tab_sup[m])
            {
              sum = 0;
              name2 = tab_rule[m]->name;
              for (n = 0; n < ncm; n++) {sum += __builtin_popcountl(name2[n] & name[n]);}
              if (sum == j) {return 0;}
              m = m-1;

            }

        }
    }

  return 1;
}



void update_FR_selec_2 (info_mining * im)
{

  int nbr_update = 0;
  rule ** FR = im->STKR->FR; 
  rule *** TSR = im->STKR->TSR; 
  rule * R_TSR; // 
  rule * R_FR;
  uint64_t ** X_train = im->DT_train->X;
  int maxul_train = im->DT_train->maxul;
  int ncm = im->DT_train->maxul_ind;
  int nvar = im->IT->nvar_selec;
  int nb_mod = im->DT_train->nvar;

  int i,j,k,m,n,sum;
  uint64_t Bone = 0; for (i = 0; i < 64; i++) {Bone |= 1UL << i;} 
  int ** Tab_Subtree_nrules = (int**)malloc(nvar*sizeof(int*));
  int  *** Tab_Subtree_Sup = (int***)malloc(nvar*sizeof(int**));
  rule **** Tab_Subtree_rule = (rule ****)malloc(nvar*sizeof(rule***));
  int *** Tab_Subtree_valid = (int***)malloc(nvar*sizeof(int**));
  uint64_t * name;
  uint64_t * ptul;
  uint64_t * x_train = (uint64_t*)malloc(maxul_train*sizeof(uint64_t));

  for (i = 0; i < nvar; i++) // Nombre de sous arbres
    {
      Tab_Subtree_Sup[i] = (int**)malloc((i+2)*sizeof(int*));
      Tab_Subtree_rule[i] = (rule***)malloc((i+2)*sizeof(rule**));
      Tab_Subtree_nrules[i] = (int*)malloc((i+2)*sizeof(int));
      Tab_Subtree_valid[i] = (int**)malloc((i+2)*sizeof(int*));
     
      for (j = 0; j < i+2; j++) 
        { 
          k = 0; 
          R_TSR = TSR[i][j];
          if (R_TSR == NULL) {Tab_Subtree_nrules[i][j] = 0; continue;}
          while (R_TSR)
            {
              k = k+1;  
              R_TSR = R_TSR->brother; 
              nbr_update ++; 
            }
         Tab_Subtree_rule[i][j] = (rule**)malloc((k+1)*sizeof(rule*));
         Tab_Subtree_Sup[i][j] = (int*)malloc((k+1) * sizeof(int)); 
         Tab_Subtree_valid[i][j] = (int*)calloc((k+1),sizeof(int));
         Tab_Subtree_nrules[i][j] = k;

         R_TSR = TSR[i][j]; k = 0;
      
         while (R_TSR)
          {
            for (n = 0; n < maxul_train; n++) {x_train[n] = Bone;}
            name = R_TSR->name;
            for (m = 0; m < nb_mod; m++) 
              {
                if ((name[m/64] >> m%64) & 1UL) 
                  {
                    ptul = X_train[m];
                    for (n = 0; n < maxul_train; n++) {x_train[n] = x_train[n] & ptul[n];}
                  }
              }
            int babar = 0;
            sum = 0; for (n = 0; n < maxul_train;n++) {sum += __builtin_popcountl(x_train[n]); babar+= __builtin_popcountl(x_train[n] & im->IT->Y_selec[n]);}
            Tab_Subtree_Sup[i][j][k] = sum;
            Tab_Subtree_rule[i][j][k] = R_TSR;
            k++;
            R_TSR = R_TSR->brother;
          }
          Tab_Subtree_Sup[i][j][k] = 0; // Une fausse valeur 
          Tab_Subtree_rule[i][j][k] = NULL; // Un faux pointeur 
          
          special_rule_order(Tab_Subtree_rule[i][j], Tab_Subtree_Sup[i][j] ,k+1, maxul_train); 
        }
    }

  for (i = 0; i < nvar; i++)
    {
      for (j = 0; j < (i+2); j++)
      {
        if (Tab_Subtree_nrules[i][j] == 0) {continue;}
        for (k = 1; k <= Tab_Subtree_nrules[i][j]; k++) 
          {
            name = Tab_Subtree_rule[i][j][k]->name;
            sum = Tab_Subtree_Sup[i][j][k];
            int valid = check_if_supset_TR (Tab_Subtree_rule,Tab_Subtree_Sup,Tab_Subtree_nrules,i,j,name,sum,ncm);
            if (valid == 0) {Tab_Subtree_valid[i][j][k] = 1; }
          }
      }
    }

  for (i = 0; i < nvar; i++)
    {
      for (j = 0; j < (i+2); j++)
      {
        if (Tab_Subtree_nrules[i][j] == 0) {continue;}
        for (k = 1; k <= Tab_Subtree_nrules[i][j]; k++) 
          { 
            if (Tab_Subtree_valid[i][j][k] == 1)
              {
                free(Tab_Subtree_rule[i][j][k]->name);
                free(Tab_Subtree_rule[i][j][k]);
                continue;
              }
            Tab_Subtree_rule[i][j][k]->brother = FR[j];
            FR[j] = Tab_Subtree_rule[i][j][k];
          }
      }
    }

  for (i = 0; i < nvar; i++)
    {
      for (j = 0; j < i+2; j++)
      {
        if (Tab_Subtree_nrules[i][j] == 0) {continue;}
        free(Tab_Subtree_rule[i][j]);
        free(Tab_Subtree_Sup[i][j]);
        free(Tab_Subtree_valid[i][j]);
      }
      free(Tab_Subtree_rule[i]);
      free(Tab_Subtree_Sup[i]);
      free(Tab_Subtree_nrules[i]);
      free(Tab_Subtree_valid[i]);
    }
 free(Tab_Subtree_rule);
 free(Tab_Subtree_Sup);
 free(Tab_Subtree_nrules);
 free(Tab_Subtree_valid);
 free(x_train);

} 


void search_rule_1 (int nmb, uint64_t * Supreme, info_mining * im)
{
  int i,j,n;
  uint64_t * ptul;
  int maxul_train = im->DT_train->maxul; 
  int nb_mod = im->DT_train->nvar;
  int ncm = im->DT_train->maxul_ind;
  uint64_t ** X = im->DT_train->X; 
  uint64_t * rulone = (uint64_t*)malloc(maxul_train*sizeof(uint64_t));
  uint64_t Bone; for (n = 0; n < 64; n++) {Bone |= 1UL << n;}
  for (n = 0; n < maxul_train; n++) {rulone[n] = Bone;}
  uint64_t * no_Y = im->IT->no_Y_selec;
  uint64_t * name = (uint64_t*)calloc(ncm,sizeof(uint64_t));  
  int * Sum_X = (int*)calloc(nb_mod,sizeof(int));
  

  int gs = 10; int a; int min_a;
  for (i = 0; i < nb_mod; i++) {for (n = 0; n < maxul_train; n++) {Sum_X[i] += __builtin_popcountl(X[i][n]);}}

  while (gs > 0)
    {
      min_a = maxul_train*64 +100;
      for (i = 0; i < nb_mod;i++)
        {
          if (!((X[i][nmb/64] >> nmb%64) & 1UL)) {continue;}
          ptul = X[i];
          a = 0;
          for (n = 0; n < maxul_train; n++) {a += __builtin_popcountl(rulone[n] & ptul[n] & no_Y[n]);}
          if (a > min_a) {continue;}
          if (a == min_a) {if (Sum_X[i] <= Sum_X[j]) {continue;}}
          j = i;
          min_a = a;
        }
      for (n = 0; n < maxul_train; n++) {rulone[n] = rulone[n] & X[j][n];}
      name[j/64] |= 1UL << j%64;
      gs = 0;
      for (n = 0; n < maxul_train; n++) {gs += __builtin_popcountl(rulone[n] & no_Y[n]);}
    }


  for (n = 0; n < maxul_train; n++) {Supreme[n] = Supreme[n] | rulone[n];} 
  int size = 0; for (n = 0; n < ncm; n++) {size+= __builtin_popcountl(name[n]);} 
  rule * nouv_R = (rule*)malloc(sizeof(rule));
  nouv_R->name = name;
  nouv_R->brother = im->STKR->FR[size];
  im->STKR->FR[size] = nouv_R;
  free(rulone);
  free(Sum_X);
}


void clean_next_step_im (info_mining * im)
{

  stock_rules * STKR = im->STKR;
  for (int k = 0; k < im->DT_train->nvar; k++)
    {
      for (int i = 0; i < im->DT_train->nvar+1; i++) {STKR->TSR[k][i] = NULL;}
    }
    
} 


uint64_t count_number_final_rules (rule ** FR, int nb_mod)
{
  uint64_t nb_rules = 0;
  rule * R_FR;  

  for (int i = 0; i < nb_mod+1; i++)
    {
      R_FR = FR[i];
      while (R_FR) {nb_rules = nb_rules+1; R_FR = R_FR->brother;}
    }
  return nb_rules;  
}
