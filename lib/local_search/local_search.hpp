/**
 * @file local_search.hpp
 * @brief Costam
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

/*! \page local_search Local Search The PAAl.Local Search  Library
Index:
<ul>
    <li> \ref Preliminaries - containing preliminaries to Local Search and our interpretation of it.
    <li> LOCAL SEARCH INTERFACE - containing c++ interfaces of Local Search classes.
    <li> SPECIFIC IMPLEMENTATIONS - containing the general idea of the library.
    <li> 2-opt for TSP  - containing the implementation of the 2_local_search for TSP and basic usage of this class
</ul>


\section Preliminaries 
Let us consider maximization problem	: <br><br>

max f(x) over x in X. <br><br>

A well known heuristic for solving problem formulated in such a way is the local search(LS). <br>
Assume that we have feasible solution x' in X. <br>
The LS algorithm searches the neighborhood N(x') of x' and tries to localize the better solution x'' with f(x'') > f(x'). <br>
If the better solution is found we set x' equal x'' and rerun previous step. <br>
If better solution couldn't be found we finish the search with resulting local optimum x*. <br><br>

This algorithm can be repeated and the best local optimum is presented. <br><br>

Let us write the pseudo code for this operation:  <br><br>

<font size=4>
<pre>
 local_search() 
 {
     x -> random_solution(X)  
     for_each(Update u in N(x)) 
     { 
         if(gain(apply u on x) > 0) 
         {
             x -> apply u on x 
         }	
     }
     return x
 }
</pre>
</font>

Note that we are working on updates (not on the full solution). This idea is going to be used in the c++ code. <br> 
The reason of this is the fact that usually the updates are much lighter than the full solutions. <br>
<br>
Note that in many cases, eg. k-local search or facility location, our solution is  the collection of the elements.<br>
In many cases for such solutions we search the neighborhood of each solution element and try to improve it by changing some part of the solution near the chosen solution element. <br>
<br>
In this cases it is more convenient to proceed in the following way:<br>

<font size=4>
<pre>
 local_search() 
 {
     x -> random_solution(X) 
     for_each(Element e of x)
     {
      for_each(Update u in N(e)) 
      { 
         if(gain(apply u on x) > 0) 
         {
             x -> apply u on x 
         }	
      }
     }
     return x
 }
</pre>
</font>
<br>
We find this algorithm schema  extremely useful in our implementation! <br>
We will refer to this schema as LocalSearchMultiSolution. <br>
Also if necessary we will refer to the "normal" LS as LocalSearchSingleSolution.

\section local_search_interface  LOCAL SEARCH INTERFACE 
\subsection loacl_search_multi LOCAL SEARCH SINGLE SOLUTION 

In order to present the LS interface we need to introduce several concepts.<br>
Note that <i>Solution</i> is the solution type and <i>Update</i> is the type of single update. <br>
<i>UpdateIteratorsRange</i> is assumed to be std::pair of iterators, pointing to the begin and end of the updates collection. <br><br>

Concepts:
<ol>
    <li> <i>NeighborhoodGetter</i>  is a concept class responsible for getting the neighborhood of the current solution  
    <pre>
    NeighborhoodGetterArchetype {
        UpdateIteratorsRange get(Solution & s)
    }
    </pre>
    <li> <i>ImproveChecker</i> is a concept class responsible for checking if the specific update element improve the solution.
    <pre>
    ImproveCheckerArchetype {
        int gain(Solution & s, const Update & update);
    }
    </pre>
    <li> <i>SolutionUpdater</i> is a concept class responsible for updating the solution with the Update.
    <pre>
    SolutionUdaterArchetype {
        int update(Solution & s, const Update & update);
    }
    </pre>

</ol>


Now we present the interface of the LS algorithm

<font size=4>
<pre>

template &lttypename Solution, 
          typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater, 
          typename SearchStrategy = search_startegies::ChooseFirstBetter> 
                        // Search strategy, descibes LS search strategy. For ow we are planning two strategies:
                        // ChooseFirstBetter -> The algorithm chooses the first update with the positive gain
                        // SteepestSlope     -> The algorithm chooses the update with the largest gain and update if positive.
          //note there is no Update here because it can be deduced    <font color="red"> maybe it should be contained... </font>
class LocalSearchStep { // <font color="red"> now we present only one step of the algorithm, later on we plan to add one more layer </font> 
public:
   LocalSearchStep(Solution solution, 
                   NeighborhoodGetter ng, 
                   ImproveChecker check, 
                   SolutionUpdater solutionUpdater); 
    
    bool search(); // performing one step of the search
    Solution & getSolution();
};

</pre>
</font>
<br>


\include local_search_multi_solution.hpp

<h4> LOCAL SEARCH MULTI SOLUTION </h4>

The interface and conceptes of the  LocalSearchMultiSolution are ver simmilar to the LocalSearchSingleSolution ones.<br>


<br>
Note that <i>SolutionElement</i> is the type of the specific element of the solution. <br>
<i>SolutionElementIterator</i> is assumed to be the type of iterators over solution. <br><br>

Concepts:
<ol>
    <li> <i>MultiSolution</i>  is a concept class representing the  solution  
    <pre>
    MultiSolutionArchetype {
        SolutionElementIterator begin();
        SolutionElementIterator end();
        InnerSolution get(); // OPTIONAL, very often solution concept is just adapter containing real solution, 
                             // The inner solution type isInnerSolution
    }
    <li> <i>MultiNeighborhoodGetter</i>  is a concept class responisble for getting the neighborhood of the current solution  
    <pre>
    MultiNeighborhoodGetterArchetype {
        UpdateIteratorsRange get(Solution & s, const SolutionElement &)
    }
    </pre>
    <li> <i>MultiImproveChecker</i> is a concept class responsible for checking if the specific update element improve the solution.
    <pre>
    MultiImproveCheckerArchetype {
        int gain(Solution & s, const SolutionElement &, const Update & update);
    }
    </pre>
    <li> <i>MultiSolutionUpdater</i> is a concept class responsible for updating the solution with the Update.
    <pre>
    MultiSolutionUdaterArchetype {
        int update(Solution & s, const SolutionElement &, const Update & update);
    }
    </pre>

</ol>


Now we present the interface of the LSMultiSolution algorithm

<font size=4>
<pre>

template &lttypename MultiSolution, 
          typename MultiNeighborhoodGetter, 
          typename MultiImproveChecker, 
          typename MultiSolutionUpdater, 
          typename MultiSearchStrategy = search_startegies::ChooseFirstBetter> 
                        // Search strategy, describes LS search strategy. For now we are planning two strategies:
                        // ChooseFirstBetter -> The algorithm chooses the first update with the positive gain
                        // SteepestSlope     -> The algorithm chooses the update with the largest gain and update if positive.
          //note there is no Update and SolutionElement here because they can be deduced    <font color="red"> maybe it should be contained... </font>
class LocalSearchStepMultiSolution { // <font color="red"> now we present only one step of the algorithm, later on we plan to add one more layer </font> 
   
public:
   LocalSearchStep(MultiSolution solution, 
                   MultiNeighborhoodGetter ng, 
                   MultiImproveChecker check, 
                   MultiSolutionUpdater solutionUpdater); 
    
    bool search();// performing one step of the search
    InnerSolution & getSolution(); // is here only if MultiSolution has get member function.
};

</pre>
</font>
<br>

<h3> 2-opt FOR TSP </h3>

Now we can draw for you interface and the example implementation and usage of 2_local_search for TSP. <br><br>

In order to present 2-opt we need to introduce some concept and types. 

<ol>
<li> VertexType - denotes type of the TSP vertex.
<li> Metric need to have fulfill the interface: 
<pre>
    MetricArchetype {
        DistanceType operator()(VertexType,VertexType);
    }
</pre> 

<li>  Cycle need to have fulfill the interface: 
<pre>
    CycleArchetype {
        void flip(VertexType,VertexType);
        EdgeIteratorRange getEdgesRange();
    }
</pre> 
</ol>

Now we can present implementattion of 2-local-search. <br>
Note that in the main algorithm we had to define only constructors.<br>
The rest is done by base class LocalSearchMultiSolution and the implemented helpers.

<font size=4>
<pre>
template &lttypename VertexType, 
           typename Metric, //<font color="red"> to consistent it should be the template as well </font>
           typename NeighborhoodGetter = TrivialNeigbourGetter,
           template <class> class ImproveChecker = ImproveChecker2Opt,
           template <class> class Cycle = data_structures::SimpleCycle>
 
          class  TwoLocalSearchStep : 
              public LocalSearchStepMultiSolution<TwoLocalSearchContainer<Cycle<VertexType>>, 
                         NeighborhoodGetter, ImproveChecker<Metric>, TwoLocalSearchUpdater >  {
 
                
                 typedef Cycle<VertexType> CycleT;
                 typedef LocalSearchStepMultiSolution<TwoLocalSearchContainer<CycleT> , NeighborhoodGetter, 
                     ImproveChecker<Metric>, TwoLocalSearchUpdater > LocalSearchStepT;
 
                 public:
 
                     template <typename SolutionIter>  
                      TwoLocalSearchStep(SolutionIter solBegin, SolutionIter solEnd, 
                                         Metric & m, 
                                         ImproveChecker<Metric> ich,
                                         NeighborhoodGetter ng = NeighborhoodGetter()
                                         ) 
 
                         : LocalSearchStepT(TwoLocalSearchContainer<CycleT>(m_cycle), std::move(ng), 
                                                 std::move(ich), TwoLocalSearchUpdater()),
                                                 m_cycle(solBegin, solEnd) {}
                     
                      TwoLocalSearchStep(Cycle<VertexType> c, 
                                         Metric & m, 
                                         ImproveChecker<Metric> ich,
                                         NeighborhoodGetter ng = NeighborhoodGetter()
                                         ) 
 
                        :   LocalSearchStepT(TwoLocalSearchContainer<CycleT>(m_cycle), std::move(ng), 
                                             std::move(ich), TwoLocalSearchUpdater()),
                                             m_cycle(std::move(c)) {}
                                      
         private:
            CycleT m_cycle;
};     
</pre>
</font>

Basic usage of this algorithm is extremely simple and elegant. <br> We are using some helper functions from the library.

<font size=4>
<pre>
      //assume that graph_t g is some graph from boost graph library

      typedef GraphMetric&ltgraph_t, int> GraphMT;
      GraphMT gm(g); // gm is a graph metric for graph g

      //create random solution <font color="red">// This is going to be contained in the library!</font>
      std::vector&ltint> ver = {A, B, C, D, E};
      std::random_shuffle(ver.begin(), ver.end()); 

      //creating of local search
      ImproveChecker2Opt&ltGraphMT> checker(gm);
      TwoLocalSearchStep&ltint, GraphMT> ls(ver.begin(), ver.end(), gm, checker);
      auto const & cman = ls.getSolution();

      //printing 
      std::cout << "Length " << simple_algo::getLength(gm, cman) << std::endl;
      simple_algo::print(cman, std::cout);

      //search
      ls.search();

      //printing after search
      simple_algo::print(cman, std::cout);
      std::cout << "Length " << simple_algo::getLength(gm, cman) << std::endl;

</pre>
</font>

Although the basic usage is very simple, the sophisticated user can still easily change default parameters and exchange them with his ones. <br><br><br><br><br><br>


</body>


</html>
 
 \tableofcontents
 Leading text.
 \section sec An example section2
 This page contains the subsections \ref subsection1 and \ref subsection2.
 For more info see page \ref page2.
 \subsection subsection1 The first subsection
 Text.
 \subsection subsection2 The second subsection
 More text.
 */

 /*! \page page2 Another page
 Even more info.
 */
#include "local_search_single_solution.hpp"
#include "local_search_multi_solution.hpp"
