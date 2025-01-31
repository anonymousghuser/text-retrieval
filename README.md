
# text retrieval
>### Implemented a simple search engine supporting *boolean retrieval*  on *zone index* returning *ranked* documents by *posting list*.
<br>


- 工程说明：
    > 数据集: *Enron Email Dataset* 

    > 工程环境： visual studio
  - TheoriesAndResources 目录：介绍理论背景，列举（部分）参考资料。
  - SampleProject 目录：存放工程原始及复现环境。可使用 visual studio 执行 Retrieval.sln 进入工程。
  - src 目录：存放 step 1 数据预处理工程 BrowseDir，step 2 文档遍历读取及分析处理工程 DataRetrieval 的源文件。
  - frequency 目录：针对支持 *域索引* 场景，对"From"、"To"、"Subject" 以及 "Body" 四个域，统计所有词条出现的频率，依降序列出前50个高频词。
  - PostingList 目录：倒排索引数据.
  - Result 目录：布尔检索结果。
    > 包括仅反映查询的词项是否在文档中出现、未对文档进行排序的布尔查询，和支持 AND 查询并进行文档排序的结果（在 query_and_sort_result.txt ）。 

- 样例演示步骤与实现说明：

## step 0: data preprocess
> 内存等资源所限，暂无法对完整数据集建立倒排索引，此处采取部分索引，对数据集中a~g文档进行预处理，共涉及 112770 个文档。

0. 首先运行工程 BrowseDir 对所有文档进行遍历，获得它们所在的路径，存放在文本文件 directory.txt 中。

## step 1: tokenization & zone index construction
1. 
   a. 运行工程 DataRetrieval 依次遍历以上所有文档，读取并分析文档的内容。
   使用函数 ` void delete_separators(string &,const string &); ` 去除文档中的分隔符，完成词条化。 

   b. 对"From"、"To"、"Subject" 以及"body"(邮件正文) 四个域分别统计每个词条出现的频率。对于每一个域，显示出最常用的 50 个词，按照频率由高到低排列。
   
   对于"From"、"To"两个域，单词"com"作为停用词。
   
   使用的函数为：`ofstream& write_sort_map(ofstream& out, const string &file, const map<string,unsigned int>&);`
   
   结果存放在 from_frequency.txt、to_frequency.txt、subject_frequency.txt 和 body_frequency.txt 四个文本文件中。
   
## step 2: inverted index construction
2. 
   a. 对 "From"、"To"、"Subject"、"Date"、"body"五个域，分别建立倒排索引。为后续的基于向量模型的查询作准备，应在倒排索引中加入词项频率信息，使用如下结构体变量作为一个倒排记录：
   ```
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
   } element; //倒排记录结构体:(文档 ID,词项频率)
   ```

   b. 通过分析文档内容，发现文档的唯一标识是 Message-ID；为了便于建立索引，要对文档名映射成文档 ID；为了后续查询时能够快速找到文档，还需要保存文档路径信息。
   
   因此，采用如下变量记录以上三类重要信息
   ```
   //文档ID 文档Message-ID 和文档路径对
   vector<string> doc_ID_log;
   vector<string> message_log;
   vector<string> directory_log;
   ```
   使用函数 `ofstream& write_doc_map(ofstream&, const string&, vector<string> &, vector<string> &,vector<string> &);` 将以上三类信息存放在文本文件 doc_map.txt 中。

   再使用函数 `ofstream& write_file(ofstream&, const string&, const map<string,set<element> > &);` 将倒排索引数据写入硬盘，数据存放在 doc_date.txt、doc_from.txt、doc_to.txt、doc_subject.txt 和 doc_body.txt 五个文本文件中。
   
   以上五个文本文件的数据存储结构如下：
   
   词项 文档 ID1 词项频率 1 文档 ID2 词项频率 2 文档 ID3 词项频率 3......

   c. 使用函数 `ifstream& read_doc_map(ifstream&, const string&, vector<string> &, vector<string> &,vector<string> &);` 将文档 ID，文档标识 Message-ID 和文档路径三类信息读取到内存，便于查询结果的输出。
   
   d. 使用函数 `ifstream& read_file(ifstream&, const string&, map<string,set<element> > &);` 将硬盘中的倒排索引数据读取到内存，数据存放在 `map<string, set<element> > postinglist[5];` 之中。

   e. 变量 `new_postinglist` 用于设置是否重新建立倒排索引
   ```
   new_postinglist = 1；重新遍历文档建立倒排索引
   new_postinglist = 0；从硬盘中读取已经建立好的倒排索引
   ```
   由于实验数据集比较大，每次查询时重新建立倒排索引是很费时的，所以一旦倒排索引建立好之后，下次查询时可以从硬盘中读取已建立好的倒排索引数据至内存，节省时间。

## Demonstration: *boolean retrieval*  on *zone index*
> 实现简单的布尔查询，支持 AND，OR 和 NOT。

使用以下三个函数
```
void and_query(ofstream& out, const string &file, const map<string,set<element> > *, vector<string> &, vector<string> &,vector<string> &);

void or_query(ofstream& out, const string &file,const map<string,set<element> > *, vector<string> &, vector<string> &,vector<string> &);

void not_query(ofstream& out, const string &file,const map<string,set<element> > *, vector<string> &, vector<string> &,vector<string> &);
```
可以分别完成 AND、OR 和 NOT 的布尔查询。

对于 AND 查询的键盘输入格式如下：

enron.to and in.body

结果存放在 query_and_result.txt 中.

以上结果只是反映查询的词项是否在文档中出现，并没有对文档进行排序。

## Demonstration: *sorted* results
> 这里的排序使用的是结合**域加权**和**向量模型**的方法。

1. 设定"Date"、"To"、"From"、"Subject"以及"body"的域权重分别为 0.05、0.1、0.1、0.4 和 0.35
   
2. 对于每个域，计算以词项频率为权重的向量内积。
   

   $$
   d_i = \vec{V}(q) \cdot \vec{V_i}(d)
   $$
   
   
   再使用如下公式进行域加权计算最终的文档得分
   
   
   $$
   s = \sum_{i=1}^{5} w_i d_i
   $$

   
   
   $w_i$ 表示域权重， $d_i$  表示每个域的向量内积的值。
   
3. 按文档得分s 从高到低进行文档排序。
   
   使用函数 `void and_sort_query(ofstream& out, const string &file, const map<string,set<element> > *, vector<string> &, vector<string> &,vector<string> &);` 完成 AND 查询并进行文档排序。
   
   键盘输入格式为：enron and in
   
   结果存放在 query_and_sort_result.txt 中.