
#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include <assert.h>

using namespace std;

struct attribute{
    string name;
    string value;
    vector<string> *possibleValues;
    bool isMissing = false;
    attribute(string name, string value, vector<string> *possibleValues){ 
        this->name = name;
        this->value = value;
        this->possibleValues = possibleValues;
        isMissing = value.compare("?")==0;
    }
    inline bool operator == (const attribute &rhs){//TODO: inline???
        return  (this->name.compare(rhs.name) == 0) && //are both name strings equal?
                (this->value.compare(rhs.value) == 0); //are both value strings equal?
    }
    string toString(){return value;}
};
struct classification{
    string value;
    classification(string value="null"){
        this->value = value;
    }
    string toString(){return value;}
    inline bool operator == (const classification &rhs){
        return this->value.compare(rhs.value) == 0; //are both value strings equal?
    }
};
struct rule{
    vector<attribute> conditions;
    classification result;
    string toString(){
        string output = "";
        for(attribute &a : conditions){ //currently adds one too many '/\'s, dont care for now, TODO: check iter
            output += a.toString();
            output += " /\\ "; // only looks like '/\' 
        }
        output += " => ";
        output += result.toString();
        return output;
    }
};
struct instance{
    classification thisClass;
    vector<attribute> attributes;
    bool hasMissing = false;
    int numMissing=0;
    instance(classification thisClass, vector<attribute> &attributes){
        this->thisClass = thisClass;
        // this->attributes = attributes;
        this->attributes.swap(attributes);
        for(auto &a : attributes){
            if(a.isMissing) {
                hasMissing = true;
                numMissing++;
            }
        }
    }
    string toString(){
        string output;
        output += "class: " + thisClass.toString() + " ";
        for(attribute a : attributes){
            output += a.toString() + "|";
        }
        output += "\n";
        return output;
    }
    
};
struct dataSet{
    string relationName = "Default relation name";
    vector<string> classList;
    vector<instance> instances;
    map<string, vector<string>> attributeList;
    int numClass(classification c){
        int numInstances = 0;
        for(instance &i : instances){
            if(i.thisClass == c){numInstances++;}
        }
        return numInstances;
    }
    string toString(bool printInstances=false){
        string output;
        output += "\nNumber of Instances: " + to_string(instances.size());
        output += "\nNumber of Classes: " + to_string(classList.size());
        output += "\nNumber of Attributes: " + to_string(attributeList.size());
        output += "\n\nClasses:\n\n";
        for(string s : classList){
            output += s + "\n";
        }
        output += "\nAttributes:\n\n";
        for(auto &s : attributeList){
            output += s.first + ": ";
            for(string str : s.second){
                output += str + " ";
            }
            output += "\n";
        }
        if(printInstances){
            int count = 20;
            output += "\nInstances:\n\n";
            vector<instance>::iterator it;
            //it = instances.begin();
            for(auto &inst : instances){

                //instance* inst = &*(it); //thats disgusting, I feel dirty
                
                output += inst.toString();
                output += "\n";
                count++;
            }
        
        }
        return output;
    }
};

void buildAttribute(string name, string line, dataSet &d);
void buildInstance(string line, vector<string> &attributeNames, int attributeNum, dataSet &d, bool rejectMissing);
void addClasses(string line, dataSet &d);
void fileInput(string fname, dataSet &d);