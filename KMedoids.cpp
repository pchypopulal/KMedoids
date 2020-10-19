#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <math.h>
#include <cmath>
#include <algorithm>
using namespace std;


const string medoids_ID_number = "medoids_ID_number";//tags for medoids ID number
const string nearest_point_ID_number = "nearest_point_ID_number";//tags for nearest point ID number


class packet{
public:
    string flow_identify_feature;
    int arrival_time;
    int packet_length;
};

class cluster{
public:
    int modiod_Idx;
    vector<int> nearest_point;
    void add_point(int point_ID)
    {
        nearest_point.push_back(point_ID);
    }
    void delete_points()
    {
        nearest_point.clear();
    }
    void print_points()
    {
    
        for (int i=0; i<nearest_point.size(); i++) {
            cout<<nearest_point[i]<<" ";
        }
        cout<<endl;
    }

    //check whether a point is in an cluster
    bool isBelong (int point_ID)
    {
       
        for (int i=0; i<nearest_point.size(); i++) {
            if(point_ID==nearest_point[i])
            {
                return true;
            }
        }
        return false;
    }

    //check whether the point is a medoid for this cluster
    bool isMedoid(int key)
	{
    
    	if(key==modiod_Idx)
    	{
        	return true;
    	}
    	return false;
	}
};

//calculate distance between 2 node
double Distance(vector<vector<double>> flow_final_data, int p_1, int p_2)
{
    return abs(flow_final_data[p_1][0]-flow_final_data[p_2][0])+abs(flow_final_data[p_1][1]-flow_final_data[p_2][1]);
}

int find_nearest_Modoid(int point_ID,vector<vector<double>> dist_matrix, vector<int> medID)
{
    int nearest_Modoid;
    double min_dis=9999999.0;
    double dis;
    for(int k=0;k<medID.size();k++)
    {
        dis=dist_matrix[point_ID][medID[k]];
        if (dis<min_dis)
        {
            min_dis=dis;
            nearest_Modoid=medID[k];
        }
    }
    return nearest_Modoid;
}

bool isMedoid(vector<int> medoid_idx, int key)
{
    for (int i=0; i<medoid_idx.size(); i++) {
        if(key==medoid_idx[i])
        {
            return true;
        }
    }
    return false;
}

//generate distance matrix
vector<vector<double>> distance_calculator(vector<vector<double>> flow_final_data, int len)
{
	
    vector<vector<double>> dist_every_point_to_point;
    for (int x=0; x<len; x++) {
        vector<double> temp_destant;
        for (int y=0; y<len; y++) {
            temp_destant.push_back(Distance(flow_final_data, x, y));
        }
        dist_every_point_to_point.push_back(temp_destant);
    }
    return dist_every_point_to_point;
}

// k medoids clustering algrithm function
void assign_points(vector<vector<double>>& flow_final_data, map<string, vector<int>>& current_medoids, double& total_cost, int medoid_num, int len, double& sse)
{
	vector<int> medID;//store ID for K medoids
    for (int i = 0; i < medoid_num; i++)
    {
        medID.push_back(current_medoids[medoids_ID_number][i]); // medoids index
    }

    // arrange data into cluster
    vector<vector<double>> dist_matrix;
    dist_matrix = distance_calculator(flow_final_data, len); // minimum distance for each row of data
    //store the nearest medoids for evert point
    vector<int> nearest_Modoid_ID_list;
    for(int i = 0; i < len; i++)
    {
    	int nearest_Modoid_ID;
    	nearest_Modoid_ID = find_nearest_Modoid(i,dist_matrix,medID);
    	nearest_Modoid_ID_list.push_back(nearest_Modoid_ID);
    }
    current_medoids[nearest_point_ID_number] = nearest_Modoid_ID_list;

    // calculate the total cost of all clusters
    vector<double> min_dist_for_every_point;
    for (int i = 0; i < len; i++)
    {
        double dist;
        dist = Distance(flow_final_data, i, nearest_Modoid_ID_list[i]);
        min_dist_for_every_point.push_back(dist);
    }

    for (int i = 0; i < len; i++)
    {
        total_cost += min_dist_for_every_point[i];
        sse += pow(min_dist_for_every_point[i],2);
    }
    sse = sse / len;
    
}

