#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>
#include <cstring>
#include <algorithm>
#include <stdio.h>

using namespace std;

//定义得分权重
const double weight[5] = {0.05, 0.1, 0.1, 0.4, 0.35 };

typedef enum 
{
  DATE,
  FROM,
  TO,
  SUBJECT,
  BODY
} Domain_name;  //域名枚举变量

typedef struct data
{
	unsigned int id;
	unsigned int count;

	bool operator < (const data& other) const
    {
        if (id != other.id) // 类型按升序排序
        {
            return (id < other.id);
        }
        else 
        {
            return (count < other.count);
        }
    }

} element; //倒排记录结构体:(文档id,词项频率)

ifstream& open_file(ifstream&, const string&);
ofstream& write_file(ofstream&, const string&, const map<string,set<element> > &);
ofstream& write_doc_map(ofstream&, const string&, vector<string> &, vector<string> &,vector<string> &);
ifstream& read_file(ifstream&, const string&, map<string,set<element> > &);
ifstream& read_doc_map(ifstream&, const string&, vector<string> &, vector<string> &,vector<string> &);
void delete_separators(string &,const string &);
void print_map(const map<string,unsigned int>&);
void print_postinglist(const map<string,set<element> >&);
ofstream& write_sort_map(ofstream& out, const string &file, const map<string,unsigned int>&);
void and_sort_query(ofstream& out, const string &file, const map<string,set<element> > *, vector<string> &, vector<string> &,vector<string> &);
void and_query(ofstream& out, const string &file, const map<string,set<element> > *, vector<string> &, vector<string> &,vector<string> &);
void or_query(ofstream& out, const string &file,const map<string,set<element> > *, vector<string> &, vector<string> &,vector<string> &);
void not_query(ofstream& out, const string &file,const map<string,set<element> > *, vector<string> &, vector<string> &,vector<string> &);

