CXX = g++

CXXFLAGS = -mpopcnt -msse4 -mabm -mmmx -mavx -mavx -mavx2 -mfma -O3 

Cac: Cac_Proc_selection.o Order_Rules.o Cac_SufRules.o Reduction.o Caracterisation.o Cac_init.o Cac_main.o
	$(CXX) Cac_Proc_selection.o Order_Rules.o Cac_SufRules.o Reduction.o Caracterisation.o Cac_init.o Cac_main.o $(CXXFLAGS) -o Cac 


Cac_init.o: Cac_init.cpp Cac_init.h
	$(CXX) -c Cac_init.cpp $(CXXFLAGS)
Order_Rules.o: Order_Rules.cpp Order_Rules.h Cac_init.h
	$(CXX) -c Order_Rules.cpp $(CXXFLAGS)
Cac_SufRules.o: Cac_SufRules.cpp Cac_SufRules.h Order_Rules.h Cac_init.h
	$(CXX) -c Cac_SufRules.cpp $(CXXFLAGS)	
Reduction.o: Reduction.cpp Reduction.h Cac_init.h
	$(CXX) -c Reduction.cpp $(CXXFLAGS) 
Caracterisation.o: Caracterisation.cpp Caracterisation.h Reduction.h Cac_SufRules.h Order_Rules.h Cac_init.h
	$(CXX) -c Caracterisation.cpp $(CXXFLAGS) 
Cac_Proc_selection.o: Cac_Proc_selection.cpp Cac_Proc_selection.h Caracterisation.h Cac_init.h
	$(CXX) -c Cac_Proc_selection.cpp $(CXXFLAGS)

Cac_main.o: Cac_main.cpp Cac_Proc_selection.h Cac_init.h 
	$(CXX) -c Cac_main.cpp $(CXXFLAGS) 
