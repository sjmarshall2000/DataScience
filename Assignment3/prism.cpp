#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>
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
    string toString(){return value;}
};
struct classification{
    string value;
    classification(string value="null"){
        this->value = value;
    }
    string toString(){return value;}
    inline bool operator == (const classification &rhs){//TODO: inline???
        return this->value.compare(rhs.value) == 0; //are both value strings equal?
    }
};
struct rule{
    //char arrow = '\u21D2';
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
double relativeFreq(classification c, dataSet &d){ //calculate ratio of class c / not class c in dataset d
    int numInstances = 0;
    int dataSize = d.instances.size();
    for(auto &t : d.instances){
        if(t.thisClass == c){numInstances++;}
    }
    double freq = numInstances / dataSize;
    return freq;

} 
void buildAttribute(string name, string line, dataSet &d){//create attribute defined in line and add to dataSet d
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
    //attributeValues.push_back("?");
    d.attributeList.insert(pair<string,vector<string>>(name, attributeValues)); //add a new entry to attributeList in dataSet d
} 
void buildInstance(string line, vector<string> &attributeNames, int attributeNum, dataSet &d, bool rejectMissing=true){//create instance from line and add to d
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
//thanks Blake for this parser, I changed it a bit
void fileInput(string fname, dataSet &d){
    ifstream file;
    string inputString;
    vector<string> attributeNames; //this one had better preserve order
    file.open(fname);
    file >> inputString;
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
                buildAttribute(attributeName, inputString, d);//attribute now in dataSet d
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
            buildInstance(inputString, attributeNames, attributeCount, d);
        }
    }
    file.close();
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

}