#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include "cpp.h"

void entry_point( std::string name, const std::string& inputfile, const std::string& outputfile )
{
    FILE *ifile, *ofile;
    if( inputfile.length() ) {
        ifile=fopen(inputfile.c_str(), "r");
    } else {
        ifile=stdin;
    }
    if( outputfile.length() ) {
        ofile=fopen(outputfile.c_str(), "w+");
    } else {
        ofile=stdout;
    }

    climinal_main(ifile, ofile, getmaincontext_cpp(), &name);

    if( inputfile.length() ) {
        fclose(ifile);
    }
    if( outputfile.length() ) {
        fclose(ofile);
    }
}

int main()
{
    std::thread threadSTD(entry_point, "ThreadSTD", "", "");
    std::thread threadA(entry_point, "ThreadA", "ia.txt", "oa.txt");
    std::thread threadB(entry_point, "ThreadB", "ib.txt", "ob.txt");
    threadSTD.join(); 
    threadA.join(); 
    threadB.join(); 

    return 0;
}