int main()
{
	unsigned int doc_ID = 0; //文档ID
	int i;
    //域标识符
	const string date = "Date";
	const string from = "From";
	const string to = "To";
	const string subject = "Subject";
	const string X_FileName = "X FileName";
	//停用词
	const string stop_word = "com";
    //控制标记
	int first_date;
	int first_from;
	int first_to;
	int first_subject;
	int read_flag_to;
	int read_flag_body;
	int new_postinglist;
    //域内容
	string content_date;
    string content_from;
	string content_to;
	string content_subject;
	string content_body;
	//用于统计单个文档词项的频率
	map<string, unsigned int> count[5];
	element unit;
    //用于统计数据集词项的频率
    map<string, unsigned int> from_map;
	map<string, unsigned int> to_map;
	map<string, unsigned int> subject_map;
	map<string, unsigned int> body_map;
    //词项-倒排记录对
	map<string, set<element> > postinglist[5];
	//文档ID 文档Message-ID 和 文档路径对
	vector<string> doc_ID_log;
	vector<string> message_log;
	vector<string> directory_log;
    //输入输出流
	ifstream infile;
	ifstream dataset_infile;
	ofstream outfile;
    //文档内容相关变量
	int lineno;
    string text_line;
    string new_text_line;
	string word;
	vector<string> line_content;
	string temp = "";
    istringstream line;
	//输入文档ID和路径
	char buffer[10];
	string id;
	string doc_file;
	//输出文档名称
    //遍历后文档路径存放文件
	const string doc_dataset = "directory.txt";
	//文档ID 文档Message-ID 和 文档路径对存放文件
	const string doc_map = "doc_map.txt";
	//倒排记录表文档
	const string doc_date = "D:\\Retrieval\\postinglist\\doc_date.txt";
	const string doc_from = "D:\\Retrieval\\postinglist\\doc_from.txt";
	const string doc_to = "D:\\Retrieval\\postinglist\\doc_to.txt";
	const string doc_subject = "D:\\Retrieval\\postinglist\\doc_subject.txt";
	const string doc_body = "D:\\Retrieval\\postinglist\\doc_body.txt";
	//词项频率文档
	const string doc_sort_from = "D:\\Retrieval\\frequency\\from_frequency.txt";
	const string doc_sort_to = "D:\\Retrieval\\frequency\\to_frequency.txt";
	const string doc_sort_subject = "D:\\Retrieval\\frequency\\subject_frequency.txt";
	const string doc_sort_body = "D:\\Retrieval\\frequency\\body_frequency.txt";
	//查询结果文档
	const string query_and_result = "D:\\Retrieval\\result\\query_and_result.txt";
	const string query_or_result = "D:\\Retrieval\\result\\query_or_result.txt";
	const string query_not_result = "D:\\Retrieval\\result\\query_not_result.txt";
	const string query_and_sort_result = "D:\\Retrieval\\result\\query_and_sort_result.txt";

	//设置是否重新建立倒排表
	//cout<<"Do you want to create new postinglists? (1:Yes;0:No)?"<<endl;
	new_postinglist = 0;
	//new_postinglist = 0;
 
	if(new_postinglist)
	{
		cout<<"The program is creating the postinglists..."<<endl;
      	//打开文档路径存放文件
		if(!open_file(dataset_infile,doc_dataset))
	    {
			cout<<"can not open file!"<<endl;
			return 0;
		}

		//遍历读取文档
		while(getline(dataset_infile,doc_file))
	    {
			doc_ID++;
            cout<<doc_file<<endl;
			//打开文档
			if(!open_file(infile,doc_file))
	        {
				cout<<"can not open file!"<<endl;
			    return 0;
	        }
			//逐行读取文档内容
            lineno = 0;
			while(getline(infile,text_line))
	        {
				lineno++;
				if(lineno==1)
				{
		            sprintf(buffer,"%d",doc_ID);
					id.assign(buffer);
					doc_ID_log.push_back(id);//文档ID
					message_log.push_back(text_line);//Message-ID
					directory_log.push_back(doc_file);//文档路径
				}
				new_text_line = "";
				//去除分隔符
		        delete_separators(new_text_line,text_line);
			    line_content.push_back(new_text_line);
		    }

			//控制标记复位
		    first_date = 0;
		    first_from = 0;  
            first_to = 0;
		    first_subject = 0;
	        read_flag_to = 0;
            read_flag_body = 0;
        
	 	    //清空域内容
            content_date = "";
	        content_from = "";
	        content_to = "";
	        content_subject = "";
	        content_body = "";
		
			//逐行分析文档内容
		    for(vector<string>::iterator iter = line_content.begin(); iter!=line_content.end(); ++iter)
	        {
				text_line = *iter;
		        //cout<<text_line<<endl;
				
				//读取date域内容至content_date
			    if(temp.assign(text_line,0,4)==date)
		        {
				    if(first_date==0)
						content_date += text_line;
                    first_date++; 
			    }
			   
			    //读取from域内容至content_from
			    if(temp.assign(text_line,0,4)==from)
		        {
					if(first_from==0)
						content_from += text_line;
                    first_from++; 
			    }
			   
  			    //读取to域内容至content_to
		        if(temp.assign(text_line,0,2)==to)
		        {
					read_flag_to = 1;
		        }
			   
			    if(temp.assign(text_line,0,7)==subject)
    		    {
					read_flag_to = 0;
				    first_to++;
		        }
			   
			    if((read_flag_to==1) && (first_to==0))
			    {
					content_to += text_line;
		        }
			   
			    //读取subject域内容至content_subject
		        if(temp.assign(text_line,0,7)==subject)
			    {
					if(first_subject==0)
						content_subject += text_line;
			  	    first_subject++; 
		   	    }
			   
			    //读取body内容至content_body
			    if(temp.assign(text_line,0,10)==X_FileName)
		        {
					read_flag_body = 1;
					continue;
			    }
		    
			    if(read_flag_body==1)
				{
				    content_body += text_line;	
			    }
		    }
			
			//清空line_content
		    line_content.clear();

		    //擦除标识符,body域不必擦除
		    content_date.erase(0,4);
		    content_from.erase(0,4);
            content_to.erase(0,2);
	        content_subject.erase(0,7);

		    //cout<<doc_ID<<" subject is : "<<content_to<<endl;

			//全部转换为小写
			transform(content_date.begin(),content_date.end(),content_date.begin(),tolower);
			transform(content_from.begin(),content_from.end(),content_from.begin(),tolower);
			transform(content_to.begin(),content_to.end(),content_to.begin(),tolower);
			transform(content_subject.begin(),content_subject.end(),content_subject.begin(),tolower);
			transform(content_body.begin(),content_body.end(),content_body.begin(),tolower);

		    //对date域统计词项的频率
	    	line.clear();
	        line.str(content_date);
		
		    while(line>>word)
	        {
				++count[DATE][word];
		    }

		    //对from域统计词项的频率,"com"作为停用词
		    line.clear();
	        line.str(content_from);
			
			while(line>>word)
	        {
				if(word!=stop_word)
			    {
					++count[FROM][word];
					++from_map[word];
			    }
		    }
			
			//对to域统计词项的频率,"com"作为停用词
	        line.clear();
            line.str(content_to);

	        while(line>>word)
	        {
				if(word!=stop_word)
		    	{
					++count[TO][word];
					++to_map[word];
			    }
			}
		    
			//对subject域统计词项的频率
	        line.clear();
            line.str(content_subject);
			
			while(line>>word)
		    {
				++count[SUBJECT][word];
				++subject_map[word];
		    }

		    //对body域统计词项的频率
		    line.clear();
            line.str(content_body);

	        while(line>>word)
		    {
				++count[BODY][word];
				++body_map[word];
		    }

			//对五个域建立倒排表
			for(i=0;i<5;i++)
			{
				map<string,unsigned int>::const_iterator count_it = count[i].begin();
				for(;count_it!=count[i].end();count_it++)
				{
					unit.id = doc_ID;
					unit.count = count_it->second;
					postinglist[i][count_it->first].insert(unit);
				}
				count[i].clear();
			}

            
		    //cout<<"doc_ID is:"<<doc_ID<<endl;
            //print_map(subject_map);
		    //getchar();
		    
		    //cout<<"doc_ID is:"<<doc_ID<<endl;
            //print_postinglist(postinglist[3]);
		    //getchar();
       }//输入文档遍历结束,while(getline(dataset_infile,doc_file))
	   
	   //文档ID 文档Message-ID 和 文档路径对写入磁盘
	   write_doc_map(outfile,doc_map,doc_ID_log,message_log,directory_log);
	   //将倒排记录表写入磁盘
       write_file(outfile,doc_date,postinglist[DATE]);
       write_file(outfile,doc_from,postinglist[FROM]);
       write_file(outfile,doc_to,postinglist[TO]);
       write_file(outfile,doc_subject,postinglist[SUBJECT]);
       write_file(outfile,doc_body,postinglist[BODY]);
       //将词项频率表写入磁盘
       write_sort_map(outfile,doc_sort_from,from_map);
       write_sort_map(outfile,doc_sort_to,to_map);
       write_sort_map(outfile,doc_sort_subject,subject_map);
       write_sort_map(outfile,doc_sort_body,body_map);
	   cout<<"The postinglists have been created."<<endl;
   }
   else
   {
	   cout<<"The program is reading the postinglists..."<<endl;
	   read_doc_map(infile,doc_map,doc_ID_log,message_log,directory_log);
	   read_file(infile,doc_date,postinglist[DATE]);
	   read_file(infile,doc_from,postinglist[FROM]);
	   read_file(infile,doc_to,postinglist[TO]);
	   read_file(infile,doc_subject,postinglist[SUBJECT]);
	   read_file(infile,doc_body,postinglist[BODY]);
	   cout<<"The postinglists have been read."<<endl;

   }
   //查询
   and_query(outfile,query_and_result,postinglist,doc_ID_log,message_log,directory_log);
   or_query(outfile,query_or_result,postinglist,doc_ID_log,message_log,directory_log);
   not_query(outfile,query_not_result,postinglist,doc_ID_log,message_log,directory_log);
   and_sort_query(outfile,query_and_sort_result,postinglist,doc_ID_log,message_log,directory_log);
   
   return 1;
}

