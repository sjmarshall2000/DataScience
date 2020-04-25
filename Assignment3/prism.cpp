#include "prism.h"

/*
 prism.h contains an import of arffParser, that means 
 calling prism.h includes everything in arffParser
 It also includes all the #includes needed
 
 arffParser has all the structs, methods and such to get
 the data, all you need to call is create a dataSet var
 and call the fileInput() function, passing a filename
 string and the dataset variable.
 
 */

double relativeFreq(classification c, dataSet &d){ //calculate ratio of class c / not class c in dataset d
    int numInstances = d.numClass(c);
    int dataSize = d.instances.size();
    double freq = numInstances / dataSize;
    return freq;
}
void buildClassifier(dataSet &d){
    
}

int main(int argc, char *argv[]){
    //cout << "Enter filename: ";
    string fileName;
    //cin >> fileName;
    fileName = "soybean.arff";//im tired of typing it in
    cout << endl;

    dataSet data1;
    fileInput(fileName, data1);
    cout << data1.toString(false) << endl;
    cout << "size of dataSet: " << sizeof(data1) << " bytes"<< endl;
}