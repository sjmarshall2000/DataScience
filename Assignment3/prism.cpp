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

// double relativeFreq(classification c, dataSet &d){ //calculate ratio of class c / not class c in dataset d
//     int numInstances = d.numClass(c);
//     int dataSize = d.instances.size();
//     double freq = numInstances / dataSize;
//     return freq;
// }
double probability(string &c, attributeValuePair &avp, dataSet &d){
    double prob = 0;
    int numInstances = d.instances.size();
    int numMatches = 0;
    for (instance &i : d.instances){
        if(i.hasPair(avp) && i.thisClass.value==c){
            numMatches++;
        }
    }
    prob = ((double)numMatches / numInstances);
    return prob;
}
void buildRules(dataSet &d, vector<rule> ruleList){
    //for every class in dataset d
    for(string &c : d.classList){
        cout <<"class :" <<c<< endl;
        //vector<rule> ruleList;
        dataSet copy1(d);
        
        //while copy1 has any of class c in it
        cout<<"copy1 has "<<copy1.numClass(c)<< " of " << c<<endl;
        while(copy1.numClass(c) > 1){ 
            
            //make a rule to fill up over time
            rule r;
            dataSet copy2(copy1);
            //while copy 2 still has multiple classes (run until its only class c left)
           
            while(copy2.classList.size() > 0){
                //cout << "Loop2|";
                map<string, vector<string>>::iterator it;
                classification cl(c);
                //cl.value = c;
                vector<string> * sVecPtr;
                attribute temp("temp","temp",sVecPtr);
                //ok so max needs to start at some basic value that is valid, so the first one we find must become max
                attributeValuePair max(temp);
                double maxProb = -1;
                bool maxAssigned = false; //this flag will change after we've done that
                
                for(it = d.attributeList.begin(); it != d.attributeList.end(); ++it) {//for every attribute
                    
                    string attributeName = it->first;
                    //cout <<attributeName<<"|";
                    for(string &a : it->second){//for every value of that attribute
                        string attributeValue = a;
                        attributeValuePair avp(attributeName, attributeValue, &d.attributeList.at(attributeName));
                        //cout << avp.toString() << endl;
                        double avpProb = probability(c,avp,d);
                        //cout << " |prob:"<<avpProb;
                        if(!maxAssigned){//if we havent assigned max yet, just make this one max, no questions asked
                            max = avp;
                            maxProb = avpProb;
                            maxAssigned = true;
                        } else { //normal case
                            if (avpProb > maxProb){
                                cout << maxProb <<" < " << avpProb << endl;
                                max = avp;
                                maxProb = avpProb;
                                
                            } else if (avpProb = maxProb){
                                //TODO: resolve this conflict
                                //currently, do nothing, keep previous max
                            }
                            
                        }
                    }
                }
                //find pair with max probability (this is 'max')
                //copy2.remove instances w/out pair
                //add pair & class to r
                if(maxAssigned){
                    cout <<copy2.instances.size() << "\t";
                    copy2.removeElementsNotMatching(max);
                    cout << "removedElements\t"<< copy2.instances.size() << endl;
                    cout << copy2.toString(true) << "\n\n\n\n";
                    r.addAttribute(max);
                    r.addClass(c);
                } else {
                    break;
                }
            }
            cout << "end of inner while loop";//<--end of inner while loop
            //add r to ruleList
            bool pass = true;
            for(auto &cond : r.conditions){
                if(cond.name=="temp"){
                    cout <<"temp escaped"<<endl;
                    pass=false;
                }
            }
            if(pass){ruleList.push_back(r);}
            cout <<"just added rule:" <<r.toString() << endl;
            //copy1.remove all instances matching rule
            copy1.removeElementsMatching(r);
        };
    }

    /* 
    rule currentRule;
    attributeValuePair maxInfoGain;
    for( string &c : d.classList){
        map<string, vector<string>>::iterator it;
        for (it = d.attributeList.begin(); it != d.attributeList.end(); ++it) {//for every attribute
            string attributeName = it->first;
            for (string &a : it->second){//for every value of that attribute
                string attributeValue = a;
                attributeValuePair avp(attributeName, attributeValue, &d.attributeList.at(attributeName));
            }
        }
    }
    */
}

// void buildClassifier(dataSet &d){
    
// }

int main(int argc, char *argv[]){
    //cout << "Enter filename: ";
    string fileName;
    //cin >> fileName;
    //fileName = "soybean.arff";//im tired of typing it in
    fileName ="weather.nominal.arff";
    cout << endl;

    dataSet data1;
    fileInput(fileName, data1, true);
    cout << data1.toString(true) << endl;
    cout << "size of dataSet: " << sizeof(data1) << " bytes"<< endl << endl;

    vector<rule> rules;
    buildRules(data1, rules);
    // attribute a("outlook","sunny",&data1.attributeList.at("outlook"));
    // attributeValuePair avp(a);
    // string c = "yes";
    // cout <<"Prob:"<<probability(c,avp, data1)<<endl;
    // for(rule &r : rules){
    //     cout << r.toString() << endl;
    // }


}