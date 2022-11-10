#include <iostream>
#include <map>
#include <vector>
#include <pthread.h>
#include <iostream>
#include <fstream> 
#include <filesystem>

using namespace std;

int convert(std::string s) 
{ 
    // Initialize a variable 
    int num = 0; 
    int n = s.length(); 
  
    // Iterate till length of the string 
    for (int i = 0; i < n; i++) 
  
        // Subtract 48 from the current digit 
        num = num * 10 + (int(s[i]) - 48); 
  
    // Print the answer 
    return num;
} 

void *mapF(void *arg)
{
    long id = *(long*) arg;
    //daca id apartine ids thread uri mapp 
    //cod pt map
    //altfel cod pt reducer
    std::cout<< "hello from map thread "<<id<<endl;
    return NULL;
}

void *reduceF(void *arg)
{
    long id = *(long*) arg;
    //daca id apartine ids thread uri mapp 
    //cod pt map
    //altfel cod pt reducer
    std::cout<< "hello from reduce thread " << id << endl;
    return NULL;
}

int readInputFile (string fileName, vector <string> *filesName){
    //citesc nr de fisiere si numele fiserelor ce contin puterile
    int nrFisiere = 0;
    cout <<"citesc din fisierul "<<fileName<<endl;

    fstream my_file;
	my_file.open(fileName, ios::in);

	if (!my_file) {
		cout << "No such file\n";
	}
	else {
		my_file >> nrFisiere;
        string nameFile;
        getline(my_file, nameFile); // ca sa scap de sf primei linii
		for(int i = 0; i < nrFisiere && !my_file.eof(); i++){
            getline(my_file, nameFile);
            filesName->push_back(nameFile);
        }
	}
	my_file.close();

    return nrFisiere;
}

//ifstream is ("test.txt", std::ifstream::binary);
int getFileSize(string name){
    ifstream file( name, ios::binary | ios::ate);
    return file.tellg();
}

long getTotalFileSize(vector<string> mapFiles){
    long totalSize = 0;
    for(unsigned int i = 0; i < mapFiles.size(); i++){
        totalSize += getFileSize(mapFiles.at(i));
    }

    return totalSize;
}

void addPath(int inputsFileNo, vector<string> *mapFiles){
  //add path ./ to files
  for(int i = 0; i < inputsFileNo; i++){
    string finalString = mapFiles->at(i);
    string pathTo = "./";
    pathTo.append(finalString);
    mapFiles->at(i) = pathTo;    
  }
}

int main(int argc, char *argv[])
{

  std::map<int, std::vector<int> > perfectPow;

  perfectPow[2] = {1,4};
  perfectPow[3] = {8, 27};
  perfectPow[4] = {16, 81};
  std::cout<< perfectPow[2][1] << std::endl;
  
  if (argc < 3){
    std::cout<< "Numar de parametri incorect.\nUsage: ./a.out <numar_mapperi> <numar_reduceri> <fisier_intrare>\n";
    return -1;
  }
  int nrMappers, nrReducers;
  std::string fileInputs;
  string initialFileName = "./";
  nrMappers = convert(argv[1]);
  nrReducers = convert(argv[2]);
  fileInputs = argv[3];
  vector<string> mapFiles;

  int inputsFileNo = readInputFile(fileInputs, &mapFiles);
  // am citit param
  // thread-uri totale = nrMapp + nrReducers
  // cum impart eficient fisierele pt fiecare mapper 
  int NUM_THREADS = nrMappers + nrReducers;

  pthread_t threads[NUM_THREADS];
  int r;
  long id;
  void *status;
  long arguments[NUM_THREADS];
  //impartirea thread urilor pt mapper
  // total size of files / nr of map threads
    addPath(inputsFileNo, &mapFiles);
    long totalSize = getTotalFileSize(mapFiles);
    // key = thread_map_id value = file names 
    map<int, vector <string> > mapThreadFiles;
    cout << totalSize / nrMappers << endl;
    for(int i = 0; i < inputsFileNo; i++){
        cout << "files size pt " << mapFiles.at(i) << " este = " << getFileSize(mapFiles.at(i)) << endl;
    }


    for (id = 0; id < NUM_THREADS; id++) {
        arguments[id] = id;
        if (id < nrMappers){
            // in loc de null pun atribut numele fisierelor pe care trebuie sa le ia fiecare mapper
            r = pthread_create(&threads[id], NULL, mapF, (void *) &arguments[id]);
        }
        else{
            // sa pun o bariera ca dupa ce se term thread urile pt map
            // sa inceapa d abia atunci thread urile pt reduce
            r = pthread_create(&threads[id], NULL, reduceF, (void *) &arguments[id]);
        }
        if (r) {
            exit(-1);
        }
    }

    for (id = 0; id < NUM_THREADS; id++) {
        r = pthread_join(threads[id], &status);

        if (r) {
            exit(-1);
        }
    }


    return 0;
}