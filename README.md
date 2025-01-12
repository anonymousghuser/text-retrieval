<style>
table {
margin: auto;
}
</style>

# text retrieval
>### Implemented a simple search engine supporting *boolean retrieval*  on *zone index* returning *ranked* documents by *posting list*.
<br></br>  
## Boolean Retrieval Model  
- A model for information retrieval supporting query in the form of a Boolean expression of terms  which   are combined with the operators AND, OR, and NOT.
- The model views each document as just a *set* of words.　( —— *similar to Bag of words (BoW) model* )
### An example information retrieval problem  
>to determine which plays of Shakespeare contain the words
Brutus AND Caesar AND NOT Calpurnia

- term-document incidence matrix
  *  Each document is identified by an index in advance to avoid linearly scanning the texts for each query.
  * For each document ( *here a play of Shakespeare’s* ), whether it contains each word out of all the words Shakespeare used is recorded.
  * The result is a binary term-document incidence matrix, as in Figure 1. 
<h4 align="center"><b><i>term-document incidence matrix</b></i></h4>

|            |                  |                      |               |             |        |         |         |     |
| :--------: | :--------------- | -------------------- | ------------- | ----------- | ------ | ------- | ------- | --- |
|            | *__DOC (PLAY)__* | Antony and Cleopatra | Julius Caesar | The Tempest | Hamlet | Othello | Macbeth | ... |
| *__TERM__* |                  |                      |               |             |        |         |         |
|   Antony   |                  | 1                    | 1             | 0           | 0      | 0       | 1       |     |
|   Brutus   |                  | 1                    | 1             | 0           | 1      | 0       | 0       |     |
|   Caesar   |                  | 1                    | 1             | 0           | 1      | 1       | 1       |     |
| Calpurnia  |                  | 0                    | 1             | 0           | 0      | 0       | 0       |     |
| Cleopatra  |                  | 1                    | 0             | 0           | 0      | 0       | 0       |     |
|   mercy    |                  | 1                    | 0             | 1           | 1      | 1       | 1       |     |
|   worser   |                  | 1                    | 0             | 1           | 1      | 1       | 0       |     |
|    ...     |

##### <center> ◮ Figure 1　A term-document incidence matrix. Matrix element (*t*, *d*) is 1 if the play in column d contains the word in row t, and is 0 otherwise.</center>  
<br></br>
- query Brutus AND Caesar AND NOT Calpurnia
  -  The matrix rows or columns perspective provide a vector for each term, which shows the documents it appears in; or a vector for each document, showing the terms that occur in it.
  - To answer the query Brutus AND Caesar AND NOT Calpurnia, we take the
vectors for Brutus, Caesar and Calpurnia, complement the last, and then do a
bitwise AND:
110100 AND 110111 AND 101111 = 100100
  - The answers for this query are thus Antony and Cleopatra and Hamlet.  
> term-document incidence matrix is often extremely sparse with few non-zero entries.
> >  A much better representation is to record only the things
that do occur, that is, the 1 positions.
<br></br>
## Inverted Index
- An index always maps back from terms to the parts of a document where they occur.
- Sometimes also named inverted file.

|                                                  |                                                                                            |     |     |     |     |     |     |     |     |     |
| ------------------------------------------------ | ------------------------------------------------------------------------------------------ | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Brutus                                           | $\rightarrow$                                                                              | 1   | 2   | 4   | 11  | 31  | 45  | 173 | 174 | ... |
| Caesar                                           | $\rightarrow$                                                                              | 1   | 2   | 4   | 5   | 6   | 16  | 57  | 132 |     |
| Calpurnia                                        | $\rightarrow$                                                                              | 2   | 31  | 54  | 101 |     |     |     |     |     |
| ...                                              |
| <tr> <td>$\underbrace{　　　}_{Dictionary}$</td> | <td colspan="9">$\underbrace{ 　　　　　　　　　　　　　　　　　　}_{Postings}$</td> </tr> |

##### <center> ◮Figure 2　The two parts of an inverted index. The dictionary is commonly kept in memory, with pointers to each postings list, which is stored on disk.</center>
<br></br>
### An example inverted index construction  
> *Doc 1*  
<kbd>I did enact Julius Caesar: I was killed i’ the Capitol; Brutus killed me.</kbd>  
*Doc 2*  
<kbd>So let it be with Caesar. The noble Brutus hath told you Caesar was ambitious:</kbd>  


- To build the index, the major steps in this are:

  1. Collect the documents to be indexed:  
<kbd>Friends, Romans, countrymen.</kbd> <kbd>So let it be with Caesar</kbd> <kbd>. . .</kbd> 
     - Within a document collection, we assume that each document has a unique serial number, known as the document identifier (*docID*).
     -  During index construction, we can simply assign successive integers to each new document when it is first encountered.
  2. Tokenize the text, turning each document into a list of tokens:  
  <kbd>Friends Romans countrymen So . . .</kbd>  
  3. Index the documents that each term occurs in by creating an inverted index, consisting of a dictionary and postings.
     1. The input to indexing is a list of normalized tokens for each document, which we can equally think of as a list of **pairs of *term* and *docID***, as in Figure 3 below.
        - The core indexing step is **sorting** this list so that the **terms** are alphabetical. 
     2. Multiple occurrences of the same term from the same
document are then merged.
     1. Instances of the same term are then grouped,
and the result is split into a *__dictionary__* and ***postings***, as shown in the  Figure 4.
- The dictionary also records some statistics, such as the number of documents which contain each term (the document frequency, which is here  also the length of each postings list).
> Since a term generally occurs in a number of documents, this data organization already reduces the storage requirements of the index. 

