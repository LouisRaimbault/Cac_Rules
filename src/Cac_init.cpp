#include "Cac_init.h"


const size_t SUL = sizeof(uint64_t);
const size_t SI = sizeof(int);



void Init_data (char * pathfile,  info_prog * ip, dta * dt ) 
{ 
  // init version , slower but safe for larger data
  auto start = std::chrono::system_clock::now(); // Start time init
  std::chrono::duration<double> diffe;
  std::cout << "import data from file as char array and count number of line ... \n";
  char * tabchar;
  long longueur = 0;long i = 0;
  char c;
  int w =0; int t = 0; int z = 0;
  int nvar = 0; int nrows = 0; int maxul = 0;
  char deli = ip->delim;
  std::string st ="";
  FILE *Fichier = fopen(pathfile,"r");  // open path to data
 
  if (Fichier != NULL)
    {                         // count number of lines and char
      while (!feof(Fichier))  
       {c = getc(Fichier);
        longueur ++;
        if (c == '\n') nrows++; }                      

    }
  fclose(Fichier); //close file
  tabchar = (char*)malloc(longueur * sizeof(char)); // create array of char
  Fichier = fopen(pathfile,"r"); // open again file
  if ( Fichier != NULL && tabchar  != NULL)
    {                           // Fill array
     while (i < longueur)  
      { c = getc(Fichier);
        tabchar[i++]=c;}
      }
  else perror ("\n\n problem in file ");
  fclose(Fichier); // close file
  
  diffe= std::chrono::system_clock::now() - start; // stop timer
  std::cout << "done in " << diffe.count() << " secondes \t";

  std::cout << "Assignment of a number to the different variables and create transation as integer array .. \n";
  start = std::chrono::system_clock::now();
  int ** TRS = (int**)malloc(nrows *sizeof(int*)); // transac items 
  int * n_TRS = (int*)malloc(nrows *sizeof(int)); // nb item per transac
  int sup_init = nrows * ip->rel_minsup;
  sup_init = 1; 
  std::string a_st ="";
  int c_i = 1;
  int z_i;
  int c_nr = 0;
  int * P_TRS = NULL;
  i = 0;
  std::unordered_map<std::string,int> mappy; // give numer to id items

  // count sup for each item
  for (t = 0; t < nrows;t++)
    { 
      c_nr = 0;
      z = i;
      while (tabchar[i] != '\n')
        { 
          
          if (tabchar[i]==deli) 
            {
              c_nr++;
            }
         i++;
          
        }
      c_nr++;  
      TRS[t] = (int*) malloc(c_nr*sizeof(int));
      n_TRS[t] = c_nr;
      c_nr = 0;
      P_TRS = TRS[t];
      for (z; z < i; z++)
        {
          if (tabchar[z] == deli)
            {
              if (!mappy[a_st])
                {
                  mappy[a_st] = c_i;
                  P_TRS[c_nr] =c_i;
                  c_i++;
                  a_st = "";
                  c_nr++;          
                  continue;
                }

              P_TRS[c_nr] = mappy[a_st];
              a_st = "";
              c_nr++;
              continue;
            }
          a_st += tabchar[z];
        }
      
      if (!mappy[a_st])
        { 
          
          mappy[a_st] = c_i;
          P_TRS[c_nr] =c_i; 
          c_i++;
          a_st = "";
          i++;          
          continue;
        }

      P_TRS[c_nr] = mappy[a_st];
      a_st = "";
      i++;
    }

  free (tabchar);
  diffe= std::chrono::system_clock::now() - start;
  std::cout << "done in " << diffe.count() << " secondes \t";
  
  maxul = nrows/64;
  int rst = nrows-64*maxul;
  if (rst) maxul++;
  std::unordered_map<std::string,int>::iterator itmap;
  std::unordered_map<std::string,int>::iterator itendmap = mappy.end(); 
  int nvar_tot = mappy.size()+1;
  std::cout << "number of raws : " << nrows << " and total number of variables : " << nvar_tot-1 << "\n";

  
  std::cout << "Checking for frequent 1-item ... \n";
  start = std::chrono::system_clock::now();
  int * item_sup = (int*)calloc(nvar_tot,SI); // item support
  int * is_good_item = (int*) malloc((nvar_tot)*sizeof(int)); // if item sup > minsup
  uint64_t ** Bdata_tot = (uint64_t**)malloc ((nvar_tot+10) * sizeof(uint64_t*));


  for (t = 0; t < nrows; t++)
    {
      w = n_TRS[t];
      P_TRS = TRS[t];
      for (z = 0; z < w; z++)
        {
          item_sup[P_TRS[z]]++;
        }
    }

  diffe= std::chrono::system_clock::now() - start;
  std::cout << "Done in " << diffe.count() << " secondes, check for corresponding variables ...\n";
  
  for (t = 0; t < nvar_tot;t++)  {is_good_item[t] = 0;}
  t = 0;
  // Create first Bit data
  for (itmap = mappy.begin(); itmap != itendmap;itmap++) 
    { 

      if (item_sup[itmap->second] >= sup_init)
        {  
          Bdata_tot[itmap->second] = (uint64_t*)calloc(maxul,SUL);
          is_good_item[itmap->second] = 1;
          nvar++;
          continue;
        }    
    } 


  if (nvar == 0)
    {
      free (Bdata_tot);
      for (t = 0; t < nrows;t++) {free(TRS[t]);}
      free(TRS);
      free(n_TRS);
      free(is_good_item);
      free(item_sup);
      return;
    }
  t = 0;  

  std::unordered_map<int,std::string> map_names;

  for (itmap = mappy.begin(); itmap != itendmap;itmap++) 
    { 

      if (is_good_item[itmap->second])
        { 
          map_names[itmap->second] = itmap->first; 
        }    
    } 

  // Fill Bit data for frequent  item
  std::cout << "Create Bit Data store in uint64t aray with " << nvar << " frequent 1 -item ... \n";
  start = std::chrono::system_clock::now();
  z_i = 0; t = 0;
  int k = 0;
  uint64_t u =0;
  while(z_i < maxul-1)
  { 
    for (t =0; t < 64; t++,k++)
      { P_TRS = TRS[k];
        u = 1UL << t;
        for (z = 0; z < n_TRS[k]; z++)
          {
            if (is_good_item[P_TRS[z]])
              {
                Bdata_tot[P_TRS[z]][z_i]+= u ;
              }
          }
      }
    z_i++;
  }

  for (t = 0; t < rst; t++,k++)
      { 
        P_TRS = TRS[k];
        u = 1UL << t;
        for (z  = 0; z < n_TRS[k];z++)
          {
            if (is_good_item[P_TRS[z]])
              {
                Bdata_tot[P_TRS[z]][z_i] +=u;
              }
          }
      }     

  z= 0;    
  uint64_t ** Bitdata = (uint64_t**)malloc(nvar*sizeof(uint64_t*));
  std::string * varnames = new std::string [nvar];
  int * sum_1freq = (int*)malloc(nvar*sizeof(int));
  for (t = 0; t < nvar_tot;t++)
    {
      if (is_good_item[t]) {sum_1freq[z] = item_sup[t]; varnames[z] = map_names[t]; Bitdata[z++] = Bdata_tot[t];}
    }
  // Reduce first Bitdata to only pointers of frequent 1 item
  free (Bdata_tot);
  for (t = 0; t < nrows;t++) {free(TRS[t]);}
  free(TRS);
  free(n_TRS);
  free(is_good_item);
  free(item_sup);
  free(sum_1freq);
  diffe= std::chrono::system_clock::now() - start;
  std::cout << "Done in " << diffe.count() << " secondes\n";
  ip->minsup = sup_init;
  dt->X = Bitdata;
  dt->Names = NULL;
  dt->order_var = NULL;
  dt->nb_ind = nrows;
  dt->nvar = nvar;
  dt->maxul = maxul;
  dt->varnames = varnames;

 return ;
}