//search in the flow
vector<int> find_flow(string key,vector<packet> packets)
{
    vector<int> target_flow_ID;
    for (int i=0; i<packets.size(); i++) {
        if(packets[i].flow_identify_feature==key)
        {
            target_flow_ID.push_back(i);
        }
    }
    return target_flow_ID;
}

//delete flow
void delete_flow(vector<int> target_flow_ID, vector<packet>& packets)
{
    vector <packet>::iterator Iter;
    //remove from end to first
    for (int i=target_flow_ID.size()-1; i>=0; i--) {
        Iter = packets.begin()+target_flow_ID[i];
        packets.erase(Iter);
        Iter = packets.begin();
    }
}


//copy map data from other map data
void map_copy(map<string, vector<int>>& m_from, map<string, vector<int>>& m_to, vector<string> vector_tags)
{
    for (string key : vector_tags)
    {
        m_to[key] = m_from[key];
    }
}

//transfer vector to set
set<int> vector_to_set(vector<int> v)
{
    set<int> s;
    for (int i : v)
    {
        s.insert(i);
    }
    return s;
}



double k_medoids_cluster(map<string, vector<int>>& current_medoids, vector<vector<double>>& flow_final_data, int len, int medoid_num, vector<int> med_idx)
{
	double sse = 0.0;
	map<string, vector<int>> record_medoids;
    vector<int> record_medoids_ID;
    vector<string> vector_tags;
    set<int> current_medodios_set;
    set<int> record_medoids_set;
    double total_cost = 0.;

    current_medoids[medoids_ID_number] = med_idx;
    assign_points(flow_final_data, current_medoids, total_cost, medoid_num, len, sse);

    record_medoids[medoids_ID_number] = record_medoids_ID;
    current_medodios_set = vector_to_set(current_medoids[medoids_ID_number]);
    record_medoids_set = vector_to_set(record_medoids[medoids_ID_number]);

    map<string, vector<int>>::iterator iter;
    for (iter = begin(current_medoids); iter != end(current_medoids); iter++)
    {
        vector_tags.push_back(iter->first);
    }

    while (current_medodios_set != record_medoids_set)//if medoids has changed
    {
        map<string, vector<int>> best_medoids;
        map_copy(current_medoids, best_medoids, vector_tags);//from current_medoids to best_medoids
        map_copy(current_medoids, record_medoids, vector_tags);
        map<string, vector<int>> temp_medoids;
        map_copy(current_medoids, temp_medoids, vector_tags);
        for (int i = 0; i < len; i++)
        {
            for (int j = 0; j < medoid_num; j++)
            {
                if (!isMedoid(best_medoids[medoids_ID_number],i))//if i is not current medoids
                {
                    double temp_total_cost = 0.;
                    double temp_sse =0.;
                    
                    map_copy(best_medoids, temp_medoids, vector_tags);//from current_medoids to temp_medoids
                    temp_medoids[medoids_ID_number][j] = i;//set I as Medoids
                    
                    assign_points(flow_final_data, temp_medoids, temp_total_cost, medoid_num, len, temp_sse);//calculate temp_cost

                    //cout << "i: " << i <<", j: "<< j << ", total_cost: " << total_cost << ", temp_total_cost :" <<temp_total_cost << ", sse :" << sse << ", temp_sse :"<< temp_sse<<endl;

                    if (total_cost - temp_total_cost >0 )//if swap can get lower cost then swap
                    {
                        total_cost = temp_total_cost;
                        sse =temp_sse;
                        map_copy(temp_medoids, best_medoids, vector_tags);
                        i=0;
        				j=0;
        				    				
        				
                    }                    
                }
            }
        }
        
        map_copy(best_medoids, current_medoids, vector_tags);//from best_medoids to current_medoids
        current_medodios_set = vector_to_set(current_medoids[medoids_ID_number]);
        record_medoids_set = vector_to_set(record_medoids[medoids_ID_number]);
    }
    return total_cost;
}

