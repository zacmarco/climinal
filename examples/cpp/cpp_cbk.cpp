#include "cpp.h"
#include <iostream>
#include <cstdio>
#include <string>

namespace {

    std::string endl("\n");

    class stream{
        private:
            FILE *file;
        public:
            stream(){
                file=NULL;
            }
            stream(FILE *f){
                file=f;
            }
            stream& operator<<(std::string s) {
                if(file) {
                    fprintf(file, "%s", s.c_str());
                }
                return *this;
            }
            stream& operator<<(const char *s) {
                if(file && s) {
                    fprintf(file, "%s", s);
                }
                return *this;
            }
    };
} 

/* COMMAND: test		( main )		*/
int test_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line, void *cookie)
{
    std::string *task=reinterpret_cast<std::string*>(cookie);
    stream os(out);

    os << "Test callback for thread " << *task << endl;

    return 0;
}