void destruct_dta (dta * D)
{
  delete [] D->varnames;
  for (int i = 0; i < D->nvar; i++) {free(D->X[i]);}
  free(D->X);
  if (D->order_var != NULL) {free(D->order_var);}
  if (D->Names != NULL) 
    {
      for (int i = 0; i < D->nvar;i++) {free(D->Names[i]);}
      free(D->Names);
    }
  free(D);
}

void set_negativ (uint64_t * T, uint64_t * no_T, int deb, int fin)
{
  for (int i = deb; i < fin; i++)
    {
      if ((T[i/64]>i%64)&1UL) {continue;}
      no_T[i/64] |= 1UL << i%64;
    }

}

void Init_info_mining (dta * train, info_mining * im , info_prog * ip)
{
  int i,k,n,m;
  uint64_t * ptul; uint64_t * ptulb;

  std::string * tab_complet_name = ip->tab_tgt_complet_name;
  std::string * tab_short_name = ip->tab_tgt_short_name;
  
  int maxul = train->maxul;
  im->tgt_train = (target*)malloc(sizeof(target));
  im->tgt_train->Y = (uint64_t*)calloc(maxul,sizeof(uint64_t));
  uint64_t Bone = 0; for (n = 0; n  < 64; n++) {Bone |= 1UL << n;}
  for (n = 0; n < maxul; n++) {im->tgt_train->Y[n] = Bone;}
  im->DT_train = (dta*)malloc(sizeof(dta));

  std::unordered_map<std::string,int> mappy;
  std::unordered_map<std::string,int> mappy_target;
  int nvar_merge = 1; int is_a_target;
  
  for (i = 0; i < train->nvar; i++)
    {
      k = 0;
      std::string st = "";
      std::string st_v = train->varnames[i];
      while (st_v[k] != '.') {st += st_v[k]; k = k+1;}
      is_a_target = 0;
      for (int q = 0; q < ip->nb_var_target; q++) {if (st == tab_short_name[q]) {is_a_target=1;}}
      if (is_a_target == 1) 
        {
          mappy_target[st_v] = 10;
          is_a_target = 0;
          for (int q = 0; q < ip->nb_var_target; q++) {if (st_v == tab_complet_name[q]) {is_a_target=1;}}

          if (is_a_target == 1) 
            {
              ptul = train->X[i];
              for (n = 0; n < maxul; n++) {im->tgt_train->Y[n] = im->tgt_train->Y[n] & ptul[n];}
              mappy_target[st_v] = 20; 
            }

          continue;
        }

      mappy[train->varnames[i]] = nvar_merge; nvar_merge = nvar_merge+1;
    }

  if (ip->take_neg == 1) 
    {
      for (i = 0; i < train->nb_ind; i++)
        {
          im->tgt_train->Y[i/64]  = im->tgt_train->Y[i/64]^(1UL << i%64); 
        }
    }  

  //// Fin du settage 
  nvar_merge = nvar_merge-1; 
  uint64_t ** X_train = (uint64_t**)malloc(nvar_merge*sizeof(uint64_t*));
  std::string * varnames = new std::string [nvar_merge];
  k = 0; 
  for (std::unordered_map<std::string,int>::iterator it = mappy.begin(); it != mappy.end(); it++) { varnames[it->second-1] = it->first;}
  // Attention , la map commence à 1 , donc faire -1 pour remplir le tableau
  for (i = 0; i < nvar_merge; i++)
    {
      X_train[i] = (uint64_t*)calloc(maxul,sizeof(uint64_t));
    }  
  for (i = 0; i < train->nvar; i++)
    {
      if (mappy_target.count(train->varnames[i])) {continue;}
      ptul = train->X[i]; ptulb = X_train[mappy[train->varnames[i]]-1];
      m = 0;
      for (n = 0; n < maxul; n++) {ptulb[n] = ptul[m]; m = m+1;}
    }

  im->DT_train->X = X_train;
  im->DT_train->varnames = varnames;
  im->DT_train->nvar = nvar_merge;
  im->DT_train->maxul = maxul;
  im->DT_train->nb_ind = train->nb_ind;
  im->DT_train->order_var = (int*)malloc(im->DT_train->nvar*sizeof(int));

  int maxul_ind = nvar_merge/64; if (nvar_merge % 64 > 0 ) {maxul_ind = maxul_ind +1;}
  im->DT_train->Names = (uint64_t**)malloc(nvar_merge*sizeof(uint64_t*));

  for (n = 0; n < nvar_merge; n++)
   {
      im->DT_train->Names[n] = (uint64_t*)calloc(maxul_ind,sizeof(uint64_t));
      im->DT_train->Names[n][n/64] |= 1UL << n%64;
    }

  im->DT_train->maxul_ind = maxul_ind;
  destruct_dta (train);

}