int main(int argc, const char * argv[]) {
    //string file_one="file1.txt";
    string file_one=argv[1];
    string file_two=argv[2];
    
    
    vector<packet> packets;
    vector<vector<double>> flow_final_data;
    vector<int> flow_idx;
    
    ifstream infile;
    string data;
    infile.open(file_one,ios::in);
    getline(infile,data);
    while (getline(infile, data)) {
        string tmp;
        vector<string> factors;//collect data for a line (generate a)
        stringstream ss(data);
        while (ss>>tmp) {
            factors.push_back(tmp);
        }
        packet pac;
        pac.flow_identify_feature=factors[0]+factors[1]+factors[2]+factors[3]+factors[4];
        pac.arrival_time=std::stoi(factors[5]);
        pac.packet_length=std::stoi(factors[6]);
        packets.push_back(pac);
    }
    infile.close();
    
    while (packets.size()!=0) {
        
        flow_idx=find_flow(packets[0].flow_identify_feature,packets);
        if (flow_idx.size()<=1) {//delete flow with only one packet
            delete_flow(flow_idx, packets);
        }
        else
        {
            double time=0;
            double length=0;
            vector<double> flow_result;//used for output
            for (int j=flow_idx.size()-1; j>0; j--) {
                length=length+packets[flow_idx[j]].packet_length;
                time=time+packets[flow_idx[j]].arrival_time-packets[flow_idx[j-1]].arrival_time;
            }
            length=length+packets[0].packet_length;//because j > 0 cannot reach to j = 0, so add this one
            time=time/(flow_idx.size()-1);
            length=length/flow_idx.size();
            flow_result.push_back(time);
            flow_result.push_back(length);
            flow_final_data.push_back(flow_result);
            delete_flow(flow_idx, packets);
            
        }
    }

    //output to Flow.txt
    ofstream fout;
    fout.open ("Flow.txt", ios::out | ios::trunc);
    fout.flags(ios::fixed);
    for(int k=0;k<flow_final_data.size();k++)
    {
        fout<<k<<" ";
        fout<<setprecision(2)<<flow_final_data[k][0];
        fout<<" ";
        fout<<setprecision(2)<<flow_final_data[k][1]<<endl;
    }
    
    fout.close();

    int len = flow_final_data.size();

    //input file2.txt
    //string file_two="file2.txt";

    int medoid_num;
    vector<int> med_idx;
    
    ifstream infile1;
    
    
    infile1.open(file_two,ios::in);
    getline(infile1,data);
    medoid_num=stoi(data);//number of medoids
    while (getline(infile1, data)) {
        int num;
        stringstream ss(data);
        while (ss>>num) {
        	med_idx.push_back(num);
        	}
    }
    infile1.close();

    //k_Medoids algorithm
    map<string, vector<int>> current_medoids;
    double total_cost = k_medoids_cluster(current_medoids, flow_final_data, len, medoid_num, med_idx);

    ofstream fout1;

    fout1.open ("KMedoidsClusters.txt",ios::out | ios::trunc);
    fout1.flags(ios::fixed);
    
    fout1 <<setprecision(2)<<total_cost << endl;

    vector<cluster> clusters;
    for(int i=0; i< medoid_num; i++)
    {
        cluster clust;
        clusters.push_back(clust);
    }

    for (int l=0; l<clusters.size(); l++) {
        clusters[l].delete_points();
        clusters[l].modiod_Idx=current_medoids[medoids_ID_number][l];
        for (int k = 0; k < current_medoids[nearest_point_ID_number].size() ; k++)
        {
            if (current_medoids[nearest_point_ID_number][k] == clusters[l].modiod_Idx)
            {
                clusters[l].add_point(k);
            }
        }
        //cout << "cluster :" << l << ", med :"<< clusters[l].modiod_Idx << endl;
        //clusters[l].print_points();
    }
    

    for (int i : current_medoids[medoids_ID_number])
    {
        fout1 << i << " ";
    }

    fout1 << endl;
    for (int i=0; i<clusters.size(); i++) 
    {
        for (int j=0; j<clusters[i].nearest_point.size(); j++) {
            fout1<<setprecision(2)<<clusters[i].nearest_point[j]<<" ";
        }
        fout1<<endl;
    }
    fout1.close();
    
    return 0;
}