ifstream& open_file(ifstream &in, const string &file)
{
	in.close();
	in.clear();
	in.open(file.c_str());
	return in;
}

void delete_separators(string& new_line,const string& line)
{
	int wordLen;
	string separators("\t\v\r\n\f\"\'\\-@<>,;:.()_/[]=*?!%#&~^$+-");//分隔符号
	string::size_type startPos, endPos;
	string word;

	startPos = 0;
    endPos = 0;
	//去除分隔字符
	while((startPos = line.find_first_not_of(separators, endPos))!= string::npos)
	{
		endPos = line.find_first_of(separators, startPos);
		if(endPos == string::npos)
		{
			wordLen = line.size() - startPos;
		}
		else
		{
			wordLen = endPos - startPos;
		}
		word.assign(line.begin() + startPos, line.begin() + startPos + wordLen);
		word += " ";
        new_line += word;
		startPos = line.find_first_not_of(separators, endPos);
	}
}

void print_map(const map<string,unsigned int> &domain_map)
{
	map<string,unsigned int>::const_iterator map_it = domain_map.begin();
	for(; map_it!=domain_map.end(); ++map_it)
	{
		cout<<map_it->first<<" "<<map_it->second<<endl;
	}
	getchar();
}

void print_postinglist(const map<string,set<element> > &postinglist)
{
	for(map<string, set<element> >::const_iterator it=postinglist.begin();it!=postinglist.end();++it)
	{ 
		cout<<it->first<<" ";
		for(set<element>::const_iterator set_it=it->second.begin();set_it!=it->second.end();++set_it)
		{
			cout<<set_it->id<<" "<<set_it->count<<" ";
		}
		cout<<endl;
	}
}

