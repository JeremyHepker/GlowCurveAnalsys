//
//  File_Manager.cpp
//  GlowCurveAnalsys
//
//  Created by jeremy hepker on 1/27/19.
//  Copyright © 2019 Jeremy Hepker. All rights reserved.
//

#include "File_Manager.hpp"
#include "CSV_iterator.cpp"

using namespace std;
File_Manager::File_Manager(std::string given_filename):filename(given_filename){};

//This function reads in the .csv file and parses the data into vector of coordinate pairs.
pair<vector<double>,vector<double>> File_Manager::read(){
    //Open and test the user input file.
    string line, catagories;
    size_t path = filename.find_last_of("/\\");
    string temp_path = filename.substr(0,path+1);
    temp_path += "temp.csv";
    ifstream file(filename);
    if(!file.is_open()){
        cerr<<"Error opening file: "<<filename<<endl;
        exit(1);
    }
    while(true){
        if(line.find("Time (") != std::string::npos) time =true;
        if(line.find("Count")==std::string::npos){
            getline(file, line,'\n');
        }else{
            getline(file, line,'\n');
            break;
        }
    }
    ofstream temp_file;
    temp_file.open(temp_path);
    if(!temp_file.is_open()){
        cerr<<"Error opening file: "<<filename<<endl;
        exit(1);
    }
    vector<string> temps;
    while(getline(file, line,'\n')){
        if(line.find('\r') != std::string::npos) line.pop_back();
        if(line.back() != ',') temps.push_back(line + ",\n");
        else temps.push_back(line+ "\n");
    }
    temps.back().pop_back();
    temps.back().pop_back();
    for(auto j = temps.begin(); j != temps.end(); ++j){
        temp_file<<*j;
    }
    temp_file.close();
    file.close();
    file.open(temp_path);
    getline(file, line);
    getline(file, line);
    stringstream ss;
    ss << line;
    int count = 0;
    while(getline(ss, line, ',')){
        count++;
        if(line.find('N') != std::string::npos){
            line.erase(0,1);
            barcodeNum = stoi(line);
        }
    }
    auto i = csv_iterator<string>( file );
    bool two = false;
    while(file){
        if(time){
            for(int j=0;j<(count-3);++j){
               ++i;
            }
            maxTemp = stod(*i);
            ++i;
        }else for(int j=0;j<(count-2);++j) ++i;
        raw_temp_data.push_back(stod(*i));
        ++i;
        raw_count_data.push_back(stod(*i));
        if(!two && raw_count_data.back() >1)two = true;
        ++i;
        if(file.eof()) break;
        if(!two){
            raw_temp_data.pop_back();
            raw_count_data.pop_back();
        }
    }
    file.close();
    if((raw_temp_data.size()%2) == 0){
        raw_temp_data.pop_back();
        raw_count_data.pop_back();
    }
    return make_pair(raw_temp_data, raw_count_data);
}

//This is a function to write the output to a new CSV file.
void File_Manager::write(vector<vector<double>> glow_curves, string output_name, int count){
    ofstream file;
    output_name += "_output.csv";
    file.open(output_name);
    if(!file.is_open()){
        cerr<<"Could not open output file : "<<output_name<<endl;
        exit(1);
    }
    file<<"temp,count_original";
    for(int j = 0; j<glow_curves.size();++j){
        string ster = "count_" + to_string(j);
        file<<","<<ster;
    }
    file<<",\n";
    file.setf(ios_base::fixed);
    file<<setprecision(5);
    for(int i = 0; i<raw_temp_data.size();++i){
        file << raw_temp_data[i]<<",";
        file << raw_count_data[i];
        for(int j = 0; j<glow_curves.size();++j){
            file<<","<<double(glow_curves[j][i]);
        }
        file<<",\n";
    }
    cout<<"Output File : "<<output_name<<endl;
    file.close();
}

double File_Manager::temp_rate(string name){
    if(isdigit(name[0])){
        int j = 0;
        while(isdigit(name[j])){
            ++j;
            if(name[j] == '.') ++j;
        }
        string temp = name.substr(0,j);
        return(stod(temp));
    }
    auto max = max_element(raw_temp_data.begin(), raw_temp_data.end());
    int pos = int(max - raw_temp_data.begin());
    return (raw_temp_data[pos]-raw_temp_data[8])/(int(max - raw_temp_data.begin())/2);
}

File_Manager::~File_Manager()
{
    raw_temp_data.clear();
    raw_count_data.clear();
}