void create_transpose_mat (uint64_t ** Bitdata, uint64_t ** Ind_Data, int nb_M,int maxul)
{
  int i,j,k,l;
  uint64_t u = 0;
  uint64_t * ptul = NULL;
  int num_entiere,num_mod;

  for (i = 0; i < nb_M; i++)
    {
      ptul = Bitdata[i]; 
      l = 0;
      num_entiere = i/64;
      num_mod = i%64;
      
      for (j = 0; j < maxul; j++)
        { 
          u = ptul[j];
          for (k = 0; k < 64; k++)
            { 
              if ((u >> k) & 1UL)
                {            
                  Ind_Data[l][num_entiere] |= 1UL << num_mod;
                }
              l++;
            }
        }        
    }
  
  return;  
}


void set_nb_yorno (uint64_t ** Ind_Data, uint64_t * y_base, int max_ind, int maxul_ind, int * is_set, int i, int * num_commun , Ynumbers * Ynb )
{
  int n;
  uint64_t * ptul = Ind_Data[i];
  uint64_t * ptulb;
  is_set[i] = 1; 
  int som = 0; int somb;  int nb_Y = 0; int nb_no_Y = 0;
  num_commun[0] = i; int nb=1;
  if ((y_base[i/64] >> i%64) & 1UL) {nb_Y = nb_Y+1;} else {nb_no_Y = nb_no_Y+1;}
  for (n = 0; n < maxul_ind; n++) {som += __builtin_popcountl(ptul[n]);}

  for (int j = i+1; j < max_ind;j++)
    {
      if (is_set[j] == 1) {continue;}
      somb = 0;
      ptulb = Ind_Data[j];
      for (n = 0; n < maxul_ind; n++) {somb += __builtin_popcountl(ptul[n] & ptulb[n]);}
      if (som == somb) 
        {
          is_set[j]=1;
          num_commun[nb] = j; nb = nb+1;
          if ((y_base[j/64] >> j%64) & 1UL) {nb_Y = nb_Y+1;} else {nb_no_Y = nb_no_Y+1;}
        }
    }

  Ynb->nb_Y = nb_Y; 
  Ynb->nb_no_Y = nb_no_Y;  

}