ifstream& read_doc_map(ifstream &in, const string &file, vector<string> &doc_ID_log,vector<string> &message_log,vector<string> &directory_log)
{
	in.close();
	in.clear();
	in.open(file.c_str());

	string line;
	unsigned count;

	count = 0;
	if(in)
	{
		while(getline(in,line))
	    {
			count++;
		    if(count%3==1)
				doc_ID_log.push_back(line);
		    if(count%3==2)
				message_log.push_back(line);
		    if(count%3==0)
				directory_log.push_back(line);
	    }
	}
	return in;
}

ifstream& read_file(ifstream& in, const string &file,map<string,set<element> > &postinglist)
{
    in.close();
	in.clear();
	in.open(file.c_str());
    
    string postinglist_line;
    istringstream line;
	string word;
	string item;
	element unit;
    unsigned count;
	unsigned i;
	unsigned j;
	char buf[10];

	if(in)
	{
		while(getline(in,postinglist_line))
		{
		   count = 0;
		   line.clear();
	       line.str(postinglist_line);
		   while(line>>word)
	       {
			   count++;
			   if(count==1)//读取词项
			   {
				   item = word;
				   continue;
			   }
			   if(count%2==0)//读取文档id
			   {
				   strcpy(buf,word.c_str());
                   sscanf(buf,"%d",&i);
				   unit.id = i;
			   }
			   if(count%2==1)//读取词项频率
			   {
				   strcpy(buf,word.c_str());
                   sscanf(buf,"%d",&j);
				   unit.count = j;
				   //读取(文档id,词项频率)对后加入倒排索引
				   postinglist[item].insert(unit);
			   }
	       }
		}
	}
	return in;
}


ofstream& write_file(ofstream& out, const string &file, const map<string,set<element> > &postinglist)
{
	out.close();
	out.clear();
	out.open(file.c_str());

	if(out)
	{
		for(map<string, set<element> >::const_iterator it=postinglist.begin();it!=postinglist.end();++it)
		{
			out<<it->first<<" ";
			for(set<element>::const_iterator set_it=it->second.begin();set_it!=it->second.end();++set_it)
			{
				out<<set_it->id<<" "<<set_it->count<<" ";
			}
			out<<endl;
		}
	}

	return out;
}


ofstream& write_doc_map(ofstream &out, const string &file, vector<string> &doc_ID_log,vector<string> &message_log,vector<string> &directory_log)
{
	out.close();
	out.clear();
	out.open(file.c_str());

	string line;
	vector<string>::size_type i;

	if(out)
	{
		for(i=0;i<doc_ID_log.size();i++)
		{
			out<<doc_ID_log[i]<<endl;
			out<<message_log[i]<<endl;
			out<<directory_log[i]<<endl;
		}
	}
	return out;
}

