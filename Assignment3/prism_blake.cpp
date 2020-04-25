/*
Blake Kohlbrecher
Data Science Assisgnment 3
Implement the PRISM Algorithm

PRISM  Algorithm:
    Step 1: calculate the probability of occurrence, p(delta of n | alpha of x), of the classification 
        delta of n for each attribute-value pair alpha of x,
    Step 2: select the alpha of x for which p(delta of n | alpha of x) is a maximum and create a subset of the
        training set comprising all the instances which contain the selected alpha of x,
    Step 3: repeat Steps 1 and 2 for this subset until it contains only instances of class
        delta of n. The induced rule is a conjunction of all the attribute-value pairs used
        in creating the homogeneous subset.
    Step 4: remove all instances covered by this rule from the training set,
    Step 5: repeat Steps 1-4 until all instances of class delta of n have been removed. 
*/

#include <string>
#include <iostream>
#include <math.h>
#include <fstream>
#include <vector>

using namespace std;

class attribute{
    public:
    string nameAtt;
    vector<string> values;
    void addname (string newName){
        nameAtt = newName;
    }
    void addvalue (string newValue){
        values.push_back(newValue);
    }
    void output(){
        cout << "Attribute Name: " << nameAtt << endl;
        cout << "Attribute Values: ";
        for (unsigned i = 0; i < values.size(); i++){
            cout << values[i] << " ";
        }
    }
};

class instance{
    public:
    vector<string> atts;
    void addAtt(string newAtt){
        atts.push_back(newAtt);
    }
    void output(){
        cout<< endl << "Instance: ";
        for (unsigned i = 0; i < atts.size(); i++) cout << atts[i] << " ";
    }
};

class rule{
    vector<string> values;
    public:
    void addValue(string newVal){
        values.push_back(newVal);
    }
    void output(){
        for (unsigned i = 0; i < values.size()-1; i++)cout << values[i] << "^";
        cout << "->" << values[values.size()-1] << endl;
    }
};

void fileInput(int a, vector<attribute *>& attributes, int b, vector<instance *>& instances);

void makeRule(int a, vector<attribute *> & attributes, vector<instance *>& instances);

int main(){
    vector<attribute *> attributes;
    vector<instance *> instances;
    vector<rule *> rules;
    int a = 0;  
    int b = 0;  
    fileInput(a, attributes, b, instances);
    for (unsigned i = 0; i < attributes.size(); i++) attributes[i]->output();
    for (unsigned i = 0; i < instances.size(); i++) instances[i]->output();

    return 0;
}

void fileInput(int a, vector<attribute *>& attributes, int b, vector<instance *>& instances){
    ifstream file;
    string fname;
    string fileinput;
    cout << "file name: ";
    cin >> fname;
    cout << endl;
    file.open(fname);
    file >> fileinput;
    while(fileinput != "@data"){
        if (fileinput == "@attribute")
        {
            string stuff;
            string attVal = "";
            file >> stuff;
            attributes.push_back(new attribute);
            attributes[a]->addname(stuff);
            file >> stuff;
            int i = 1;
            while (stuff[i] != '}'){
                if (stuff[i]==','){
                    i=0;
                    attributes[a]->addvalue(attVal);
                    file >> stuff;
                    attVal = "";
                }
                else if (stuff[i+1] == '}'){
                    attVal.push_back(stuff[i]);
                    i++;
                    attributes[a]->addvalue(attVal);
                }
                else{
                    attVal.push_back(stuff[i]);
                    i++;
                }
            }
            a++;
        }
        file >> fileinput;
    }
    file >> fileinput;
    while (file){
        string attVal = "";
        int i = 0, commas = 0;
        instances.push_back(new instance);
        while (commas < 4){
            if (fileinput[i]==','){
                i+=1;
                instances[b]->addAtt(attVal);
                attVal = "";
                commas++;
            }
            else{
                attVal.push_back(fileinput[i]);
                i++;
            }
        }
        attVal = fileinput.substr(i);
        instances[b]->addAtt(attVal);
        b++;
        file >> fileinput;
    }
    file.close();
}

void makeRule(int a, vector<attribute *> & attributes, vector<instance *>& instances){

}