void set_target_reg (info_mining * im)
{
  im->IE = (info_eta*)malloc(sizeof(info_eta));
  im->IE->treshold_eta = im->IP->treshold_eta;

  int max_ind = im->DT_train->maxul*64;
  uint64_t * ptul; uint64_t * ptulb;
  int i,j,n;
  int maxul_ind = im->DT_train->nvar/64;
  int maxul = im->DT_train->maxul;
  if (im->DT_train->nvar%64) {maxul_ind = maxul_ind +1;}
  uint64_t ** Ind_Data = (uint64_t**)malloc(max_ind*sizeof(uint64_t*));
  for (i = 0; i < max_ind; i++) {Ind_Data[i] = (uint64_t*)calloc(maxul_ind,sizeof(uint64_t));}
  create_transpose_mat (im->DT_train->X,Ind_Data,im->DT_train->nvar,im->DT_train->maxul);
  uint64_t * y_base = im->tgt_train->Y;
  uint64_t * neo_Y ;
  int * is_set = (int*)calloc(max_ind,sizeof(int));
  int * num_commun = (int*)malloc(max_ind*sizeof(int));

  std::unordered_map<int,int> mappy;
  int nb_eta_values = 1;
  Ynumbers * Ynb = (Ynumbers*)malloc(sizeof(Ynumbers));

  for (i = 0; i < im->DT_train->nb_ind; i++)
      {
        if (is_set[i] == 1) {continue;}
        
        set_nb_yorno (Ind_Data,y_base,im->DT_train->nb_ind,maxul_ind,is_set,i,num_commun ,Ynb);
        
        int nb_tot = Ynb->nb_Y + Ynb->nb_no_Y;
        int eta_val = (int) ((float)Ynb->nb_Y/(float)nb_tot*(float)100000);
        if(mappy.count(eta_val) == 0) {mappy[eta_val] = nb_eta_values; nb_eta_values = nb_eta_values+1;}
      }
  im->IE->nb_eta_values = nb_eta_values-1;
  std::cout << "nb eta values = " << im->IE->nb_eta_values << "\n";
  im->IE->eta_values = (int*)malloc(im->IE->nb_eta_values*sizeof(int));
  im->IE->tab_Y_eta = (uint64_t**)malloc(im->IE->nb_eta_values*sizeof(uint64_t*));
  im->IE->sum_Y_eta = (int*)malloc(im->IE->nb_eta_values*sizeof(int));

  std::unordered_map<std::string,int>::iterator itmap; i = 0;
  for (std::unordered_map<int,int>::iterator it = mappy.begin(); it != mappy.end();it++)
    {
      im->IE->eta_values[(it->second)-1] = it->first; i++;
    }

  for (i = 0; i < max_ind; i++) {is_set[i] = 0;}
  for (int k = 0; k < im->IE->nb_eta_values; k++) {im->IE->tab_Y_eta[k] = (uint64_t*)calloc(maxul,sizeof(uint64_t));}
  for (i = 0; i < im->DT_train->nb_ind; i++)
    {
      if (is_set[i] == 1) {continue;}
      set_nb_yorno (Ind_Data,y_base,im->DT_train->nb_ind,maxul_ind,is_set,i,num_commun ,Ynb);
      int nb_tot = Ynb->nb_Y + Ynb->nb_no_Y;
      int eta_val = (int) ((float)Ynb->nb_Y/(float)nb_tot*(float)100000);
      neo_Y = im->IE->tab_Y_eta[mappy[eta_val]-1];

      for (j = 0; j < nb_tot; j++) {neo_Y[num_commun[j]/64] |= 1UL << num_commun[j]%64;}
    }

 for (i = 0; i < im->IE->nb_eta_values; i++)
  {
    int sum = 0;
    for (j = 0; j < im->DT_train->maxul; j++) { sum += __builtin_popcountl(im->IE->tab_Y_eta[i][j]);}
    im->IE->sum_Y_eta[i] = sum;
  } 

 for (i = 0; i < max_ind; i++) {free(Ind_Data[i]);} free(Ind_Data);
 free(is_set); free(num_commun); free(Ynb);
}