| term      | docID |     | term      | docID |     |     |     |     |     |     |     |
| --------- | ----- | --- | --------- | ----- | --- | --- | --- | --- | --- | --- | --- |
| I         | 1     |     | ambitious | 2     |     |     |     |     |     |     |     |
| did       | 1     |     | be        | 2     |     |     |     |     |     |     |     |
| enact     | 1     |     | **brutus**    | 1     |     |     |     |     |     |     |     |
| julius    | 1     |     | **brutus**    | 2     |     |     |     |     |     |     |     |
| ***caesar***    | 1     |     | capitol   | 1     |     |     |     |     |     |     |     |
| I         | 1     |     | ***caesar***    | 1     |     |     |     |     |     |     |     |
| was       | 1     |     | ***caesar***    | 2     |     |     |     |     |     |     |     |
| killed    | 1     |     | ***caesar***    | 2     |     |     |     |     |     |     |     |
| i’        | 1     |     | did       | 1     |     |     |     |     |     |     |     |
| the       | 1     |     | enact     | 1     |     |     |     |     |     |     |     |
| capitol   | 1     |     | hath      | 1     |     |     |     |     |     |     |     |
| **brutus**    | 1     |     | I         | 1     |     |     |     |     |     |     |     |
| killed    | 1     |     | I         | 1     |     |     |     |     |     |     |     |
| me        | 1     | =⇒  | i’        | 1     |     |     |     |     |     |     |     |
| so        | 2     |     | it        | 2     |     |     |     |     |     |     |     |
| let       | 2     |     | julius    | 1     |     |     |     |     |     |     |     |
| it        | 2     |     | killed    | 1     |     |     |     |     |     |     |     |
| be        | 2     |     | killed    | 1     |     |     |     |     |     |     |     |
| with      | 2     |     | let       | 2     |     |     |     |     |     |     |     |
| ***caesar***    | 2     |     | me        | 1     |     |     |     |     |     |     |     |
| the       | 2     |     | noble     | 2     |     |     |     |     |     |     |     |
| noble     | 2     |     | so        | 2     |     |     |     |     |     |     |     |
| **brutus**    | 2     |     | the       | 1     |     |     |     |     |     |     |     |
| hath      | 2     |     | the       | 2     |     |     |     |     |     |     |     |
| told      | 2     |     | told      | 2     |     |     |     |     |     |     |     |
| you       | 2     |     | you       | 2     |     |     |     |     |     |     |     |
| ***caesar***    | 2     |     | was       | 1     |     |     |     |     |     |     |     |
| was       | 2     |     | was       | 2     |     |     |     |     |     |     |     |
| ambitious | 2     |     | with      | 2     |     |     |     |     |     |     |     |

##### <center> ◮Figure 3　Building an index by sorting and grouping. The sequence of terms in each document, tagged by their documentID (left) is sorted alphabetically (right). Instances of the same term are then grouped by word and then by documentID.</center>

| <kbd>term</kbd> <kbd>doc. freq.</kbd> |     | $\rightarrow$ | postings<br>lists</br> |               |      |     |     |
| ------------------------------------: | --- | ------------- | :--------------------: | :------------ | ---: | --- | --- |
| <kbd>ambitious</kbd>   <kbd> 1 </kbd> |     | $\rightarrow$ |           2            |               |      |     |     |
|    <kbd>be</kbd>    <kbd>  1   </kbd> |     | $\rightarrow$ |           2            |               |      |     |     |
|   <kbd>brutus</kbd>    <kbd>  2</kbd> |     | $\rightarrow$ |           1            | $\rightarrow$ |    2 |     |     |
|  <kbd>capitol</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           1            |               |      |     |     |
|   <kbd>caesar</kbd>    <kbd>  2</kbd> |     | $\rightarrow$ |           1            | $\rightarrow$ |    2 |     |     |
|      <kbd>did</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           1            |               |      |     |     |
|    <kbd>enact</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           1            |               |      |     |     |
|     <kbd>hath</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           2            |               |      |     |     |
|        <kbd>I</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           1            |               |      |     |     |
|       <kbd>i’</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           1            |               |      |     |     |
|       <kbd>it</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           2            |               |      |     |     |
|   <kbd>julius</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           1            |               |      |     |     |
|   <kbd>killed</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           1            |               |      |     |     |
|      <kbd>let</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           2            |               |      |     |     |
|       <kbd>me</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           1            |               |      |     |     |
|    <kbd>noble</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           2            |               |      |     |     |
|       <kbd>so</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           2            |               |      |     |     |
|      <kbd>the</kbd>    <kbd>  2</kbd> |     | $\rightarrow$ |           1            | $\rightarrow$ |    2 |     |     |
|     <kbd>told</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           2            |               |      |     |     |
|      <kbd>you</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           2            |               |      |     |     |
|      <kbd>was</kbd>    <kbd>  2</kbd> |     | $\rightarrow$ |           1            | $\rightarrow$ |    2 |     |     |
|     <kbd>with</kbd>    <kbd>  1</kbd> |     | $\rightarrow$ |           2            |               |      |     |     |


##### <center> ◮Figure 4　The terms and documentIDs are then separated out. The dictionary stores the terms, and has a pointer to the postings list for each term. It commonly also stores other summary information such as, here, the document frequency of each term. We use this information for improving query time efficiency and, later, for weighting in ranked retrieval models. Each postings list stores the list of documents in which a term occurs, and may store other information such as the term frequency (the frequency of each term in each document) or the position(s) of the term in each document.</center>
