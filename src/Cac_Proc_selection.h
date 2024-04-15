#ifndef CAC_PROC_SELECTION_H
#define CAC_PROC_SELECTION_H
#include "Cac_init.h"
#include "Caracterisation.h"


void order_variables (info_mining * im, int tip);
void set_info_mining (info_mining * im);
void init_info_var (info_mining * im);
void random_selection_var (info_mining * im);
void CAC_selec (info_mining * im);

#endif