void set_target_pred (info_mining * im)
{
  int max_ind = im->DT_train->maxul*64;
  uint64_t * ptul; uint64_t * ptulb;
  int i,j,n;
  int maxul_ind = im->DT_train->nvar/64;
  int maxul = im->DT_train->maxul;
  if (im->DT_train->nvar%64) {maxul_ind = maxul_ind +1;}
  uint64_t ** Ind_Data = (uint64_t**)malloc(max_ind*sizeof(uint64_t*));
  for (i = 0; i < max_ind; i++) {Ind_Data[i] = (uint64_t*)calloc(maxul_ind,sizeof(uint64_t));}
  create_transpose_mat (im->DT_train->X,Ind_Data,im->DT_train->nvar,im->DT_train->maxul);
  uint64_t * y_base = im->tgt_train->Y;
  uint64_t * neo_Y = (uint64_t*)calloc(maxul,sizeof(uint64_t));
  int * is_set = (int*)calloc(max_ind,sizeof(int));
  int * num_commun = (int*)malloc(max_ind*sizeof(int));

  int som,somb; int nb;
  Ynumbers * Ynb = (Ynumbers*)malloc(sizeof(Ynumbers));
  for (n = 0; n < maxul; n++) {neo_Y[n] = 0;}
  for (i = 0; i < im->DT_train->nb_ind; i++)
    {
      if (is_set[i] == 1) {continue;}
      set_nb_yorno (Ind_Data,y_base,im->DT_train->nb_ind,maxul_ind,is_set,i,num_commun ,Ynb);
      int nb_tot = Ynb->nb_Y + Ynb->nb_no_Y;
      int eta_val = (int) ((float)Ynb->nb_Y/(float)nb_tot*(float)100000);

      if (eta_val > im->IE->treshold_eta) 
        {
          for (j = 0; j < nb_tot; j++) {neo_Y[num_commun[j]/64] |= 1UL << num_commun[j]%64;}
        }
    }
 im->tgt_train->Y_base = im->tgt_train->Y;
 im->tgt_train->Y = neo_Y;
 for (i = 0; i < max_ind; i++) {free(Ind_Data[i]);} free(Ind_Data);
 free(is_set); free(num_commun); free(Ynb);

}


