# Cac

## Description 


**src**
**Indications** 

A main difficulty in characterizing association rules is the extraction of the rules. Depending on the different databases, this mining is more or less difficult and needs to be handled differently. This is why the algorithm offers a set of parameters. These parameters are intended to carry out a characterization on all databases. These may be different on the same basis depending on the mining parameters. Note that the word rules is used here to mention the association rules with Conf2 = 1.

* A,B,C,D : These are parameters to choose for mining rules
* MIR,MDR,GMIR,GMDR : These are parameters to choose for the type of reduction desired
* R,L : These are parameters to designate the type of study. With R you choose a reduction. With L all reductions are made



## Parameters for Cac in src:
|param|required|note|
|--------------------|--------|--------|
|    transaction dataset    |    yes    | The Dataset transaction  | 
|    target   |    yes    | name of target, add - to make a product variable |  
|    Minsup  "A = "   |    yes    | Integer. The minsup value for each block (in per thousand)  | 
|    Nb Tree "B = "   |    yes    | Integer. The number of Trees for each block | 
|    Nb Var Tree "C = "   	  |    yes    | Integer. The number of variables for trees in each block  | 
|    Nb Blocks "D = "   	  |    yes    | Integer. The number of blocks.  | 
|    Type of Reduction   |    yes    | if R in study param. MIR,MDR-GMIR-GMDR. if L, not taken into account | 
|    Type of Study   |    yes    | R to choose Reduction or L for all reduction   |
|    output file     |    no    |  If you wan't the extraction file | 



## Examples 

./Cac ../../sample/Mushroom_Cac.txt v0.e d=, A=1 B=1 C=1000 D=1 GMIR R \
The target is v0.e \
The search for rules will be done in one step (D=1). \
This step will take a value of 1/1000*(number of rows with Y=1, Y being the target) for the minsup (A=1). \
This step will build one tree (C=1) \
For the only one tree, il will take 1000 variables. If this value exceeds the total number of variables, then the maximum number is used \
The reduction function is GMIR 


./Cac ../../sample/Mushroom_Cac.txt v0.e-v22.d d=, A=1-300 B=1-5 C=10-1000 D=2 0 L \
The target is the product of v0.e and v22.d (Y = 1 if v0.e = 1 AND v22.d = 1) \
The search for rules will be done in two step (D=2). \
The parameters of the first step are A = 1, B = 1, C = 10 \
The parameters of the second step are A = 300, B = 5, C = 1000 \
The rules are then grouped together (duplicates are not taken into account) \
All reduction function are used (separately)


## Information about export files 

The export files gives for the requested reduction(s)
For each eta values (please read the article Data Analysis and Characterization with Association Rules. for explanations) :
* Number of raws with Y_eta = 1
* Number and list of rules 
* number of variables and modalities present in rules

It also gives a summary of the modalities and variables used for all eta values.