#pragma once
// #include "arffParser.h"
//All other includes are rolled into arffParser.h
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

// struct attribute;
// struct classification;
// struct instance;
// struct dataSet;
// struct attributeValuePair;
// struct rule;

struct attribute{
    string name;
    string value;
    vector<string> *possibleValues;
    bool isMissing = false;
    attribute(string name, string value, vector<string> *possibleValues){ 
        this->name = name;
        this->value = value;
        this->possibleValues = possibleValues;
        isMissing = value == "?";
    }
    inline bool operator == (const attribute &rhs){//TODO: inline???
        return  (this->name == rhs.name) && //are both name strings equal?
                (this->value == rhs.value); //are both value strings equal?
    }
    string toString(){return value;}
};
struct classification{ //This might be abandoned here soon, since I just store most classes as strings
    string value;
    classification(string value="null"){
        this->value = value;
    }
    string toString(){return value;}
    inline bool operator == (const classification &rhs){
        return this->value == rhs.value; //are both value strings equal?
    }
};
struct attributeValuePair{ //kinda a silly class, its just a shell around attribute
    attribute *a = NULL;
    attributeValuePair(string name, string value, vector<string> * possibleValues){
        a = new attribute(name, value, possibleValues);
    }
    attributeValuePair(attribute &att){
        a = new attribute(att.name, att.value, att.possibleValues);
    }
    inline bool operator == (const attributeValuePair &rhs){//TODO: inline???
        return this->a->name == rhs.a->name && this->a->value == rhs.a->value; //reuse the operator written for attribute
    }
    string toString(){
        string output;
        output += "#Attribute:" + a->name + ", Value:"+a->value+"|";
        return output;
    }
    // ~attributeValuePair(){ //since I made attribute a with new
    //     delete a;
    // }
};
struct rule{
    vector<attribute> conditions;
    classification result;
    void addClass(classification result){
        this->result = result;
    }
    void addClass(string result){
        classification c(result);
        addClass(c);
    }
    void addAttribute(attribute a){
        conditions.push_back(a);
    }
    void addAttribute(attributeValuePair avp){
        conditions.push_back(*(avp.a));
    }
    inline bool operator == (const rule &rhs){//TODO: inline???
        if(this->result.value != rhs.result.value){return false;}
        if(this->conditions.size() != rhs.conditions.size()){return false;}
        // vector<attribute>::iterator it;
        // for(it){
            
        // }//TODO: make this actually compare attribute vectors
        return true;
        
    }
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
    bool hasPair(attributeValuePair &avp){
        for(attribute &a : attributes){
            if(a.name == avp.a->name){
                if(a.value == avp.a->value){return true;}
            }
        }
        return false;
    }
    bool matchesRule(rule r){
        if(thisClass.value == r.result.value) { //if they arent same class
            return false;
        }
        for(attribute &a : r.conditions){
            bool matched=false;
            for(attribute &att : attributes){
                if(a.name==att.name && a.value==att.value){
                    matched=true;
                }
            }
            if(!matched){return false;}
        }
        return true;
    }
    inline bool operator == (const instance &rhs){
        return  (this->thisClass.value == rhs.thisClass.value) && //are both name strings equal?
                (this->attributes.size() == rhs.attributes.size()); //are both value strings equal?
                //TODO: make this actually check attribute vectors
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
    int numClass(string c){
        int numInstances = 0;
        for(instance &i : instances){
            if(i.thisClass.value == c){numInstances++;}
        }
        return numInstances;
    }
    int numClass(classification c){
        return numClass(c.value);
    }
    void updateClassList(){
        classList.clear();
        for (instance &i : instances){
            //if its not already in there
            if(count(classList.begin(), classList.end(),i.thisClass.value) == 0){
                //add it
                classList.push_back(i.thisClass.value);
            }
        }
    }
    void removeElementsMatching(rule r){
        vector<instance>::iterator it;
        for (it = instances.begin(); it != instances.end();){
            if(it->matchesRule(r)){
                it = instances.erase(it);
            } else {++it;}
        }
        updateClassList();
    }
    void removeElementsNotMatching(attributeValuePair avp){
        vector<instance>::iterator it;
        for (it = instances.begin(); it != instances.end();){
            if(it->hasPair(avp)){
                it = instances.erase(it);
            } else {++it;}
        }
        updateClassList();
    }
    dataSet(){
        updateClassList();
    }
    dataSet(dataSet &other){ //copy constructor
        this->classList = other.classList;
        this->attributeList = other.attributeList;
        this->instances = other.instances;
        //this->numClass = other.numClass;
        this->relationName = other.relationName;
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

void buildAttribute(string name, string line, dataSet &d, bool rejectMissing){//create attribute defined in line and add to dataSet d
    //time to cleanup the line, it looks like this:"{value,value-2,value-3,value-ect}"
    line.erase(remove(line.begin(), line.end(), '{'), line.end());
    line.erase(remove(line.begin(), line.end(), '}'), line.end());
    line.erase(remove(line.begin(), line.end(), ' '), line.end());
    
    line = line.append(",");           //trust me everything's easier when there's a comma after each attribute
    vector<string> attributeValues;
    stringstream ss(line);
    string val;
    while(getline(ss, val, ',' )){ //get line until the next ',' char, until end of whole string
        attributeValues.push_back(val);
    }

    if(!rejectMissing){attributeValues.push_back("?");}
    d.attributeList.insert(pair<string,vector<string>>(name, attributeValues)); //add a new entry to attributeList in dataSet d
} 
void buildInstance(string line, vector<string> &attributeNames, int attributeNum, dataSet &d, bool rejectMissing){//create instance from line and add to d
    //cout << "line length: " <<line.length() << endl;
    for(int i = 0; i < line.length(); i++){if(!isprint(line[i]) || line[i] == ' '){line.erase(line.begin() + i);}}
    //line.replace(line.begin(), line.end(), ',', ' '); //gee this is easier than commas 
    //above commented line is broken, this is equivalent:
    for(int i = 0; i < line.length(); i++){if(line[i] == ','){line[i] = ' ';}}

    stringstream ss(line);
    string nextAttributeValue;
    string className;

    vector<attribute> attributes;
    for(int i = 0; i < attributeNum; i++){ //we want to hold off on the class for now
        ss >> nextAttributeValue;
        string attName = attributeNames[i];
        //cout << attName << "|";
        // for(auto &a : d.attributeList.at(attName)){
        //     cout <<"'"<<a<<"'";
        // }
        //cout << "|selected:'" << nextAttributeValue <<"'\n"; //TODO: why this no work
        if(count(nextAttributeValue.begin(), nextAttributeValue.end(), '?') != 0){
            //Its a missing value, oh no!
            if(rejectMissing){
                return; //this is how I handle most my problems
            } else {
                //TODO: handle missing - extra credit
            }
        }
        int occurances = count(d.attributeList.at(attName).begin(), d.attributeList.at(attName).end(), nextAttributeValue);
        //cout << " occurances: " << occurances <<endl;
        assert(occurances != 0); //assert the attribute value is one of th epossible values for that attribute

        attribute att(attName, nextAttributeValue, &d.attributeList.at(attName));
        attributes.push_back(att);
    }
    ss >> className;
    // cout << "Classlist: "<<endl<<"'";
    // for(auto &c : d.classList){
    //     cout << c << "'";
    // }
    // cout  << endl;
    int occurances = count(d.classList.begin(), d.classList.end(), className);
    // cout << className <<"|"<< occurances << endl;
    assert( occurances != 0);//the className must be in classlist
    
    classification thisClass(className);

    instance inst(thisClass, attributes);
    //cout << className << "#"; //crappy debugging, ignore
    d.instances.push_back(inst);
}
void addClasses(string line, dataSet &d){//add classes from line to dataSet d
    //cout << "initial class line:|" << line <<endl;
    line.erase(remove(line.begin(), line.end(), ' '), line.end()); //get rid of spare spaces
    line.erase(remove(line.begin(), line.end(), '{'), line.end());
    line.erase(remove(line.begin(), line.end(), '}'), line.end());
    //cout << "final   class line:|" << line <<endl;
    //line.replace(line.begin(), line.end(), 'e', '#'); //<--why this doesn't work, who knows 
    for(int i = 0; i < line.length(); i++){if(line[i] == ','){line[i] = ' ';}} //this is the same thing

    stringstream ss(line);
    string className;
    while(ss >> className){
        d.classList.push_back(className);
    }

}
//thanks Blake for this parser, I changed it a bit (a lot)
void fileInput(string fname, dataSet &d, bool rejectMissing){
    ifstream file;
    string inputString;
    vector<string> attributeNames; //this one had better preserve order
    file.open(fname);
    file >> inputString;
    //this requires '@DATA', etc. to be all caps, I dont care enough to work around it
    while(inputString != "@DATA"){ //until instances
        if(inputString == "@RELATION") { //get relation line
            string relation;
            file >> relation;
            d.relationName = relation;
        }
        if (inputString == "@ATTRIBUTE"){ //get a new attribute
            string attributeName;
            file >> attributeName;
            if(attributeName == "class"){
                getline(file, inputString);
                addClasses(inputString,d);//"its like freshman orientation all over again"
            } else {
                //Time to make a new attribute
                attributeNames.push_back(attributeName); //keep track of the names, this might be handy :P
                //getline(file, inputString);//whole attribute's in there now
                getline(file, inputString, '{');//TODO: confirm if this is dumb
                getline(file, inputString, '}');
                buildAttribute(attributeName, inputString, d, rejectMissing);//attribute now in dataSet d
            }
        }
        file >> inputString;
    } 
    int attributeCount = attributeNames.size();
    
    // cout << "ATTRIBUTES:"<<endl;
    // for(auto &p : d.attributeList){
    //     for( auto &s : p.second){
    //         cout << s << "|length:"<<s.length()<<endl;
    //     }
    // }

    //"@DATA" is in inputString now
    getline(file, inputString); // skip a line, this is dirty I know
    getline(file, inputString); // you do crazy things when you are desperate
    while(getline(file, inputString)){
        if(inputString.find('%') == string::npos){ //if the line is not comment
            buildInstance(inputString, attributeNames, attributeCount, d, rejectMissing);
        }
    }
    file.close();
}