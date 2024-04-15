#ifndef ORDER_RULES_H
#define ORDER_RULES_H

#include "Cac_init.h"



void Cfer_tri_groupe (int * sumvec, int * tab_help,  int nb );

void order_scale_1 (int * tab_help, int * tab_val, int nb, int min, int max);

void Cfer_r_order (int * tr_sumvec, int * tab_help, int * tr_help, int iinf,int isup,int nb );

void Cfer_order(int * tab_help, int * sumvec ,int nb_elem, int maxul);
void special_TOV_order(uint64_t ** TOV , int * sumvec ,int nb_elem, int maxul);
void special_rule_order(rule ** tab_to_order, int * sumvec ,int nb_elem, int maxul);
void special_rule_order2(rule ** tab_to_order, uint64_t ** TOV , int * sumvec ,int nb_elem, int maxul);
void special_ecv_order (ecv ** tab_to_order, int * sumvec, int nb_elem, int maxul);
void special_float_order (float * tab_to_order, int nb_elem);

#endif