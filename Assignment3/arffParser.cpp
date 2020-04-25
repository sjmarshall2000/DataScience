#include "arffParser.h"

using namespace std;

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
    attributeValues.push_back("?");
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
            buildInstance(inputString, attributeNames, attributeCount, d, true);
        }
    }
    file.close();
}