void export_result (info_mining * im)
{
  std::ofstream outfile(im->IP->pathexport,std::ios::app);
  if (!(outfile.is_open())) {std::cout << "ERROR FILE \n"; return;}
  int k = 0;
  if (outfile.is_open())
    {
      outfile << "Results for " << im->pathname << " with reduction " << *(im->type_red) << "\n";
      outfile << "The target is " << *(im->IP->tgt_name);
      outfile << " Eta value = " << (float)im->IE->cur_eta/(float)100000 << "   ";
      outfile << " (" << im->IE->cur_sum_Y << ")\n";
      outfile << "Number of Rules/Variables/Modalities : " << im->IR->nb_rule_red << "-";
      outfile << im->IR->nb_qualvar << "-";
      outfile << im->IR->nb_mod << "\n\n";
      outfile << "List of variables (" << im->IR->nb_qualvar << ")\n";
      k = 0;
      for (int i = 0 ; i < im->IFV->nb_qualvar+1;i++)
        {
          if (im->IR->qual_var_used[i]) 
            {
              k++;
              outfile << im->IFV->qual_var_name[i];
              if (k != im->IR->nb_qualvar) {outfile << "~";}
            }
          
        }
      outfile << "\n\n";
      outfile << "List of modalities (" << im->IR->nb_mod << ")\n";
      k = 0;
      for (int i = 0; i < im->DT_train->nvar; i++)
        {
          if (im->IR->binary_var_used[i]) 
            { 
              k++;
              for (char c : im->DT_train->varnames[i])
                {
                  if (c == '.') {continue;}
                  outfile << c;
                }
              if (k != im->IR->nb_mod) {outfile <<"~";}
            }
        }
      outfile << "\n\n";
      outfile << "List of Rules (" << im->IR->nb_rule_red << ")\n";
      for (int i = im->IR->nb_rule_red-1; i >= 0 ; i--)
        {
          for (int c = 0; c < im->IR->Tab_name[i].size()-1;c++)
            {
              if (im->IR->Tab_name[i][c] == '.') {continue;}
              if (im->IR->Tab_name[i][c] == ',') {outfile << "."; continue;}
              outfile << im->IR->Tab_name[i][c];
            }
          outfile << "\n";
        }      

      outfile << "\n\n";
      outfile.close();
    }
}


void export_IRF (info_mining * im)
{

  int tot_qual_var = 0; int tot_binary_var = 0; int k = 0;
  for (int i = 0; i < im->DT_train->nvar;i++) {tot_binary_var = tot_binary_var + im->IRF->binary_var_used[i];}
  for (int i = 0; i < im->IFV->nb_qualvar+1; i++) {tot_qual_var = tot_qual_var + im->IRF->qual_var_used[i];}
  std::ofstream outfile(im->IP->pathexport,std::ios::app);
  if (!(outfile.is_open())) {std::cout << "ERROR FILE \n"; return;}
  if (outfile.is_open())
    {
      outfile << "Results #all Eta Values# for " << im->pathname << " with reduction " << *(im->type_red) << "\n";
      outfile << "Number of Variables/Modalities : " << tot_qual_var << "-";
      outfile << tot_binary_var << "\n\n";
      outfile << "List of variables (" << tot_qual_var << ")\n";
      k = 0;
      for (int i = 0 ; i < im->IFV->nb_qualvar+1;i++)
        { 
          if (im->IRF->qual_var_used[i]) 
            {
              k++;
              outfile << im->IFV->qual_var_name[i];
              if (k != tot_qual_var) {outfile << "~";}
            }
          
        }
      outfile << "\n\n";
      outfile << "List of modalities (" << tot_binary_var << ")\n";
      k = 0;
      for (int i = 0; i < im->DT_train->nvar; i++)
        {
          if (im->IRF->binary_var_used[i]) 
            {
              k++;
              for (char c : im->DT_train->varnames[i])
                {
                  if (c == '.') {continue;}
                  outfile << c ;
                }
              if (k != tot_binary_var) {outfile << "~";}
            }

        }
      outfile << "\n";
      outfile.close();
    }

}


