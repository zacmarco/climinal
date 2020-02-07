#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include "cpp.h"

void entry_point( std::string name, std::string inputfile, std::string outputfile )
{
    FILE *ifile=fopen(inputfile.c_str(), "r");
    FILE *ofile=fopen(outputfile.c_str(), "w+");

    climinal_main(ifile, ofile, getmaincontext_cpp(), &name);

    fclose(ifile);
    fclose(ofile);
}

int main()
{
    std::thread threadA(entry_point, "ThreadA", "ia.txt", "oa.txt");
    std::thread threadB(entry_point, "ThreadB", "ib.txt", "ob.txt");
    threadA.join(); 
    threadB.join(); 

    return 0;
}
