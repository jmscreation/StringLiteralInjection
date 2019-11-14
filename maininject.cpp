#include <iostream>
#include <string>

#include "empty.h"
#include "console.h"
#include "injector.h"
#include "compressor.h"

using namespace std;

int main(int argc, char** argv) {
    string filePath = "export.exe";
    string fileInjection;

    cout << "Enter Injection String:" << endl;

    char c;
    while(1){
        c = cin.get();
        if(c == '\n' && fileInjection[fileInjection.size()-1] == '\n') break;
        fileInjection += c;
    }

    Injector i;
    i.loadResource();
    i.setInjection(fileInjection);

    if(!i.inject(filePath)){
        cout << "Failed to export file - check the log:" << endl;
        cout << i.getLog() << endl;
    } else {
        cout << "File exported successfully: " << filePath;
    }

    pause();
    return 0;
}


/*

int main(int argc, char** argv) {
    string path = "Application.exe";
    string fdata; //actual data used for storing file in memory
    fstream file;

    {   /// Import file data from container application
        cout << "Importing file..." << endl;
        file.open(path, ios::in | ios::binary);

        if(!file.is_open()){
            cout << "File not found" << endl;
            return 0;
        }

        stringstream data;
        data << file.rdbuf();   //copy file to stream memory
        file.close();
        fdata = data.str(); //copy data into string memory
    }   // End file memory scope

    string needle = NEEDLE_OPEN, newData;
    size_t length, pos;
    {   /// Search for import location in exe file data
        cout << "Searching for import location..." << endl;
        pos = fdata.find(needle);
        if(pos == string::npos) {
            cout << "Could find target injection offset" << endl;
            return 0;
        }
        length = fdata.find('\0', pos) - pos;
        cout << "Found injection target @" << pos << " of length " << length << endl;
        newData = "This is super cool!";  //This is where we inject the new information

        if(newData.size() + 4 > length){    //Check if the new data is larger than what the target will accept
            cout << "Cannot inject more data than allocated" << endl;
            return 0;
        }
    }   // End searching scope

    {   /// Compression of injection data
        cout << "Compressing data..." << endl;
        Compressor c;
        if(c.compress(newData)){
            string oldData = newData;
            newData = c.result();
            if(!c.decompress(newData) || c.result() != oldData){
                cout << "Compression error: " << (c.errorOccurred() ? c.getError() : "Unknown") << endl;
                return 0;
            }
        } else {
            cout << "Failed to compress: " << c.getError() << endl;
            return 0;
        }
    }   // End compression scope

    unsigned int sz = newData.size();
    {   /// Inject data into memory and fill the rest with null
        cout << "Injecting " << sz << " bytes of new data into memory..." << endl;

        string sizeHeader;    //header to contain size of data - first fill with null bytes
        for(unsigned int i=0; i<sizeof(sz);i++) sizeHeader += *(((char*)&sz) + i); // add each byte from our size data

        length -= sizeHeader.size() + sz; // modify length to leftover data size
        newData = sizeHeader + newData;   // insert header at beginning of newData

        sz = newData.size();    //update the new size of the data

        string _null; _null.reserve(length); for(unsigned int i=0; i<length;i++) _null += '\0'; //null array
        fdata.replace(pos, sz, newData)
             .replace(pos + sz, length, _null);
    }   // End injection scope

    {   /// Flush modified data into the file
        cout << "Flushing to new exe..." << endl;
        file.open(path, ios::out | ios::binary);
         if(!file.is_open()){
            cout << "Error flushing to file" << endl;
            return 0;
        }
        file << fdata;
        file.close();
    }   // End flush scope
    cout << "Task Complete!" << endl;

    return 0;
}

*/