ofstream& write_sort_map(ofstream& out, const string &file, const map<string,unsigned int> &domain_map)
{
	out.close();
	out.clear();
	out.open(file.c_str());

	vector<string> key;
	vector<unsigned int> value;
	unsigned max;
	int i;

	map<string,unsigned int>::const_iterator map_it = domain_map.begin();
	for(; map_it!=domain_map.end(); ++map_it)
	{
		key.push_back(map_it->first);
		value.push_back(map_it->second);
	}

	for(i=0;i<50;i++)
	{
		//循环50次，找到频率靠前的50个
		max = 0;
		vector<string>::const_iterator it_key = key.begin();
		vector<unsigned>::const_iterator it_value = value.begin();

		vector<string>::const_iterator it1 = key.begin();
		vector<unsigned>::const_iterator it2 = value.begin();

		for(;it1!=key.end()&&it2!=value.end();it1++,it2++)
		{
			if(*it2>=max)
			{
				it_key = it1;
				it_value = it2;
				max = *it2;
			}
		}
		if(out)
		{
			out<<"No."<<i+1<<" Item "<<*it_key<<" occurs "<<*it_value<<" time(s)."<<endl;
	    }
		key.erase(it_key);
		value.erase(it_value);
	}
	
/*
	if(out)
	{
		for(i=0 ;i<max; ++i)
	    {
			out<<"No."<<i+1<<" Item "<<key[i]<<" occurs "<<value[i]<<" time(s)."<<endl;
	    }
	}
*/
	return out;
}

void and_sort_query(ofstream& out, const string &file,const map<string,set<element> > *postinglist, vector<string> &doc_ID_log,vector<string> &message_log,vector<string> &directory_log)
{
	out.close();
	out.clear();
	out.open(file.c_str());

	vector<unsigned int>::size_type i;
	vector<unsigned int>::size_type j;
	string query;
	string new_query;
	string word;
	set<unsigned int> all_answer;
	map<unsigned int,unsigned int> score1[5];
	map<unsigned int,unsigned int> score2[5];
	set<unsigned int> doc[5];
	vector<double> total_score;
    vector<unsigned int> total_doc;
	double sum;
	int doc_temp;
	double score_temp;

	string item[2];

	map<string, set<element> >::const_iterator it1;
	map<string, set<element> >::const_iterator it2;
	set<element>::const_iterator p1;
	set<element>::const_iterator p2;

	cout<<"Please input a query, such as cash and Nove:\n";

	cin.clear();
	getline(cin,query);
	delete_separators(new_query,query);

	istringstream stream(new_query);
    
    stream>>word;
	item[0] = word;
	stream>>word;
	stream>>word;
	item[1] = word;

	//对每个域进行布尔“与”运算
	for(i=0;i<5;i++)
	{
		it1 = postinglist[i].find(item[0]);
	    it2 = postinglist[i].find(item[1]);
		if(it1!=postinglist[i].end() && it2!=postinglist[i].end())
		{
			p1 = it1->second.begin();
		    p2 = it2->second.begin();
			while(p1!=it1->second.end() && p2!=it2->second.end())
		    {
				if (p1->id == p2->id)
			    {
					doc[i].insert(p1->id);
					score1[i][p1->id] = p1->count;
					score2[i][p2->id] = p2->count;
					p1++;
				    p2++;
		        }
				else if(p1->id < p2->id)
					p1++;
				else p2++;
			}
		}
	}
	//合并5个域的结果
	for(i=0;i<5;i++)
	{
		set<unsigned int>::const_iterator p3 = doc[i].begin();
		for(;p3!=doc[i].end();p3++)
			all_answer.insert(*p3);
	}

	//统计每个文档的得分
	set<unsigned int>::const_iterator p = all_answer.begin();
	for(;p!=all_answer.end();p++)
	{
		sum = 0;
		for(i=0;i<5;i++)
		{
			set<unsigned int>::const_iterator p4 = doc[i].find(*p);
			if(p4!=doc[i].end())
				sum += weight[i]*( score1[i][*p4] + score2[i][*p4] );
		}
		//cout<<"sum is "<<sum<<endl;
		//getchar();
		total_doc.push_back(*p);
		total_score.push_back(sum);
	}

	//按词项频率排序
	for(i=0; i<total_doc.size(); i++)
	{
		for(j = total_doc.size()-1; j>i;j--)
		{
			if(total_score[j]>=total_score[j-1])
			{
				score_temp = total_score[j];
				total_score[j] = total_score[j-1];
				total_score[j-1] = score_temp;

				doc_temp = total_doc[j];
				total_doc[j] = total_doc[j-1];
				total_doc[j-1] = doc_temp;
			}
		}
	}

	if(out)
	{
		out<<"The result of query \""<<query<<"\" is:"<<endl;
		if(!total_doc.empty())
		{
			for(vector<unsigned int>::const_iterator it = total_doc.begin(); it!=total_doc.end(); ++it)
			{
				out<<doc_ID_log[*it-1]<<endl;
				out<<message_log[*it-1]<<endl;
				out<<directory_log[*it-1]<<endl;
			}
		}
		else
		{
			out<<"none!";
		}
	}

    if(out)
	{
		if(!total_score.empty())
		{
			out<<"\nDoc_score:\n";
			for(vector<double>::const_iterator it = total_score.begin(); it!=total_score.end(); ++it)
				out<<*it<<" ";
		}
		else
		{
			out<<"none!";
		}
	}
    cout<<"The result is in "<<file<<endl;
}


