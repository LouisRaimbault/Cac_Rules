#ifndef REDUCTION_H
#define REDUCTION_H

#include "Cac_init.h"
#include "Order_Rules.h"


void fill_x (uint64_t * x,rule * R_FR, uint64_t Bone, info_mining * im);
int RDP_short_reduction (rule ** TOR, int * is_in_supreme,int nb_rules, info_mining * im);
int SET_ORDER_RED (rule ** TOR, int * TOS , int * is_in_supreme ,int nb_rules, info_mining * im);
int MIR_RED (rule ** TOR, int * is_in_supreme ,int nb_rules, info_mining * im);
int MDR_RED (rule ** TOR, int * is_in_supreme ,int nb_rules, info_mining * im);
void CHECK_IECLASS (ecv ** Tab_ecv, int nb, info_mining * im);
void REC_IECLASS (ecv ** Tab_ecv, int nb,int num_func, info_mining * im);
void IECLASS (rule ** TR_TOR, int * TR_TOSR ,int nb, int ** pt_tokeep, int sup ,info_mining * im);
int set_equiv_class (rule *** TOR, int ** TOS, int nb_rules  , info_mining * im);


#endif