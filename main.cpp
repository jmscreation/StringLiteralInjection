#include <iostream>
#include "stdio.h"
#include "console.h"
#include "empty.h"
#include "compressor.h"

using namespace std;

int main(int argc, char** argv) {
    do  {
        const char* IMPORT_DATA = EMPTY_CONTENT_BIN;
        string simport = IMPORT_DATA;
        unsigned int sz = 0;
        {   /// Retrieve size of data and get the data
            string szData = simport.substr(0, sizeof(sz));
            if(szData == string(NEEDLE_OPEN).substr(0,szData.size())){
                cout << "Invalid Non-Injected EXE Call" << endl;
                break;
            }
            for(unsigned int i=0;i<szData.size();i++) *((char*)&sz + i) = szData[i];

            simport = simport.substr(sizeof(sz), sz);
        }   //End of size scope

        {   /// Decompress data
            Compressor c;
            if(!c.decompress(simport)){
                cout << "Failed to decompress: " << c.getError() << endl;
                break;
            }
            simport = c.result();
        }

        for(char c : simport){
            cout << c;
            Sleep(20);
        }
    } while(0);

    pause();
    return 0;
}