void export_IRF_LL (info_mining * im)
{
  std::string * names_red = new std::string [4];
  names_red[0] = "MDR"; names_red[1] = "MIR";
  names_red[2] = "GMDR"; names_red[3] = "GMIR";
  std::ofstream outfile(im->IP->pathexport,std::ios::app);
  if (!(outfile.is_open())) {std::cout << "ERROR FILE \n"; return;}
  int k = 0;
  for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < im->IE->nb_eta_values; j++)
        {
          k = 0;
          im->IE->cur_eta = im->IE->eta_values[j];
          im->IE->cur_sum_Y = im->IE->sum_Y_eta[j];
          im->IR = im->LL->Tab_IR[i][j];
          outfile << "Results for " << im->pathname << " with reduction " << names_red[i] << "\n";
          outfile << "The target is " << *(im->IP->tgt_name);
          outfile << " Eta value = " << (float)im->IE->cur_eta/(float)100000 << "   ";
          outfile << " (" << im->IE->cur_sum_Y << ")\n";
          outfile << "Number of Rules/Variables/Modalities : " << im->IR->nb_rule_red << "-";
          outfile << im->IR->nb_qualvar << "-";
          outfile << im->IR->nb_mod << "\n\n";
          outfile << "List of variables (" << im->IR->nb_qualvar << ")\n";

          for (int i = 0 ; i < im->IFV->nb_qualvar+1;i++)
            {
              if (im->IR->qual_var_used[i]) 
                {
                  k++;
                  outfile << im->IFV->qual_var_name[i];
                  if (k != im->IR->nb_qualvar) {outfile << "~";}
                }
              
            }
          k = 0;  
          outfile << "\n\n";
          outfile << "List of modalities (" << im->IR->nb_mod << ")\n";
          for (int i = 0; i < im->DT_train->nvar; i++)
            {
              if (im->IR->binary_var_used[i]) 
                {
                  k++;
                  for (char c : im->DT_train->varnames[i])
                    {
                      if (c == '.') {continue;}
                      outfile << c;
                    }
                  if (k != im->IR->nb_mod) {outfile <<"~";}
                }
            }
          outfile << "\n\n";
          outfile << "List of Rules (" << im->IR->nb_rule_red << ")\n";
          for (int i = im->IR->nb_rule_red-1; i >= 0 ; i--)
            {
              for (int c = 0; c < im->IR->Tab_name[i].size()-1;c++)
                {
                  if (im->IR->Tab_name[i][c] == '.') {continue;}
                  if (im->IR->Tab_name[i][c] == ',') {outfile << "."; continue;}
                  outfile << im->IR->Tab_name[i][c];
                }
              outfile << "\n";
            }
          outfile << "\n";  
        }      
      outfile << "\n";
      im->IRF = im->LL->Tab_IRF[i];
      int tot_qual_var = 0; int tot_binary_var = 0; 
      for (int i = 0; i < im->DT_train->nvar;i++) {tot_binary_var = tot_binary_var + im->IRF->binary_var_used[i];}
      for (int i = 0; i < im->IFV->nb_qualvar+1; i++) {tot_qual_var = tot_qual_var + im->IRF->qual_var_used[i];}

      outfile << "Results #all Eta Values# for " << im->pathname << " with reduction " << names_red[i] << "\n";
      outfile << "Number of Variables/Modalities : " << tot_qual_var << "-";
      outfile << tot_binary_var << "\n\n";
      outfile << "List of variables (" << tot_qual_var << ")\n";
      k = 0;
      for (int i = 0 ; i < im->IFV->nb_qualvar+1;i++)
        {
          if (im->IRF->qual_var_used[i]) 
            {
              k++;
              outfile << im->IFV->qual_var_name[i];
              if (k != im->IFV->nb_qualvar) {outfile << "~";}
            }
        }
      k = 0;
      outfile << "\n\n";
      outfile << "List of modalities (" << tot_binary_var << ")\n";
      for (int i = 0; i < im->DT_train->nvar; i++)
        {
          if (im->IRF->binary_var_used[i]) 
            {
              k++;
              for (char c : im->DT_train->varnames[i])
                {
                  if (c == '.') {continue;}
                  outfile << c ;
                }
              if (k !=  tot_binary_var) {outfile << "~";}
            }

        }
      outfile << "\n\n\n";
    }

 outfile.close();
 delete [] names_red;
}