void and_query(ofstream& out, const string &file,const map<string,set<element> > *postinglist, vector<string> &doc_ID_log,vector<string> &message_log,vector<string> &directory_log)
{
	out.close();
	out.clear();
	out.open(file.c_str());

	unsigned int i;
	string query;
	string new_query;
	string word;
	set<unsigned int> answer;
	string domain[2];
	string item[2];
	unsigned int index[2];
    
	map<string, set<element> >::const_iterator it1;
	map<string, set<element> >::const_iterator it2;
	set<element>::const_iterator p1;
	set<element>::const_iterator p2;

	cout<<"Please input a query, such as cash.body and Nov.date:\n";
    
	cin.clear();
	getline(cin,query);
	delete_separators(new_query,query);

	istringstream stream(new_query);
    
    stream>>word;
	item[0] = word;
	stream>>word;
	domain[0] = word;
	stream>>word;
	stream>>word;
	item[1] = word;
	stream>>word;
	domain[1] = word;
    
	for(i=0; i<2; ++i)
	{
		if(domain[i] == "date")
			index[i] = 0; 
		else if(domain[i] == "from") 
			index[i] = 1; 
		else if(domain[i] == "to") 
			index[i] = 2; 
		else if(domain[i] == "subject") 
			index[i] = 3; 
		else if(domain[i] == "body") 
			index[i] = 4; 
	}

	answer.clear();
	
	it1 = postinglist[index[0]].find(item[0]);
	it2 = postinglist[index[1]].find(item[1]);
  
	if(it1!=postinglist[index[0]].end() && it2!=postinglist[index[1]].end())
	{
		p1 = it1->second.begin();
		p2 = it2->second.begin();
	    while(p1!=it1->second.end() && p2!=it2->second.end())
		{
			if (p1->id == p2->id)
			{
				answer.insert(p1->id);
			    p1++;
				p2++;
		    }
			else if(p1->id < p2->id)
				p1++;
			else p2++;
		}
	}

	if(out)
	{
		out<<"The result of query \""<<query<<"\" is:"<<endl;
		if(!answer.empty())
		{
			for(set<unsigned int>::const_iterator it = answer.begin(); it!=answer.end(); ++it)
			{
				out<<doc_ID_log[*it-1]<<endl;
				out<<message_log[*it-1]<<endl;
				out<<directory_log[*it-1]<<endl;
			}
		}
		else
		{
			out<<"none!";
		}
	}
	cout<<"The result is in "<<file<<endl;
}


