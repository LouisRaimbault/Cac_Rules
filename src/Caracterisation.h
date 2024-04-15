#ifndef CARACTERISATION_H
#define CARACTERISATION_H

#include "Cac_init.h"
#include "Cac_SufRules.h"
#include "Cac_Proc_selection.h"
#include "Order_Rules.h"
#include "Reduction.h"



void finish_SufRules (info_mining * im);
int get_and_order_rules ( rule *** TOR, int ** TOS ,info_mining * im);
void set_IR (rule ** TOR, int * is_in_supreme, int nb_rules , info_mining * im);
void Cac_Carac (info_mining * im);
void Cac_Carac_all (info_mining * im, int eta_num);

#endif