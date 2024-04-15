#ifndef CAC_SUFRULES_H
#define CAC_SUFRULES_H

#include "Cac_init.h"
#include "Order_Rules.h"



void CBF2 (uint64_t * Ultab, int *ind,int* mod,uint64_t * ptul, uint64_t * nlgtabl,  int good_nbc, int rst);
void width_Rules (uint64_t ** BDT, uint64_t ** nom_noeuds, short * labelmin, int nb, uint64_t * y ,uint64_t * no_y, short size, int maxul, info_mining * im);
void SufRules (info_mining * im);
void final_elag_simple (info_mining * im);
int check_if_supset_TR (rule **** MSR, int *** TSS, int ** TSN, int num_sousarbre, int taille_rule ,uint64_t * name, int sup, int ncm);
void update_FR_selec_2 (info_mining * im);
void search_rule_1 (int nmb, uint64_t * Supreme, info_mining * im);
void clean_next_step_im (info_mining * im);
uint64_t count_number_final_rules (rule ** FR, int nb_mod);


#endif