void or_query(ofstream& out, const string &file,const map<string,set<element> > *postinglist, vector<string> &doc_ID_log,vector<string> &message_log,vector<string> &directory_log)
{
	out.close();
	out.clear();
	out.open(file.c_str());

	unsigned int i;
	string query;
	string new_query;
	string word;
	set<unsigned int> answer;
	string domain[2];
	string item[2];
	unsigned int index[2];
    
	map<string, set<element> >::const_iterator it1;
	map<string, set<element> >::const_iterator it2;
	set<element>::const_iterator p1;
	set<element>::const_iterator p2;

	cout<<"Please input a query, such as cash.body or Nov.date:\n";

	cin.clear();
	getline(cin,query);
	delete_separators(new_query,query);

	istringstream stream(new_query);
    
    stream>>word;
	item[0] = word;
	stream>>word;
	domain[0] = word;
	stream>>word;
	stream>>word;
	item[1] = word;
	stream>>word;
	domain[1] = word;
    
	for(i=0; i<2; ++i)
	{
		if(domain[i] == "date")
			index[i] = 0; 
		else if(domain[i] == "from") 
			index[i] = 1; 
		else if(domain[i] == "to") 
			index[i] = 2; 
		else if(domain[i] == "subject") 
			index[i] = 3; 
		else if(domain[i] == "body") 
			index[i] = 4; 
	}
    
    answer.clear();

	it1 = postinglist[index[0]].find(item[0]);
	it2 = postinglist[index[1]].find(item[1]);

	if(it1!=postinglist[index[0]].end() && it2==postinglist[index[1]].end())
	{
		p1 = it1->second.begin();
		while(p1!=it1->second.end())
		{
			answer.insert(p1->id);
			p1++;
		}
	}

	if(it1==postinglist[index[0]].end() && it2!=postinglist[index[1]].end())
	{
		p2 = it2->second.begin();
		while(p2!=it2->second.end())
		{
			answer.insert(p2->id);
			p2++;
		}
	}

	if(it1!=postinglist[index[0]].end() && it2!=postinglist[index[1]].end())
	{
		p1 = it1->second.begin();
		p2 = it2->second.begin();

		while(p1!=it1->second.end() && p2!=it2->second.end())
	    {
			if (p1->id == p2->id)
			{
				answer.insert(p1->id);
				p1++;
				p2++;
			}
			else if(p1->id < p2->id)
			{
				answer.insert(p1->id);
				p1++;
			}
			else 
			{
				answer.insert(p2->id);
				p2++;
			}
		}
		
		while(p1!=it1->second.end())
		{
			answer.insert(p1->id);
			p1++;
		}

		while(p2!=it2->second.end())
		{
			answer.insert(p2->id);
			p2++;
		}
	}
	

	if(out)
	{
		out<<"The result of query \""<<query<<"\" is:"<<endl;
		if(!answer.empty())
		{
			for(set<unsigned int>::const_iterator it = answer.begin(); it!=answer.end(); ++it)
			{
				out<<doc_ID_log[*it-1]<<endl;
				out<<message_log[*it-1]<<endl;
				out<<directory_log[*it-1]<<endl;
			}
		}
		else
		{
			out<<"none!";
		}
	}
	cout<<"The result is in "<<file<<endl;
}


void not_query(ofstream& out, const string &file,const map<string,set<element> > *postinglist, vector<string> &doc_ID_log,vector<string> &message_log,vector<string> &directory_log)
{
	out.close();
	out.clear();
	out.open(file.c_str());

	unsigned int i;
	string query;
	string new_query;
	string word;
	set<unsigned int> answer;
	string domain;
	string item;
	unsigned int index;
    
	map<string, set<element> >::const_iterator it;
	set<element>::const_iterator p1;
	set<unsigned int>::const_iterator p2;

	cout<<"Please input a query, such as not Nov.date:\n";

	cin.clear();
	getline(cin,query);
	delete_separators(new_query,query);

	istringstream stream(new_query);
    
	stream>>word;
    stream>>word;
	item = word;
	stream>>word;
	domain = word;
    
	if(domain == "date")
		index = 0; 
	else if(domain == "from") 
		index = 1; 
	else if(domain == "to") 
		index = 2; 
	else if(domain == "subject") 
		index = 3; 
	else if(domain == "body") 
		index = 4; 

	answer.clear();

	for(i=1; i<=doc_ID_log.size(); i++)
	{
		answer.insert(i);
	}
    
	it = postinglist[index].find(item);
	if(it!=postinglist[index].end())
	{
		p1 = it->second.begin();
		while(p1!=it->second.end())
		{
			p2 = answer.find(p1->id);
			if(p2 != answer.end())
				answer.erase(*p2);
			p1++;
		}
	}
	
	if(out)
	{
		out<<"The result of query \""<<query<<"\" is:"<<endl;
		if(!answer.empty())
		{
			for(set<unsigned int>::const_iterator it = answer.begin(); it!=answer.end(); ++it)
			{
				out<<doc_ID_log[*it-1]<<endl;
				out<<message_log[*it-1]<<endl;
				out<<directory_log[*it-1]<<endl;
			}
		}
		else
		{
			out<<"none!";
		}
	}
	cout<<"The result is in "<<file<<endl;
	
}