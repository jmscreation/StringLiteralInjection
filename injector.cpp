#include "injector.h"

using namespace std;

Injector::Injector() {
    injectionData = "";
    log << "Injector initialized" << endl;
}

Injector::~Injector() {
}

bool Injector::loadResource(int resource){
    resourceData.str("");   //clear buffer
    HMODULE curProc = GetModuleHandle(NULL);
    bool success = false;
    do {
        log << "Finding resource..." << endl;
        HRSRC hres = FindResource(curProc, MAKEINTRESOURCE(resource), RT_RCDATA);
        if(!hres) break;

        log << "Loading resource..." << endl;
        HGLOBAL res = LoadResource(curProc,hres);
        if(!res) break;

        size_t ressize = SizeofResource(curProc,hres);
        const char* resdata = (const char*)LockResource(res);
        if(!resdata) break;

        log << "Resource loaded. Copy to sstream memory..." << endl;
        resourceData.write(resdata, ressize);   //write data to string stream in memory

        log << "Freeing win32 resource..." << endl;
        FreeResource(res);

        if(size_t(resourceData.tellp()) == ressize) success = true;
        log << "Testing written size for match: " << resourceData.tellp() << "==" << ressize << endl;
    } while(0);
    log << "Resource " << (success ? "loaded successfully" : "failed to load") << endl;
    return success;
}


bool Injector::inject(const std::string& path){
    bool success = false;
    unsigned int sz;
    string fdata; //actual data used for storing file in memory
    do {
        {   /// Import file data from container application
            log << "Importing resource..." << endl;
            resourceData.seekg(0, ios::beg);
            fdata = resourceData.str();   //copy file resource into string memory
            if(!fdata.size()){
                log << "Importing failed because there was no resource data to import" << endl;
                break;
            }
        }   // End file memory scope

        string newData = injectionData;  //Injection data copied locally here
        {   /// Compression of injection data
            log << "Compressing data..." << endl;
            Compressor c;
            if(c.compress(newData)){
                string oldData = newData;
                newData = c.result();
                if(!c.decompress(newData) || c.result() != oldData){
                    log << "Compression error: " << (c.errorOccurred() ? c.getError() : "Unknown") << endl;
                    break;
                }
            } else {
                log << "Failed to compress: " << c.getError() << endl;
                break;
            }
        }   // End compression scope


        string needle = NEEDLE_OPEN;
        size_t length, pos;
        {   /// Search for import location in exe file data
            log << "Searching for import location..." << endl;
            pos = fdata.find(needle);
            if(pos == string::npos) {
                log << "Could not find target injection offset" << endl;
                break;
            }
            length = fdata.find('\0', pos) - pos;
            log << "Found injection target @" << pos << " of length " << length << endl;

            if(newData.size() + sizeof(sz) > length){    //Check if the new data is larger than what the target will accept
                log << "Cannot inject more data than allocated" << endl;
                break;
            }
        }   // End searching scope


        sz = newData.size();
        {   /// Inject data into memory and fill the rest with null
            log << "Injecting " << sz << " bytes of new data into memory..." << endl;

            string sizeHeader;    //header to contain size of data - first fill with null bytes
            for(unsigned int i=0; i<sizeof(sz);i++) sizeHeader += *(((char*)&sz) + i); // add each byte from our size data

            length -= sizeHeader.size() + sz; // modify length to leftover data size
            newData = sizeHeader + newData;   // insert header at beginning of newData

            sz = newData.size();    //update the new size of the data

            string _null; _null.reserve(length); for(unsigned int i=0; i<length;i++) _null += '\0'; //null array
            fdata.replace(pos, sz, newData)
                 .replace(pos + sz, length, _null);
        }   // End injection scope

        fstream file;
        {   /// Flush modified data into the exe file
            log << "Flushing to new exe..." << endl;
            file.open(path, ios::out | ios::binary);
             if(!file.is_open()){
                log << "Error flushing to file. Could not open file" << endl;
                break;
            }
            file << fdata;
            file.close();
            success = true;
        }   // End flush scope
    } while(0);
    log << "File " << (success ? "exported successfully" : "failed to export") << endl;

    return success;
}