void final_gest_mem (info_mining * im)
{
   int nb_mod = im->DT_train->nvar;   
   for (int i = 0; i < 2; i++) {free(im->Y_1_eta[i]);}
   free(im->Y_1_eta);

   free(im->tgt_train->Y);
   free(im->tgt_train);

   for (int i = 0; i < nb_mod; i++) {free(im->STKR->TSR[i]);}
   free(im->STKR->TSR); 
   free(im->STKR->FR);
   free(im->STKR);
   
   delete im->IFV->mappy_qual;
   delete [] im->IFV->qual_var_name;
   for (int i = 1; i < im->IFV->nb_qualvar+1; i++) 
    {
      qual_var * qv = im->IFV->tab_qual_var[i];
      qual_var * qv2;
      while (qv)
        {
          qv2 = qv;
          qv = qv->brother;
          free(qv2);
        }

    }
   free(im->IFV->tab_qual_var);
   free(im->IFV->rdm_order);
   free(im->IFV);


   if (im->IE->reg_or_pred == 'R')
    {
      delete [] im->IR->Tab_name;
      free(im->IR->Tab_nb_mod);
      free(im->IR->rules_red);
      free(im->IR->qual_var_used);
      free(im->IR->binary_var_used);
      free(im->IR);
      free(im->IRF->binary_var_used);
      free(im->IRF->qual_var_used);
      free(im->IRF);
    }

  if (im->IE->reg_or_pred == 'L')
    {

      for (int i = 0; i < 4; i++)  
        {
          
          for (int j = 0; j < im->IE->nb_eta_values;j++) 
            {
              delete [] im->LL->Tab_IR[i][j]->Tab_name;
              free(im->LL->Tab_IR[i][j]->Tab_nb_mod);
              free(im->LL->Tab_IR[i][j]->rules_red);
              free(im->LL->Tab_IR[i][j]->binary_var_used);
              free(im->LL->Tab_IR[i][j]->qual_var_used);
              free(im->LL->Tab_IR[i][j]);
            }
          free(im->LL->Tab_IR[i]);
          free(im->LL->Tab_IRF[i]->binary_var_used);
          free(im->LL->Tab_IRF[i]->qual_var_used);
          free(im->LL->Tab_IRF[i]);
        }
      free(im->LL->Tab_IR);
      free(im->LL->Tab_IRF);
      free(im->LL);
    }


  free(im->IT->order);
  free(im->IT->order_var);
  free(im->IT->TAB_MSP);
  free(im->IT->TAB_NB_TREE);
  free(im->IT->TAB_NB_VAR_TREE);
  free(im->IT);  


  free(im->IE->eta_values);
  for (int i = 0; i < im->IE->nb_eta_values;i++) {free(im->IE->tab_Y_eta[i]);}
  free(im->IE->tab_Y_eta);
  free(im->IE->sum_Y_eta);
  free(im->IE);

   destruct_dta (im->DT_train);
   delete [] im->IP->tab_tgt_complet_name;
   delete [] im->IP->tab_tgt_short_name;
   free(im->IP);
   free(im);
}
