# Cac

## Description 

This page propose one versions of the Cac C++ programs which can be used bu using GCC compiler and the command make.
These algorithms were built around the notion of Mining Frequent ItemSets and associations rules. This data analysis method was first introduced by Agrawal et al. 1993 for mining transaction databases.
More precisely, follow the following articles :

* A Recursive Algorithm for Mining Association Rules.  Springer Nature COMPUTER  SCIENCE,      3, 384 (2022).   https://doi-org.ezproxy.universite-paris-saclay.fr/10.1007/s42979-022-01266-y
* SufRec, an algorithm for mining association rules: Recursivity and task parallelism.  Expert    Systems with Applications, Volume 236, 121321  (2024).   https://doi.org/10.1016/j.eswa.2023.121321
* Association rules and decision rules.  Statistical Analysis and Data Mining, Volume 16, Issue5, (2023),  p. 411-435.  https://doi.org/10.1002/sam.11620:
* Data Analysis and Characterization with Association Rules.


This version allows the realisation of a characterization of variables, defined in the article " Data Analysis and Characterization with Association Rules"


Available databases have been compressed. Information about these databases is given in the databases_test folder.


**Cac:**
* Search for association rules with choice of parameters
* Carrying out the characterization
* Choice of characterization reduction.
* Gives an export in txt format of the requested characterization



## Creating binaries and getting started
```
cd src/ && make

```

## Dataset format 

For this version, the Dataset format accepted is a file of type transaction, such that the items are separated by a choosen separator
and the transaction by a new line.Please, now that the sep must be one of the ASCII Chart. 
The following tab is an example with sep=, item a b c and d, and five transactions:



|transactions|
|------------|
|a,b|
|c,d|
|a,c|
|a,b,d|
|c|


The folder sample contain a simple small dataset test. You can use it for the example below to see how the software works.
For more information about the program settings, please read the readme of the src folder

### Example
```
Cac in src

./Cac ../../sample/Mushroom_Cac.txt v0.e d=, A=1 B=1 C=1000 D=1 GMIR R Do the characterization of variable "v0.e" with "GMIR" reduction.
./Cac ../../sample/Mushroom_Cac.txt v0.e,v22.d d=, A=1 B=1 C=1000 D=1 GMIR R Do the characterization of variable product "v0.e and v22.d" with GMIR reduction.
./Cac ../../sample/Mushroom_Cac.txt v0.e d=, A=1 B=1 C=1000 D=1 GMIR R pathname Do the characterization of variable "v0.e" with "GMIR" reduction and get export in ../../sample/filename
./Cac ../../sample/Mushroom_Cac.txt v0.e d=, A=1 B=1 C=1000 D=1 0 L ../../sample/pathname Do the characterization of variable "v0.e" with the 4 available reductions and get export in pathname.  

```


## Definitions of frequent itemSets :

Let us remind you the 2 mains definitions of this data analys method

Let I = {a1, . . . , an} be a finite set of items. A transaction database is a set of transactions T =
{t1, . . . , tN } where each transaction ti ⊂ I, 1 ≤ i ≤ N, represents a nonempty
subset of items. An itemset A is a subset of I; A is a k-itemset if it contains
k items. The support of an itemset A is denoted as supp(A) and is defined
as the number of transactions which contain A. The relative support of A is
freq(A) = supp(A)/N. A is frequent if freq(A) ≥ σ where σ is a user-specified minimum relative support threshold, called minSup.


**Definitions of confident associations rules  :**
An association rule is an implication A ⇒ B where A and B are two itemsets. The support of a rule A ⇒ B is defined as sup(A ⇒ B) = sup(A∪B).
The precise mentions of association rules used in this algorithm are defined in the articles Association rules and decision rules and Data Analysis and Characterization with Association Rules.







